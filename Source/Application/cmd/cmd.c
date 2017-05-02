/**************************************************************************************************
  Filename:       cmd.c
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:
**************************************************************************************************/
//Task_sleep(100000);//100000 = 1s
/*********************************************************************
 * INCLUDES
 */
#ifdef USE_ICALL
#include <ICall.h>
#else
#include <stdlib.h>
#endif

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/uart/UARTCC26XX.h>
#include "bcomdef.h"
#include "board.h"
#include "ringbuff.h"
#include "cmd.h"
#include "cust_nv.h"
#include "doorlockservice.h"

/*********************************************************************
 * CONSTANTS
 */
#define LOG FALSE
#define LOG_WRITE_WAIT UART_WAIT_FOREVER//500//5000 = 50ms//UART_WAIT_FOREVER

// Task stack size
#ifndef CMD_TASK_STACK_SIZE
#define CMD_TASK_STACK_SIZE 800
#endif

#ifndef CMD_RXTASK_STACK_SIZE
#define CMD_RXTASK_STACK_SIZE 400
#endif
// Task configuration
#define CMD_TASK_PRIORITY 1

#define CMD_BUFF_SIZE 128

#define UART_BUAD 115200

#define UART_RX_BUF_SIZE 32

#ifdef POWER_SAVING
// Indexes for pin configurations in PIN_Config array
#define REM_RDY_PIN_IDX      0
#define LOC_RDY_PIN_IDX      1

#define LocRDY_ENABLE()      PIN_setOutputValue(hCmdHandshakePins, locRdyPIN, 0)
#define LocRDY_DISABLE()     PIN_setOutputValue(hCmdHandshakePins, locRdyPIN, 1)
#else
#define LocRDY_ENABLE()
#define LocRDY_DISABLE()
#endif //POWER_SAVING

#define STR_LEN_VALID_PIN (11 + PIN_LEN)

#define STR_REQ_AT "AT"
#define STR_REQ_HANDSHAKE "AT\r\n"
#define STR_SET_PIN "AT+PIN=\"%[^\"]\"\r\n"
#define STR_RES_PIN "+PIN=\"%s\"\r\n"
#define STR_FORMAT_PIN "AT+PIN=\"\"\r\n"
#define STR_SET_DATAMODE "AT+DATAMODE=%d\r\n"
#define STR_RES_DATAMODE "+DATAMODE=%d\r\n"
#define STR_FORMAT_DATAMODE "AT+DATAMODE=(0,1,2)\r\n"
#define STR_SET_TIMEZONE "AT+TIMEZONE=%d\r\n"
#define STR_RES_TIMEZONE "+TIMEZONE=%d\r\n"
#define STR_FORMAT_TIMEZONE "AT+TIMEZONE=(-12 - 12)\r\n"
#define STR_SET_MOTORMODE "AT+MOTORMODE=%d\r\n"
#define STR_RES_MOTORMODE "+MOTORMODE=%d\r\n"
#define STR_FORMAT_MOTORMODE "AT+MOTORMODE=(1,0)\r\n"
#define STR_SET_MOTORTIME "AT+MOTORTIME=%d\r\n"
#define STR_RES_MOTORTIME "+MOTORTIME=%d\r\n"
#define STR_FORMAT_MOTORTIME "AT+MOTORTIME=(4 - 127)\r\n"
#define STR_SET_OFFICEMODE "AT+OFFICEMODE=%d\r\n"
#define STR_RES_OFFICEMODE "+OFFICEMODE=%d\r\n"
#define STR_FORMAT_OFFICEMODE "AT+OFFICEMODE=(1,0)\r\n"
#define STR_SET_OFFICETIME "AT+OFFICETIME=%d\r\n"
#define STR_RES_OFFICETIME "+OFFICETIME=%d\r\n"
#define STR_FORMAT_OFFICETIME "AT+OFFICETIME=(2 - 7)\r\n"
#define STR_SET_HBINTERVAL "AT+HBINTERVAL=%d\r\n"
#define STR_RES_HBINTERVAL "+HBINTERVAL=%d\r\n"
#define STR_FORMAT_HBINTERVAL "AT+HBINTERVAL=(0 - 3)\r\n"

#define STR_RES_VERSION "+VERSION=%d.%d.%d\r\n"
#define STR_RES_MODELNO "+MODELNO=%s\r\n"
#define STR_RES_MODELDESC "+MODELDESC=%s\r\n"

#define STR_RES_OK "OK\r\n"
#define STR_RES_ERROR "ERROR\r\n"

/*********************************************************************
 * TYPEDEFS
 */
typedef enum
{
	CMD_INIT = 0,
	CMD_PRERESUME,
	CMD_RESUME,
	CMD_RUN,
	CMD_PRESUSPEND,
	CMD_SUSPEND,
	CMD_DEINIT
} cmdState_e;

typedef void (*CMD_PROCESS_HANDLER)(
	uint8_t idx,
    char *cmd
);

typedef struct{
    char *cmd;
    CMD_PROCESS_HANDLER handler;
} cmdTable_t;

typedef enum{
	CMD_GET = 0,
	CMD_SET,
	CMD_FORMAT,
	CMD_UNKNOWN
} cmdType_e;

/*********************************************************************
 * LOCAL VARIABLES
 */
#if (GL_LOG)
static char log[100] = {0};
#endif

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Semaphore globally used to post events to the application thread
static ICall_Semaphore sem;

static UART_Handle uartHandle;
static Semaphore_Struct cmdTxSem = {0};
static Semaphore_Struct cmdRxSem = {0};
static Semaphore_Struct cmdCmdSem = {0};
static uint8_t rxBuf[UART_RX_BUF_SIZE] = {0};
static size_t cmdRxSize = 0;
static RingBuff *cmdRxBuf = 0;

static volatile bool cmdPMSetConstraint = false;
static volatile bool cmdActive = false;

// Task configuration
static Task_Struct cmdRxTask;
static Task_Struct cmdTask;
static char cmdRxTaskStack[CMD_RXTASK_STACK_SIZE];
static char cmdTaskStack[CMD_TASK_STACK_SIZE];
static cmdState_e cmdState = CMD_INIT;

#ifdef POWER_SAVING
static Semaphore_Struct cmdSBSem = {0};

//! \brief PIN Config for Mrdy and Srdy signals without PIN IDs
static PIN_Config cmdHandshakePinsCfg[] =
{
    PIN_GPIO_OUTPUT_DIS | PIN_INPUT_EN | PIN_PULLUP,
    PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

static uint32_t remRdyPIN = (IOID_UNUSED & IOC_IOID_MASK);
static uint32_t locRdyPIN = (IOID_UNUSED & IOC_IOID_MASK);

//! \brief PIN State for remRdy and locRdy signals
static PIN_State cmdHandshakePins;

//! \brief PIN Handles for remRdy and locRdy signals
static PIN_Handle hCmdHandshakePins;

static uint8_t remRdy_state;

#endif //! POWER_SAVING

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void uart_readCallBack(UART_Handle handle, void *buf, size_t size);
static void uart_writeCallBack(UART_Handle handle, void *buf, size_t size);
static void cmd_openUart(void);
static void cmd_closeUart(void);
static int cmd_write(const void *buffer, size_t size);
static void cmd_taskRxFxn(UArg a0, UArg a1);
static void cmd_taskFxn(UArg a0, UArg a1);

//command handle
static void cmd_hdlFxn(char *cmd);
static cmdType_e cmd_getType(uint8_t idx, char *cmd);
static void cmd_onRestore(uint8_t idx, char *cmd);
static void cmd_onPin(uint8_t idx, char *cmd);
static void cmd_onDataMode(uint8_t idx, char *cmd);
static void cmd_onTimeZone(uint8_t idx, char *cmd);
static void cmd_onMotorMode(uint8_t idx, char *cmd);
static void cmd_onMotorTime(uint8_t idx, char *cmd);
static void cmd_onOfficeMode(uint8_t idx, char *cmd);
static void cmd_onOfficeTime(uint8_t idx, char *cmd);
static void cmd_onHBInterval(uint8_t idx, char *cmd);
static void cmd_onVersion(uint8_t idx, char *cmd);
static void cmd_onModelNumber(uint8_t idx, char *cmd);
static void cmd_onModelDesc(uint8_t idx, char *cmd);

#ifdef POWER_SAVING
//! \brief HWI interrupt function for remRdy
static void cmd_remRdyPINHwiFxn(PIN_Handle hPin, PIN_Id pinId);
static void cmd_setPM(void);
static void cmd_relPM(void);
#endif //POWER_SAVING

/*********************************************************************
 * LOCAL VARIABLES
 */
static cmdTable_t cmdTable[] =
{
	{"+RESTORE", cmd_onRestore},
	{"+PIN", cmd_onPin},
	{"+DATAMODE", cmd_onDataMode},
	{"+TIMEZONE", cmd_onTimeZone},
	{"+MOTORMODE", cmd_onMotorMode},
	{"+MOTORTIME", cmd_onMotorTime},
	{"+OFFICEMODE", cmd_onOfficeMode},
	{"+OFFICETIME", cmd_onOfficeTime},
	{"+HBINTERVAL", cmd_onHBInterval},
	{"+VERSION", cmd_onVersion},
	{"+MODELNO", cmd_onModelNumber},
	{"+MODELDESC", cmd_onModelDesc},
	{NULL, NULL}
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return  None.
 */
void uart_readCallBack(UART_Handle handle, void *buf, size_t size)
{
	cmdRxSize = size;
	Semaphore_post(Semaphore_handle(&cmdRxSem));
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return  None.
 */
void uart_writeCallBack(UART_Handle handle, void *buf, size_t size)
{
	Semaphore_post(Semaphore_handle(&cmdTxSem));
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param   None.
 *
 * @return  None.
 */
void cmd_openUart(void)
{
	UART_Params params = {
		UART_MODE_CALLBACK,   /* readMode */
		UART_MODE_CALLBACK,   /* writeMode */
		UART_WAIT_FOREVER,    /* readTimeout */
		UART_WAIT_FOREVER,    /* writeTimeout */
		uart_readCallBack,    /* readCallback */
		uart_writeCallBack,   /* writeCallback */
		UART_RETURN_NEWLINE,  /* readReturnMode */
		UART_DATA_BINARY,       /* readDataMode */
		UART_DATA_BINARY,       /* writeDataMode */
		UART_ECHO_OFF,         /* readEcho */
		UART_BUAD,               /* baudRate */
		UART_LEN_8,           /* dataLength */
		UART_STOP_ONE,        /* stopBits */
		UART_PAR_NONE,        /* parityType */
		NULL                  /* custom */
	};

	UART_init();
	uartHandle = UART_open(Board_UART, &params);
	UART_control(uartHandle, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE, NULL);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param   None.
 *
 * @return  None.
 */
void cmd_closeUart(void)
{
	UART_close(uartHandle);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return  None.
 */
int cmd_write(const void *buffer, size_t size)
{
	Semaphore_pend(Semaphore_handle(&cmdTxSem), UART_WAIT_FOREVER);
	return UART_write(uartHandle, buffer, size);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return  None.
 */
#if (GL_LOG)
int Log_writeStr(const char *buffer)
{
	if(Semaphore_pend(Semaphore_handle(&cmdTxSem), LOG_WRITE_WAIT)){
		return UART_write(uartHandle, buffer, strlen(buffer));
	}
	else{
		return UART_ERROR;
	}
}

int Log_printf(const char *fmt, ...)
{
	if(Semaphore_pend(Semaphore_handle(&cmdTxSem), LOG_WRITE_WAIT)){
		va_list ptr = {0};
		va_start(ptr, fmt);
		vsprintf(log, fmt, ptr);
		return UART_write(uartHandle, log, strlen(log));
	}
	else{
		return UART_ERROR;
	}
}
#endif

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param   None.
 *
 * @return  None.
 */
void Cmd_createTask(void)
{
	Semaphore_Params semParams;
	Semaphore_Params_init(&semParams);
	semParams.mode = Semaphore_Mode_BINARY;
	Semaphore_construct(&cmdTxSem, 1, &semParams);
	Semaphore_pend(Semaphore_handle(&cmdTxSem), UART_WAIT_FOREVER);
	Semaphore_construct(&cmdRxSem, 1, &semParams);
	Semaphore_pend(Semaphore_handle(&cmdRxSem), UART_WAIT_FOREVER);
	Semaphore_construct(&cmdCmdSem, 1, &semParams);
	Semaphore_pend(Semaphore_handle(&cmdCmdSem), UART_WAIT_FOREVER);

#ifdef POWER_SAVING
	Semaphore_construct(&cmdSBSem, 1, &semParams);
	Semaphore_pend(Semaphore_handle(&cmdSBSem), UART_WAIT_FOREVER);
#endif //POWER_SAVING

	Task_Params taskParams;

	// Configure task
	Task_Params_init(&taskParams);
	taskParams.stack = cmdRxTaskStack;
	taskParams.stackSize = CMD_RXTASK_STACK_SIZE;
	taskParams.priority = CMD_TASK_PRIORITY;
	Task_construct(&cmdRxTask, cmd_taskRxFxn, &taskParams, NULL);

	Task_Params_init(&taskParams);
	taskParams.stack = cmdTaskStack;
	taskParams.stackSize = CMD_TASK_STACK_SIZE;
	taskParams.priority = CMD_TASK_PRIORITY;
	Task_construct(&cmdTask, cmd_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      cmd_taskRxFxn
 *
 * @brief   Application task entry point for the cmd.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void cmd_taskRxFxn(UArg a0, UArg a1)
{
	for(;;){
		switch(cmdState){
		case CMD_INIT:{
#ifdef POWER_SAVING
			remRdyPIN = (Board_MRDY_PIN & IOC_IOID_MASK);
			locRdyPIN = (Board_SRDY_PIN & IOC_IOID_MASK);

			// Add PIN IDs to PIN Configuration
			cmdHandshakePinsCfg[REM_RDY_PIN_IDX] |= remRdyPIN;
			cmdHandshakePinsCfg[LOC_RDY_PIN_IDX] |= locRdyPIN;

			// Initialize LOCRDY/REMRDY. Enable int after callback registered
			hCmdHandshakePins = PIN_open(&cmdHandshakePins, cmdHandshakePinsCfg);
			PIN_registerIntCb(hCmdHandshakePins, cmd_remRdyPINHwiFxn);
			PIN_setConfig(hCmdHandshakePins,
						  PIN_BM_IRQ,
						  remRdyPIN | PIN_IRQ_BOTHEDGES);

			// Enable wakeup
			PIN_setConfig(hCmdHandshakePins,
						  PINCC26XX_BM_WAKEUP,
						  remRdyPIN | PINCC26XX_WAKEUP_NEGEDGE);

			remRdy_state = PIN_getInputValue(remRdyPIN);
			if(!remRdy_state){
				cmd_setPM();
				cmdState = CMD_RESUME;
			}
			else{
				cmdState = CMD_SUSPEND;
			}
#else
			cmdState = CMD_PRERESUME;
#endif //POWER_SAVING
			cmdRxBuf = Ringbuff_init(CMD_BUFF_SIZE);
			break;
		}
		case CMD_PRERESUME:{
#ifdef POWER_SAVING
			Task_sleep(5000);//50ms
			remRdy_state = PIN_getInputValue(remRdyPIN);
			if(!remRdy_state){
				cmd_setPM();
				cmdState = CMD_RESUME;
			}
			else{
				cmdState = CMD_SUSPEND;
			}
#else
			cmdState = CMD_RESUME;
#endif
			break;
		}
		case CMD_RESUME:{
			if(!cmdActive){
				cmd_openUart();
				Semaphore_post(Semaphore_handle(&cmdTxSem));
				cmdActive = true;
			}
#if (GL_LOG && LOG)
			cmd_write("CMD_RESUME\r\n", strlen("CMD_RESUME\r\n"));
#endif
			LocRDY_ENABLE();
			cmdState = CMD_RUN;
			break;
		}
		case CMD_RUN:{
#if (GL_LOG && LOG)
			cmd_write("CMD_RUN\r\n", strlen("CMD_RUN\r\n"));
#endif
			cmdRxSize = 0;
			UART_read(uartHandle, rxBuf, UART_RX_BUF_SIZE);
			Semaphore_pend(Semaphore_handle(&cmdRxSem), UART_WAIT_FOREVER);
			if(cmdRxSize > 0){
				Ringbuff_write(cmdRxBuf, rxBuf, cmdRxSize);
				Semaphore_post(Semaphore_handle(&cmdCmdSem));
			}
			break;
		}
		case CMD_PRESUSPEND:{
#ifdef POWER_SAVING
			Task_sleep(5000);//50ms
			remRdy_state = PIN_getInputValue(remRdyPIN);
			if(!remRdy_state){
				cmdState = CMD_RESUME;
			}
			else{
				cmdState = CMD_SUSPEND;
			}
#else
			cmdState = CMD_SUSPEND;
#endif
			break;
		}
		case CMD_SUSPEND:{
			if(cmdActive){
#if (GL_LOG && LOG)
				cmd_write("CMD_SUSPEND\r\n", strlen("CMD_SUSPEND\r\n"));
#endif
				Semaphore_pend(Semaphore_handle(&cmdTxSem), UART_WAIT_FOREVER);
				cmd_closeUart();
				cmdActive = false;
			}
#ifdef POWER_SAVING
			LocRDY_DISABLE();
			cmd_relPM();
			Semaphore_pend(Semaphore_handle(&cmdSBSem), UART_WAIT_FOREVER);
			cmd_setPM();
#endif // POWER_SAVING
			break;
		}
		case CMD_DEINIT:{
			break;
		}
		}
	}
}

/*********************************************************************
 * @fn      cmd_taskFxn
 *
 * @brief   Application task entry point for the cmd.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void cmd_taskFxn(UArg a0, UArg a1)
{
	ICall_registerApp(&selfEntity, &sem);
	for(;;){
		Semaphore_pend(Semaphore_handle(&cmdCmdSem), UART_WAIT_FOREVER);
		if(cmdRxBuf->count > 0){
			char *cmd = (char *)ICall_malloc(cmdRxBuf->count + 1);
			if(NULL != cmd){
				memset(cmd, 0, cmdRxBuf->count + 1);
				Ringbuff_read(cmdRxBuf, (uint8_t *)cmd, cmdRxBuf->count);
				uint16_t size = 0;
				uint16_t sizesize = 0;
				char *pos = cmd;
				char *pospos = 0;

				while(true){
					pospos = strstr(pos, "\r\n");
					if(NULL != pospos){
						size = pospos - pos + 2;
						sizesize += size;
						char *cmdcmd = (char *)ICall_malloc(size + 1);
						if(NULL != cmdcmd){
							memset(cmdcmd, 0, size + 1);
							memcpy(cmdcmd, pos, size);
							//handle command
							cmd_hdlFxn(cmdcmd);
							ICall_free(cmdcmd);
						}
						pos = pospos + 2;
					}
					else{
						break;
					}
				}
				Ringbuff_delete(cmdRxBuf, sizesize);
				ICall_free(cmd);
			}
		}
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
#if (GL_LOG)
void Cmd_onValueChanged(uint8_t param)
{
	bool res = false;
	switch(param){
	case DOORLOCK_PIN:{
		char *pin = ICall_malloc(PIN_LEN + 1);
		if(pin){
			memset(pin, 0, PIN_LEN + 1);
			res = (SUCCESS == CustNV_getPin(pin));
			if(res){
				Log_printf(STR_RES_PIN, pin);
			}
			ICall_free(pin);
		}
		break;
	}
	case DOORLOCK_PARA:{
//	case DOORLOCK_DM:{
		uint8_t dm = 0;
		res = (SUCCESS == CustNV_getDatamode(&dm));
		if(res){
			Log_printf(STR_RES_DATAMODE, dm);
		}
//		break;
//	}
//	case DOORLOCK_TZ:{
		int8_t tz = 0;
		res = (SUCCESS == CustNV_getTimeZone(&tz));
		if(res){
			Log_printf(STR_RES_TIMEZONE, tz);
//		}
//		break;
	}
//	case DOORLOCK_MM:{
		uint8_t mm = 0;
		res = (SUCCESS == CustNV_getMotorMode(&mm));
		if(res){
			Log_printf(STR_RES_MOTORMODE, mm);
		}
//		break;
//	}
//	case DOORLOCK_MDT:{
		uint8_t mdt = 0;
		res = (SUCCESS == CustNV_getMotorTime(&mdt));
		if(res){
			Log_printf(STR_RES_MOTORTIME, mdt);
		}
//		break;
//	}
//	case DOORLOCK_OM:{
		uint8_t om = 0;
		res = (SUCCESS == CustNV_getOfficeMode(&om));
		if(res){
			Log_printf(STR_RES_OFFICEMODE, om);
		}
//		break;
//	}
//	case DOORLOCK_OMT:{
		uint8_t omt = 0;
		res = (SUCCESS == CustNV_getOfficeTime(&omt));
		if(res){
			Log_printf(STR_RES_OFFICETIME, omt);
		}
//		break;
//	}
//	case DOORLOCK_HBI:{
		uint8_t hbi = 0;
		res = (SUCCESS == CustNV_getHBInterval(&hbi));
		if(res){
			Log_printf(STR_RES_HBINTERVAL, hbi);
		}
		break;
	}
	default:{
		break;
	}
	}
}
#endif

/*********************************************************************
 * @fn      cmd_taskFxn
 *
 * @brief   Application task entry point for the cmd.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
void cmd_hdlFxn(char *cmd)
{
	uint8_t i = 0;
	bool handled = false;

	if(strstr(cmd, STR_REQ_AT) == cmd){//start with AT
		if(strlen(cmd) == strlen(STR_REQ_HANDSHAKE)){//received only "AT"
			cmd_write(STR_RES_OK, strlen(STR_RES_OK));
			handled = true;
		}
		else{
			for(i = 0; ;i++){
				if(cmdTable[i].cmd != NULL){
					if(strstr(cmd, cmdTable[i].cmd) != NULL){
						if(cmdTable[i].handler != NULL){
							cmdTable[i].handler(i, cmd);
							handled = true;
						}
						break;
					}
				}
				else{
					break;
				}
			}
		}
	}


	if(!handled){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
cmdType_e cmd_getType(uint8_t idx, char *cmd)
{
	char *p = strstr(cmd, cmdTable[idx].cmd);
	p += strlen(cmdTable[idx].cmd);

	if('?' == *p){
		return CMD_GET;
	}
	else if('=' == *p){
		if('?' == *(p + 1)){
			return CMD_FORMAT;
		}
		else{
			return CMD_SET;
		}
	}
	else if('\r' == *p){
		return CMD_SET;
	}
	else{
		return CMD_UNKNOWN;
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
void cmd_onRestore(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){

	}
	else if(CMD_FORMAT == ct){

	}
	else if(CMD_SET == ct){
		res = (SUCCESS == CustNV_restore());
		if(res){
			cmd_write(STR_RES_OK, strlen(STR_RES_OK));
		}
	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
void cmd_onPin(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){
		char *pin = ICall_malloc(PIN_LEN + 1);
		if(pin){
			memset(pin, 0, PIN_LEN + 1);
			res = (SUCCESS == CustNV_getPin(pin));
			if(res){
				Log_printf(STR_RES_PIN, pin);
			}
			ICall_free(pin);
		}
	}
	else if(CMD_FORMAT == ct){
		cmd_write(STR_FORMAT_PIN, strlen(STR_FORMAT_PIN));
		res = true;
	}
	else if(CMD_SET == ct){
		if(strlen(cmd) == STR_LEN_VALID_PIN){//11+PIN_LEN
			char *pin = ICall_malloc(PIN_LEN + 1);
			if(pin){
				memset(pin, 0, PIN_LEN + 1);
				if(sscanf(cmd, STR_SET_PIN, pin) > 0){
					res = (SUCCESS == CustNV_setPin(pin));
					if(res){
						cmd_write(STR_RES_OK, strlen(STR_RES_OK));
					}
				}
				ICall_free(pin);
			}

		}
	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
static void cmd_onDataMode(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){
		uint8_t dm = 0;
		res = (SUCCESS == CustNV_getDatamode(&dm));
		if(res){
			Log_printf(STR_RES_DATAMODE, dm);
		}
	}
	else if(CMD_FORMAT == ct){
		cmd_write(STR_FORMAT_DATAMODE, strlen(STR_FORMAT_DATAMODE));
		res = true;
	}
	else if(CMD_SET == ct){
		uint32_t dm = 0;
		if(sscanf(cmd, STR_SET_DATAMODE, &dm) > 0){
			res = (SUCCESS == CustNV_setDatamode((uint8_t)dm));
			if(res){
				cmd_write(STR_RES_OK, strlen(STR_RES_OK));
			}
		}
	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
void cmd_onTimeZone(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){
		int8_t tz = 0;
		res = (SUCCESS == CustNV_getTimeZone(&tz));
		if(res){
			Log_printf(STR_RES_TIMEZONE, tz);
		}
	}
	else if(CMD_FORMAT == ct){
		cmd_write(STR_FORMAT_TIMEZONE, strlen(STR_FORMAT_TIMEZONE));
		res = true;
	}
	else if(CMD_SET == ct){
		int32_t tz = 0;
		if(sscanf(cmd, STR_SET_TIMEZONE, &tz) > 0){
			res = (SUCCESS == CustNV_setTimeZone((int8_t)tz));
			if(res){
				cmd_write(STR_RES_OK, strlen(STR_RES_OK));
			}
		}
	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
void cmd_onMotorMode(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){
		uint8_t mode = 0;
		res = (SUCCESS == CustNV_getMotorMode(&mode));
		if(res){
			Log_printf(STR_RES_MOTORMODE, mode);
		}
	}
	else if(CMD_FORMAT == ct){
		cmd_write(STR_FORMAT_MOTORMODE, strlen(STR_FORMAT_MOTORMODE));
		res = true;
	}
	else if(CMD_SET == ct){
		uint32_t mode = 0;
		if(sscanf(cmd, STR_SET_MOTORMODE, &mode) > 0){
			res = (SUCCESS == CustNV_setMotorMode((uint8_t)mode));
			if(res){
				cmd_write(STR_RES_OK, strlen(STR_RES_OK));
			}
		}
	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
void cmd_onMotorTime(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){
		uint8_t time = 0;
		res = (SUCCESS == CustNV_getMotorTime(&time));
		if(res){
			Log_printf(STR_RES_MOTORTIME, time);
		}
	}
	else if(CMD_FORMAT == ct){
		cmd_write(STR_FORMAT_MOTORTIME, strlen(STR_FORMAT_MOTORTIME));
		res = true;
	}
	else if(CMD_SET == ct){
		uint32_t time = 0;
		if(sscanf(cmd, STR_SET_MOTORTIME, &time) > 0){
			res = (SUCCESS == CustNV_setMotorTime((uint8_t)time));
			if(res){
				cmd_write(STR_RES_OK, strlen(STR_RES_OK));
			}
		}
	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
void cmd_onOfficeMode(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){
		uint8_t mode = 0;
		res = (SUCCESS == CustNV_getOfficeMode(&mode));
		if(res){
			Log_printf(STR_RES_OFFICEMODE, mode);
		}
	}
	else if(CMD_FORMAT == ct){
		cmd_write(STR_FORMAT_OFFICEMODE, strlen(STR_FORMAT_OFFICEMODE));
		res = true;
	}
	else if(CMD_SET == ct){
		uint32_t mode = 0;
		if(sscanf(cmd, STR_SET_OFFICEMODE, &mode) > 0){
			res = (SUCCESS == CustNV_setOfficeMode((uint8_t)mode));
			if(res){
				cmd_write(STR_RES_OK, strlen(STR_RES_OK));
			}
		}
	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
void cmd_onOfficeTime(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){
		uint8_t time = 0;
		res = (SUCCESS == CustNV_getOfficeTime(&time));
		if(res){
			Log_printf(STR_RES_OFFICETIME, time);
		}
	}
	else if(CMD_FORMAT == ct){
		cmd_write(STR_FORMAT_OFFICETIME, strlen(STR_FORMAT_OFFICETIME));
		res = true;
	}
	else if(CMD_SET == ct){
		uint32_t time = 0;
		if(sscanf(cmd, STR_SET_OFFICETIME, &time) > 0){
			res = (SUCCESS == CustNV_setOfficeTime((uint8_t)time));
			if(res){
				cmd_write(STR_RES_OK, strlen(STR_RES_OK));
			}
		}
	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
void cmd_onHBInterval(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){
		uint8_t time = 0;
		res = (SUCCESS == CustNV_getHBInterval(&time));
		if(res){
			Log_printf(STR_RES_HBINTERVAL, time);
		}
	}
	else if(CMD_FORMAT == ct){
		cmd_write(STR_FORMAT_HBINTERVAL, strlen(STR_FORMAT_HBINTERVAL));
		res = true;
	}
	else if(CMD_SET == ct){
		uint32_t time = 0;
		if(sscanf(cmd, STR_SET_HBINTERVAL, &time) > 0){
			res = (SUCCESS == CustNV_setHBInterval((uint8_t)time));
			if(res){
				cmd_write(STR_RES_OK, strlen(STR_RES_OK));
			}
		}
	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
void cmd_onVersion(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){
		Log_printf(STR_RES_VERSION, MAJOR_VER, MINOR_VER, BUILD_VER);
		res = true;
	}
	else if(CMD_FORMAT == ct){

	}
	else if(CMD_SET == ct){

	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
void cmd_onModelNumber(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){
		Log_printf(STR_RES_MODELNO, MODEL_NO);
		res = true;
	}
	else if(CMD_FORMAT == ct){

	}
	else if(CMD_SET == ct){

	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 */
void cmd_onModelDesc(uint8_t idx, char *cmd)
{
	cmdType_e ct = cmd_getType(idx, cmd);
	bool res = false;

	if(CMD_GET == ct){
		Log_printf(STR_RES_MODELDESC, MODEL_DESC);
		res = true;
	}
	else if(CMD_FORMAT == ct){

	}
	else if(CMD_SET == ct){

	}
	else{
	}

	if(!res){
		cmd_write(STR_RES_ERROR, strlen(STR_RES_ERROR));
	}
}

// -----------------------------------------------------------------------------
//! \brief      This is a HWI function handler for the MRDY pin. Some MRDY
//!             functionality can execute from this HWI context.
//!
//! \param[in]  hPin - PIN Handle
//! \param[in]  pinId - ID of pin that triggered HWI
//!
//! \return     void
// -----------------------------------------------------------------------------
#ifdef POWER_SAVING
void cmd_remRdyPINHwiFxn(PIN_Handle hPin, PIN_Id pinId)
{
	if (remRdy_state != PIN_getInputValue(remRdyPIN)){
		remRdy_state = PIN_getInputValue(remRdyPIN);
		if(CMD_SUSPEND == cmdState){
			cmdState = CMD_PRERESUME;
			Semaphore_post(Semaphore_handle(&cmdSBSem));
		}
		else if(CMD_RUN == cmdState){
			cmdState = CMD_PRESUSPEND;
			Semaphore_post(Semaphore_handle(&cmdRxSem));
		}
	}
}
#endif //POWER_SAVING

#ifdef POWER_SAVING
// -----------------------------------------------------------------------------
//! \brief      This routine is used to set constraints on power manager
//!
//! \return     void
// -----------------------------------------------------------------------------
void cmd_setPM(void)
{
    if (cmdPMSetConstraint)
    {
        return;
    }

    // set constraints for Standby and idle mode
    Power_setConstraint(Power_SB_DISALLOW);
    Power_setConstraint(Power_IDLE_PD_DISALLOW);
    cmdPMSetConstraint = true;
}
#endif //POWER_SAVING

#ifdef POWER_SAVING
// -----------------------------------------------------------------------------
//! \brief      This routine is used to release constraints on power manager
//!
//! \return     void
// -----------------------------------------------------------------------------
void cmd_relPM(void)
{
    if (!cmdPMSetConstraint)
    {
        return;
    }

    // release constraints for Standby and idle mode
    Power_releaseConstraint(Power_SB_DISALLOW);
    Power_releaseConstraint(Power_IDLE_PD_DISALLOW);
    cmdPMSetConstraint = false;
}
#endif //POWER_SAVING


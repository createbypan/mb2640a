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
#include <stdbool.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/uart/UARTCC26XX.h>
#include "board.h"
#include "ringbuff.h"
#include "cmd.h"

/*********************************************************************
 * CONSTANTS
 */
#define LOG FALSE
// Task stack size
#ifndef CMD_TASK_STACK_SIZE
#define CMD_TASK_STACK_SIZE 128
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
    char *cmd
);

typedef struct{
    char *cmd;
    CMD_PROCESS_HANDLER handler;
} cmdTable_t;

/*********************************************************************
 * LOCAL VARIABLES
 */
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
static char cmdTaskRxStack[CMD_TASK_STACK_SIZE];
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
static void cmd_onTest(char *cmd);

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
    {"+TEST", cmd_onTest},
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
int log_write(const void *buffer, size_t size, UInt32 timeout)
{
	if(Semaphore_pend(Semaphore_handle(&cmdTxSem), timeout)){
		return UART_write(uartHandle, buffer, size);
	}
	else{
		return UART_ERROR;
	}
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

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param   None.
 *
 * @return  None.
 */
void cmd_createTask(void)
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
	taskParams.stack = cmdTaskRxStack;
	taskParams.stackSize = CMD_TASK_STACK_SIZE;
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
			cmdRxBuf = RingBuffInit(CMD_BUFF_SIZE);
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
				WriteRingBuff(cmdRxBuf, rxBuf, cmdRxSize);
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
	for(;;){
		Semaphore_pend(Semaphore_handle(&cmdCmdSem), UART_WAIT_FOREVER);
		if(cmdRxBuf->count > 0){
			char *cmd = (char *)ICall_malloc(cmdRxBuf->count + 1);
			if(NULL != cmd){
				memset(cmd, 0, cmdRxBuf->count + 1);
				ReadRingBuff(cmdRxBuf, (uint8_t *)cmd, cmdRxBuf->count);
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
				DeleteRingBuff(cmdRxBuf, sizesize);
				ICall_free(cmd);
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
void cmd_hdlFxn(char *cmd)
{
	uint8_t i = 0;
	for(i = 0; ;i++){
		if(cmdTable[i].cmd != NULL){
			if(strstr(cmd, cmdTable[i].cmd) != NULL){
				if(cmdTable[i].handler != NULL){
					cmdTable[i].handler(cmd);
				}
				break;
			}
		}
		else{
			break;
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
void cmd_onTest(char *cmd)
{
	cmd_write(cmd, strlen(cmd));
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


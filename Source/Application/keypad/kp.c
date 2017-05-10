/*******************************************************************************
  Filename:
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:
*******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#ifdef USE_ICALL
#include <ICall.h>
#else
#include <stdlib.h>
#endif

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
#include "stdbool.h"
#include "string.h"
#include "board.h"
#include "cmd.h"
#include "gpio_in.h"
#include "kp.h"

/*********************************************************************
 * CONSTANTS
 */
#define LOG TRUE

#if (GL_LOG && LOG)
#define KP_WRITESTRING(S) Log_writeStr(S)
#define KP_PRINTF(F, V) Log_printf(F, V)
#else
#define KP_WRITESTRING(S)
#define KP_PRINTF(F, V)
#endif

#define I2C_WAIT_FOREVER (~0)
#define I2C_WAIT_KEY 5000 //5000 = 50ms

// Task stack size
#ifndef KP_TASK_STACK_SIZE
#define KP_TASK_STACK_SIZE 400
#endif

// Task configuration
#define KP_TASK_PRIORITY 1

#define KP_MAX_INPUT 20

#define KEY_1    0x0001
#define KEY_2    0x0002
#define KEY_3    0x0004
#define KEY_4    0x0008
#define KEY_5    0x0010
#define KEY_6    0x0020
#define KEY_7    0x0040
#define KEY_8    0x0080
#define KEY_9    0x0100
#define KEY_STAR 0x0200
#define KEY_0    0x0400
#define KEY_HASH 0x0800

#define st(x) do { x } while (__LINE__ == -1)
//clear code buffer
#define KP_CLEAR_INPUTBUFF() st( \
                memset(inputBuf, 0, KP_MAX_INPUT); \
                inputIdx = 0; \
                inputState = KP_CODE_STATE_IDLE; \
            )

/*********************************************************************
 * TYPEDEFS
 */
typedef enum
{
	KP_INIT = 0,
	KP_PRERESUME,
	KP_RESUME,
	KP_RUN,
	KP_PRESUSPEND,
	KP_SUSPEND,
	KP_DEINIT
} KPState_e;

typedef enum{
    KP_KEY_TYPE_NUM,
    KP_KEY_TYPE_STAR,
    KP_KEY_TYPE_HASH,
    KP_KEY_TYPE_UNKNOWN,
} KPKeyType_e;

typedef enum{
    KP_CODE_TYPE_CUS,//customer password//000000#
    KP_CODE_TYPE_MNM,//management password//*000000#
    KP_CODE_TYPE_CMD,//command code//*#000000#
    KP_CODE_TYPE_ERROR,//for upper task to indicate input error
} KPCodeType_e;

typedef enum{
    KP_CODE_STATE_IDLE,
    KP_CODE_STATE_CUS_START,
    KP_CODE_STATE_MNM_CMD_START,
    KP_CODE_STATE_MNM_START,
    KP_CODE_STATE_MNM_WAIT_STAR,
    KP_CODE_STATE_MNM_WAIT_HASH,
    KP_CODE_STATE_CMD_START,
    KP_CODE_STATE_CMD_WAIT_HASH,
    KP_CODE_STATE_CUS_DONE,
    KP_CODE_STATE_MNM_DONE,
    KP_CODE_STATE_CMD_DONE,
    KP_CODE_STATE_ERROR,
} KPCodeState_e;

/*********************************************************************
 * LOCAL VARIABLES
 */
// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Semaphore globally used to post events to the application thread
static ICall_Semaphore sem;

static I2C_Handle i2cHandle;
static Semaphore_Struct transSem = {0};
// Task configuration
static Task_Struct kpTask;
static char kpTaskStack[KP_TASK_STACK_SIZE];

static KPState_e kpState = KP_INIT;
static volatile bool kpPMSetConstraint = false;
static volatile bool kpActive = false;

static I2C_Transaction i2cTrans = {0};
static uint8_t txBuf[1] = {0};
static uint8_t rxBuf[2] = {0};
static char inputBuf[KP_MAX_INPUT + 1] = {0};
static uint8_t inputIdx = 0;
static KPCodeState_e inputState = KP_CODE_STATE_IDLE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void kp_openI2c(void);
static void kp_closeI2c(void);
static void kp_taskRxFxn(UArg a0, UArg a1);
static void kp_intHandler(uint8_t keys);
static char kp_KeyMap(uint16_t value);
static bool kp_PushKey(char key);
static KPCodeState_e kp_CodeProcess(KPCodeState_e state, uint8_t key);

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
 * @return
 */
void kp_openI2c(void)
{
	I2C_Params params;

	I2C_Params_init(&params); //I2C_MODE_BLOCKING
	i2cHandle = I2C_open(Board_I2C, &params);
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
void kp_closeI2c(void)
{
	I2C_close(i2cHandle);
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
void Kp_createTask(void)
{
	GpioIn_registerHandler(GPIOIN_KPINT, kp_intHandler);

	Semaphore_Params semParams;
	Semaphore_Params_init(&semParams);
	semParams.mode = Semaphore_Mode_BINARY;
	Semaphore_construct(&transSem, 1, &semParams);
	Semaphore_pend(Semaphore_handle(&transSem), I2C_WAIT_FOREVER);

	Task_Params taskParams;
	// Configure task
	Task_Params_init(&taskParams);
	taskParams.stack = kpTaskStack;
	taskParams.stackSize = KP_TASK_STACK_SIZE;
	taskParams.priority = KP_TASK_PRIORITY;
	Task_construct(&kpTask, kp_taskRxFxn, &taskParams, NULL);
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
void kp_taskRxFxn(UArg a0, UArg a1)
{
	for(;;){
		switch(kpState){
		case KP_INIT:{
			kpState = KP_RESUME;

			break;
		}
		case KP_PRERESUME:{
			kpState = KP_RESUME;

			break;
		}
		case KP_RESUME:{
			if(!kpActive){
				kp_openI2c();
				kpActive = true;
			}
			KP_WRITESTRING("KP_RESUME\r\n");
			kpState = KP_RUN;

			break;
		}
		case KP_RUN:{
			if(Semaphore_pend(Semaphore_handle(&transSem), I2C_WAIT_FOREVER)){
				txBuf[0] = 0x01;
				i2cTrans.writeCount = 1;
				i2cTrans.writeBuf = txBuf;
				i2cTrans.readCount = 2;
				i2cTrans.readBuf = rxBuf;
				i2cTrans.slaveAddress = 0x28;

				if(I2C_transfer(i2cHandle, &i2cTrans)){
					char key = kp_KeyMap(*((uint16_t *)rxBuf));
					if('\0' != key){
						if(kp_PushKey(key)){
							inputState = kp_CodeProcess(inputState, key);
							if(KP_CODE_STATE_CUS_DONE == inputState){
								KP_WRITESTRING(inputBuf);
								KP_WRITESTRING("\r\nKP_CODE_STATE_CUS_DONE\r\n");
								KP_CLEAR_INPUTBUFF();
							}
							else if(KP_CODE_STATE_MNM_DONE == inputState){
								KP_WRITESTRING(inputBuf);
								KP_WRITESTRING("\r\nKP_CODE_STATE_MNM_DONE\r\n");
								KP_CLEAR_INPUTBUFF();
							}
							else if(KP_CODE_STATE_CMD_DONE == inputState){
								KP_WRITESTRING(inputBuf);
								KP_WRITESTRING("\r\nKP_CODE_STATE_CMD_DONE\r\n");
								KP_CLEAR_INPUTBUFF();
							}
							else if(KP_CODE_STATE_ERROR == inputState){
								KP_WRITESTRING(inputBuf);
								KP_WRITESTRING("\r\nKP_CODE_STATE_ERROR\r\n");
								KP_CLEAR_INPUTBUFF();
							}
							else{

							}
						}
						else{
							KP_WRITESTRING(inputBuf);
							KP_WRITESTRING("\r\nKP_CODE_STATE_ERROR\r\n");
							KP_CLEAR_INPUTBUFF();
						}
					}
				}
			}

			break;
		}
		case KP_PRESUSPEND:{
			kpState = KP_SUSPEND;

			break;
		}
		case KP_SUSPEND:{
			if(kpActive){
				KP_WRITESTRING("KP_SUSPEND\r\n");
				kp_closeI2c();
				kpActive = false;
			}
			break;
		}
		case KP_DEINIT:{
			break;
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
char kp_KeyMap(uint16_t value)
{
	char key = '\0';
	if(KEY_1 == value){
		key = '1';
	}
	else if(KEY_2 == value){
		key = '2';
	}
	else if(KEY_3 == value){
		key = '3';
	}
	else if(KEY_4 == value){
		key = '4';
	}
	else if(KEY_5 == value){
		key = '5';
	}
	else if(KEY_6 == value){
		key = '6';
	}
	else if(KEY_7 == value){
		key = '7';
	}
	else if(KEY_9 == value){
		key = '9';
	}
	else if(KEY_0 == value){
		key = '0';
	}
	else if(KEY_STAR == value){
		key = '*';
	}
	else if(KEY_HASH == value){
		key = '#';
	}

	if('\0' != key){
		KP_PRINTF("+KEY=%c\r\n", key);
	}

	return key;
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
bool kp_PushKey(char key)
{
	if(inputIdx < KP_MAX_INPUT){
		inputBuf[inputIdx++] = key;
		////////////////////////////////////////////////////////////////////
		//when the buffer is full and the last key is not '#', return false
		if(KP_MAX_INPUT == inputIdx){
			if('#' != key){
				return false;
			}
		}
		////////////////////////////////////////////////////////////////////

		return true;
	}

	return false;
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
KPCodeState_e kp_CodeProcess(KPCodeState_e state, uint8_t key)
{
	KPKeyType_e type = KP_KEY_TYPE_UNKNOWN;

    if(key >= '0' && key <= '9'){
        type = KP_KEY_TYPE_NUM;
    }
    else if(key == '*'){
        type = KP_KEY_TYPE_STAR;
    }
    else if(key == '#'){
        type = KP_KEY_TYPE_HASH;
    }
    else{
        return state;
    }

    switch(state){
	case KP_CODE_STATE_IDLE:{
		if(KP_KEY_TYPE_NUM == type){
			state = KP_CODE_STATE_CUS_START;
		}
		else if(KP_KEY_TYPE_STAR == type){
			state = KP_CODE_STATE_MNM_CMD_START;
		}
		else{
			state = KP_CODE_STATE_ERROR;
		}

		break;
	}
	case KP_CODE_STATE_CUS_START:{
		if(KP_KEY_TYPE_HASH == type){
			state = KP_CODE_STATE_CUS_DONE;//customer code done
		}
		else if(KP_KEY_TYPE_NUM == type){
			//do nothing
		}
		else{
			state = KP_CODE_STATE_ERROR;
		}

		break;
	}
	case KP_CODE_STATE_MNM_CMD_START:{
		if(KP_KEY_TYPE_NUM == type){
			state = KP_CODE_STATE_MNM_START;
		}
		else if(KP_KEY_TYPE_HASH == type){
			state = KP_CODE_STATE_CMD_START;
		}
		else{
			state = KP_CODE_STATE_ERROR;
		}

		break;
	}
	case KP_CODE_STATE_MNM_START:{
		if(KP_KEY_TYPE_HASH == type){
			state = KP_CODE_STATE_MNM_DONE;//management code done
		}
		else if(KP_KEY_TYPE_NUM == type){
			//do nothing
		}
		else{
			state = KP_CODE_STATE_ERROR;
		}

		break;
	}
	case KP_CODE_STATE_CMD_START:{
		if(KP_KEY_TYPE_HASH == type){
			state = KP_CODE_STATE_CMD_DONE;//cmd code done
		}
		else if(KP_KEY_TYPE_NUM == type){
			//do nothing
		}
		else{
			state = KP_CODE_STATE_ERROR;
		}

		break;
	}
    }

    return state;
}

/*********************************************************************
 * @fn      kp_intHandler
 *
 * @brief   Key event handler function
 *
 * @param   keys - keys pressed.
 *
 * @return  none
 */
void kp_intHandler(uint8_t keys)
{
	Semaphore_post(Semaphore_handle(&transSem));
}

/*********************************************************************
*********************************************************************/

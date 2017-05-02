/*******************************************************************************
  Filename:       board_key.c
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:
*******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <stdbool.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

#include <ti/drivers/pin/PINCC26XX.h>

#ifdef USE_ICALL
#include <ICall.h>
#endif
   
#include <inc/hw_ints.h>
#include "bcomdef.h"

#include "util.h"
#include "board_key.h"
#include "Board.h"

/*********************************************************************
 * TYPEDEFS
 */
typedef struct{
	keysPressedCB_t appKeyMRDYHandler;
	keysPressedCB_t appKeyKPIntHandler;
} appKeyChangeHadlers_t;

typedef struct{
	uint_t prev;
	uint_t curr;
} keyStatus_t;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void Board_keyCallback(PIN_Handle hPin, PIN_Id pinId);
static void Board_keyMRDYChangeHandler(UArg a0);
static void Board_keyKPIntChangeHandler(UArg a0);

/*******************************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Value of keys Event
static uint8_t keysEvent;

keyStatus_t keyMRDYStatus;
keyStatus_t keyKPIntStatus;

// Key debounce clock
static Clock_Struct keyMRDYChangeClock;
static Clock_Struct keyKPIntChangeClock;

// Pointer to application callback
static appKeyChangeHadlers_t appKeyChangeHandlers = {NULL};

// PIN configuration structure to set all KEY pins as inputs with pullups enabled
static PIN_Config keyPinsCfg[] =
{
	Board_MRDY_PIN    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  |  PIN_PULLUP,
	Board_KP_INT      | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  |  PIN_PULLUP,
    PIN_TERMINATE
};

static PIN_State  keyPins;
static PIN_Handle hKeyPins;

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/*********************************************************************
 * @fn      Board_initKeys
 *
 * @brief   Enable interrupts for keys on GPIOs.
 *
 * @param   none
 *
 * @return  none
 */
void Board_initKeys()
{
  // Initialize KEY pins. Enable int after callback registered
  hKeyPins = PIN_open(&keyPins, keyPinsCfg);
  PIN_registerIntCb(hKeyPins, Board_keyCallback);
  
  PIN_setConfig(hKeyPins, PIN_BM_IRQ, Board_MRDY_PIN  | PIN_IRQ_BOTHEDGES);
  PIN_setConfig(hKeyPins, PIN_BM_IRQ, Board_KP_INT    | PIN_IRQ_NEGEDGE);

#ifdef POWER_SAVING
  //Enable wakeup
  PIN_setConfig(hKeyPins, PINCC26XX_BM_WAKEUP, Board_MRDY_PIN | PINCC26XX_WAKEUP_NEGEDGE);
  PIN_setConfig(hKeyPins, PINCC26XX_BM_WAKEUP, Board_KP_INT | PINCC26XX_WAKEUP_POSEDGE);
#endif
  
  keyMRDYStatus.curr = PIN_getInputValue(Board_MRDY_PIN);
  keyMRDYStatus.prev = keyMRDYStatus.curr;

  keyKPIntStatus.curr = PIN_getInputValue(Board_KP_INT);
  keyKPIntStatus.prev = keyKPIntStatus.curr;

  // Setup keycallback for keys
  Util_constructClock(&keyMRDYChangeClock, Board_keyMRDYChangeHandler,
		  KEY_MRDY_DEBOUNCE_TIMEOUT, 0, false, 0);

  Util_constructClock(&keyKPIntChangeClock, Board_keyKPIntChangeHandler,
		  KEY_KPINT_DEBOUNCE_TIMEOUT, 0, false, 0);
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return  none
 */
void Board_registerMRDYHandler(keysPressedCB_t handler)
{
	appKeyChangeHandlers.appKeyMRDYHandler = handler;
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return  none
 */
void Board_registerKPIntHandler(keysPressedCB_t handler)
{
	appKeyChangeHandlers.appKeyKPIntHandler = handler;
}

/*********************************************************************
 * @fn      Board_keyCallback
 *
 * @brief   Interrupt handler for Keys
 *
 * @param   none
 *
 * @return  none
 */
static void Board_keyCallback(PIN_Handle hPin, PIN_Id pinId)
{
  keysEvent = 0;

  if(Board_MRDY_PIN == pinId){
	  if(keyMRDYStatus.curr != PIN_getInputValue(Board_MRDY_PIN)){
		  Util_startClock(&keyMRDYChangeClock);
	  }
  }
  else if(Board_KP_INT == pinId){
	  if ( PIN_getInputValue(Board_KP_INT) == 0 ){
		  Util_startClock(&keyKPIntChangeClock);
	  }
  }
  else{

  }
}

/*********************************************************************
 * @fn      Board_keyMRDYChangeHandler
 *
 * @brief   Handler for key change
 *
 * @param   UArg a0 - ignored
 *
 * @return  none
 */
static void Board_keyMRDYChangeHandler(UArg a0)
{
	uint_t kv = PIN_getInputValue(Board_MRDY_PIN);
	if(keyMRDYStatus.curr != kv){
		keyMRDYStatus.prev = keyMRDYStatus.curr;
		keyMRDYStatus.curr = kv;
		keysEvent = 0;
		if(0 == keyMRDYStatus.curr){
			keysEvent |= KEYEVT_MRDY_DOWN;
		}
		else{
			keysEvent |= KEYEVT_MRDY_UP;
		}

		if (appKeyChangeHandlers.appKeyMRDYHandler != NULL)
		{
			// Notify the application
			(*appKeyChangeHandlers.appKeyMRDYHandler)(keysEvent);
		}
	}
}

/*********************************************************************
 * @fn      Board_keyMRDYChangeHandler
 *
 * @brief   Handler for key change
 *
 * @param   UArg a0 - ignored
 *
 * @return  none
 */
static void Board_keyKPIntChangeHandler(UArg a0)
{
	if ( PIN_getInputValue(Board_KP_INT) == 0 ){
		keysEvent = 0;
		keysEvent |= KEYEVT_KP_INT;
		keyKPIntStatus.curr = 0;
		keyKPIntStatus.prev = 0;
		if (appKeyChangeHandlers.appKeyKPIntHandler != NULL)
		{
			// Notify the application
			(*appKeyChangeHandlers.appKeyKPIntHandler)(keysEvent);
		}
	}
}
/*********************************************************************
*********************************************************************/

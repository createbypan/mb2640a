/*******************************************************************************
  Filename:       gpio_in.c
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:
*******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <stdbool.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include "bcomdef.h"
#include "util.h"
#include "gpio_in.h"
#include "Board.h"

/*********************************************************************
 * CONSTANTS
 */
#define GPIOIN_DEBOUNCE_TIMEOUT_MRDY  100
#define GPIOIN_DEBOUNCE_TIMEOUT_KPINT  10

/*********************************************************************
 * TYPEDEFS
 */
typedef struct{
	gpioInCB_t gpioInMRDYHandler;
	gpioInCB_t gpioInKPIntHandler;
} gpioInChangeHadlers_t;

typedef struct{
	uint_t prev;
	uint_t curr;
} gpioInStatus_t;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void gpioin_IntCallback(PIN_Handle hPin, PIN_Id pinId);
static void gpioin_MRDYChangeHandler(UArg a0);
static void gpioin_KPIntChangeHandler(UArg a0);

/*******************************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static gpioInCB_t gpioInCBs[GPIOIN_CNT];
static gpioInStatus_t gpioInStatus[GPIOIN_CNT];
//Debounce clock
static Clock_Struct gpioInChangeClocks[GPIOIN_CNT];
// Value of keys Event
static uint8_t gpioInEvents;
// PIN configuration structure to set all KEY pins as inputs with pullups enabled
static PIN_Config gpioInsCfg[] =
{
	Board_MRDY_PIN    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  |  PIN_PULLUP,
	Board_KP_INT      | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  |  PIN_PULLUP,
    PIN_TERMINATE
};

static PIN_State  gpioIns;
static PIN_Handle hGpioIns;

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/*********************************************************************
 * @fn      GpioIn_initPins
 *
 * @brief   Enable interrupts for keys on GPIOs.
 *
 * @param   none
 *
 * @return  none
 */
void GpioIn_initPins()
{
  // Initialize KEY pins. Enable int after callback registered
  hGpioIns = PIN_open(&gpioIns, gpioInsCfg);
  PIN_registerIntCb(hGpioIns, gpioin_IntCallback);
  
  PIN_setConfig(hGpioIns, PIN_BM_IRQ, Board_MRDY_PIN  | PIN_IRQ_BOTHEDGES);
  PIN_setConfig(hGpioIns, PIN_BM_IRQ, Board_KP_INT    | PIN_IRQ_NEGEDGE);

#ifdef POWER_SAVING
  //Enable wakeup
  PIN_setConfig(hGpioIns, PINCC26XX_BM_WAKEUP, Board_MRDY_PIN | PINCC26XX_WAKEUP_NEGEDGE);
  PIN_setConfig(hGpioIns, PINCC26XX_BM_WAKEUP, Board_KP_INT | PINCC26XX_WAKEUP_POSEDGE);
#endif
  
  gpioInStatus[GPIOIN_MRDY].curr = PIN_getInputValue(Board_MRDY_PIN);
  gpioInStatus[GPIOIN_MRDY].prev = gpioInStatus[GPIOIN_MRDY].curr;

  gpioInStatus[GPIOIN_KPINT].curr = PIN_getInputValue(Board_KP_INT);
  gpioInStatus[GPIOIN_KPINT].prev = gpioInStatus[GPIOIN_KPINT].curr;

  // Setup keycallback for keys
  Util_constructClock(&gpioInChangeClocks[GPIOIN_MRDY], gpioin_MRDYChangeHandler,
		  GPIOIN_DEBOUNCE_TIMEOUT_MRDY, 0, false, 0);

  Util_constructClock(&gpioInChangeClocks[GPIOIN_KPINT], gpioin_KPIntChangeHandler,
		  GPIOIN_DEBOUNCE_TIMEOUT_KPINT, 0, false, 0);
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
bool GpioIn_registerHandler(uint8_t id, gpioInCB_t cb)
{
	if(id < GPIOIN_CNT){
		gpioInCBs[id] = cb;

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
static void gpioin_IntCallback(PIN_Handle hPin, PIN_Id pinId)
{
	if(Board_MRDY_PIN == pinId){
		if(gpioInStatus[GPIOIN_MRDY].curr != PIN_getInputValue(Board_MRDY_PIN)){
			Util_startClock(&gpioInChangeClocks[GPIOIN_MRDY]);
		}
	}
	else if(Board_KP_INT == pinId){
		if (PIN_getInputValue(Board_KP_INT) == 0){
			Util_startClock(&gpioInChangeClocks[GPIOIN_KPINT]);
		}
	}
	else{

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
static void gpioin_MRDYChangeHandler(UArg a0)
{
	uint_t v = PIN_getInputValue(Board_MRDY_PIN);
	if(gpioInStatus[GPIOIN_MRDY].curr != v){
		gpioInStatus[GPIOIN_MRDY].prev = gpioInStatus[GPIOIN_MRDY].curr;
		gpioInStatus[GPIOIN_MRDY].curr = v;
		gpioInEvents = 0;
		if(0 == gpioInStatus[GPIOIN_MRDY].curr){
			gpioInEvents |= GPIOIN_EVT_MRDY_DOWN;
		}
		else{
			gpioInEvents |= GPIOIN_EVT_MRDY_UP;
		}

		if (gpioInCBs[GPIOIN_MRDY] != NULL)
		{
			// Notify the application
			(*gpioInCBs[GPIOIN_MRDY])(gpioInEvents);
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
static void gpioin_KPIntChangeHandler(UArg a0)
{
	if (PIN_getInputValue(Board_KP_INT) == 0){
		gpioInEvents = 0;
		gpioInEvents |= GPIOIN_EVT_KPINT;
		gpioInStatus[GPIOIN_KPINT].curr = 0;
		gpioInStatus[GPIOIN_KPINT].prev = 0;
		if (gpioInCBs[GPIOIN_KPINT] != NULL){
			// Notify the application
			(*gpioInCBs[GPIOIN_KPINT])(gpioInEvents);
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
uint_t GpioIn_getStatus(uint8_t id)
{
	if(id < GPIOIN_CNT){
		return gpioInStatus[id].curr;
	}
	else{
		return 0;
	}
}

/*********************************************************************
*********************************************************************/

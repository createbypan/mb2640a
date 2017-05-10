/*******************************************************************************
  Filename:       gpio_out.c
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:
*******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <ti/sysbios/knl/Clock.h>
#include "board.h"
#include "util.h"
#include "gpio_out.h"

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    uint8_t mode;
    uint8_t left;
    uint8_t onPct;
    uint32_t time;
    uint32_t next;
} gpioOut_Ctrl_t;

typedef struct
{
	Clock_Struct handle;
	uint32_t duration;
	bool running;
} gpioOut_ClkCtrl_t;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
#define ACTIVE_LOW 0
#define ACTIVE_HIGH 1

#define GPIOOUT_SRDY_POLARITY ACTIVE_LOW
#define GPIOOUT_TEST_POLARITY ACTIVE_LOW

#if GPIOOUT_SRDY_POLARITY == ACTIVE_LOW
#define GPIOOUT_ON_SRDY() PIN_setOutputValue(hGpioOuts, Board_SRDY_PIN, 0)
#define GPIOOUT_OFF_SRDY() PIN_setOutputValue(hGpioOuts, Board_SRDY_PIN, 1)
#else
#define GPIOOUT_ON_SRDY() PIN_setOutputValue(hGpioOuts, Board_SRDY_PIN, 1)
#define GPIOOUT_OFF_SRDY() PIN_setOutputValue(hGpioOuts, Board_SRDY_PIN, 0)
#endif

#if GPIOOUT_TEST_POLARITY == ACTIVE_LOW
#define GPIOOUT_ON_TEST() PIN_setOutputValue(hGpioOuts, Board_TEST, 0)
#define GPIOOUT_OFF_TEST() PIN_setOutputValue(hGpioOuts, Board_TEST, 1)
#else
#define GPIOOUT_ON_TEST() PIN_setOutputValue(hGpioOuts, Board_TEST, 1)
#define GPIOOUT_OFF_TEST() PIN_setOutputValue(hGpioOuts, Board_TEST, 0)
#endif

/*******************************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
// PIN configuration structure to set all pins as outputs with pullups enabled
static PIN_Config gpioOutsCfg[] =
{
	Board_SRDY_PIN	| PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW   | PIN_PUSHPULL,
	Board_TEST	| PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW   | PIN_PUSHPULL,
    PIN_TERMINATE
};

static PIN_State  gpioOuts;
static PIN_Handle hGpioOuts;

static gpioOut_Ctrl_t gpioOutCtrlTable[GPIOOUT_CNT];
static gpioOut_ClkCtrl_t gpioOutBlinkClock;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void gpioOut_onOff(uint8_t ids, uint8_t mode);
static void gpioOut_BlinkHandler(UArg a0);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/*********************************************************************
 * @fn      GpioOut_initPins
 *
 * @brief
 *
 * @param   none
 *
 * @return  none
 */
void GpioOut_initPins()
{
	hGpioOuts = PIN_open(&gpioOuts, gpioOutsCfg);
	Util_constructClock(&gpioOutBlinkClock.handle/*handle*/, gpioOut_BlinkHandler/*callback*/,
			0/*duration*/, 0/*period*/, false/*start*/, 0/*arg*/);
	gpioOutBlinkClock.running = false;
	gpioOutBlinkClock.duration = 0;
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param   none
 *
 * @return  none
 */
void gpioOut_onOff(uint8_t ids, uint8_t mode)
{
    if(ids & GPIOOUT_SRDY){
        if(mode == GPIOOUT_ON){
            GPIOOUT_ON_SRDY();
        }
        else{
        	GPIOOUT_OFF_SRDY();
        }
    }

    if(ids & GPIOOUT_TEST){
		if(mode == GPIOOUT_ON){
			GPIOOUT_ON_TEST();
		}
		else{
			GPIOOUT_OFF_TEST();
		}
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param   none
 *
 * @return  none
 */
void GpioOut_set(uint8_t ids, uint8_t mode)
{
    uint8_t id = 0;
    gpioOut_Ctrl_t *sts = NULL;

    switch(mode)
    {
        case GPIOOUT_ON:
        case GPIOOUT_OFF:
        case GPIOOUT_TOGGLE:
        {
            id = GPIOOUT_START;
            ids &= GPIOOUT_ALL;
            sts = gpioOutCtrlTable;

            while(ids){
                if(ids & id){
                    if(mode != GPIOOUT_TOGGLE){
                        sts->mode = mode;
                    }
                    else{
                        sts->mode ^= GPIOOUT_ON;
                    }
                    gpioOut_onOff(id, sts->mode);
                    ids ^= id;
                }
                id <<= 1;
                sts++;
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param   none
 *
 * @return  none
 */
void GpioOut_blink(uint8_t ids, uint8_t numBlinks, uint8_t percent, uint32_t period)
{
	uint8_t id = 0;
	gpioOut_Ctrl_t *sts = NULL;

	if(ids && percent && period){
		if(percent < 100){
			id = GPIOOUT_START;
			ids &= GPIOOUT_ALL;
			sts = gpioOutCtrlTable;

			while(ids){
				if(ids & id){
					sts->mode = GPIOOUT_OFF;
					sts->time = period;
					sts->onPct = percent;
					sts->left = numBlinks;
					if(!numBlinks) sts->mode |= GPIOOUT_FLASH;
					sts->next = (Clock_getTicks() * Clock_tickPeriod) / 1000;//sts->next in milliseconds
					sts->mode |= GPIOOUT_BLINK;
					ids ^= id;
				}
				id <<= 1;
				sts++;
			}

			Util_stopClock(&gpioOutBlinkClock.handle);
			Util_restartClock(&gpioOutBlinkClock.handle, 0);
		}
		else{
			GpioOut_set(ids, GPIOOUT_ON);
		}
	}
	else{
		GpioOut_set(ids, GPIOOUT_OFF);
	}
}

/*********************************************************************
 * @fn
 *
 * @brief
 *
 * @param   none
 *
 * @return  none
 */
void gpioOut_BlinkHandler(UArg a0)
{
	uint8_t id = GPIOOUT_START;
	uint8_t ids = GPIOOUT_ALL;
	gpioOut_Ctrl_t *sts = gpioOutCtrlTable;
	uint32_t time = 0;
	uint32_t wait = 0;
	uint32_t next = 0;
	uint8_t pct = 0;

	while(ids){
		if(ids & id){
			if(sts->mode & GPIOOUT_BLINK){
				time = (Clock_getTicks() * Clock_tickPeriod) / 1000;//time in milliseconds
				if(time >= sts->next){
					if(sts->mode & GPIOOUT_ON){
						pct = 100 - sts->onPct;
						sts->mode &= ~GPIOOUT_ON;
						gpioOut_onOff(id, GPIOOUT_OFF);

						if(!(sts->mode & GPIOOUT_FLASH)){
							sts->left--;
						}
					}
					else if(!(sts->left) && !(sts->mode & GPIOOUT_FLASH)){
						sts->mode ^= GPIOOUT_BLINK;//end of blink action
					}
					else{
						pct = sts->onPct;
						sts->mode |= GPIOOUT_ON;
						gpioOut_onOff(id, GPIOOUT_ON);
					}

					if(sts->mode & GPIOOUT_BLINK){
						wait = ((pct * sts->time) / 100);
						sts->next = time + wait;
					}
					else{
						wait = 0;
					}
				}
				else{
					wait = sts->next - time;
				}

				if(!next || (wait && (wait < next))){
					next = wait;
				}
			}

			ids ^= id;
		}

		id <<= 1;
		sts++;
	}

	if(next > 0){
		Util_restartClock(&gpioOutBlinkClock.handle, next);
	}
}

/*********************************************************************
*********************************************************************/

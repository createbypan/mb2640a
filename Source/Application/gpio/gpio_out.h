/*******************************************************************************
  Filename:       gpio_out.h
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:
*******************************************************************************/

#ifndef GPIO_OUT_H
#define GPIO_OUT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */
#define GPIOOUT_START 0x01
#define GPIOOUT_SRDY 0x01
#define GPIOOUT_TEST 0x02

#define GPIOOUT_ALL (GPIOOUT_SRDY | GPIOOUT_TEST)
#define GPIOOUT_CNT 2

#define GPIOOUT_OFF 0
#define GPIOOUT_ON 1
#define GPIOOUT_BLINK 2
#define GPIOOUT_FLASH 4
#define GPIOOUT_TOGGLE 8

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * API FUNCTIONS
 */
void GpioOut_initPins();
void GpioOut_set(uint8_t ids, uint8_t mode);
void GpioOut_blink(uint8_t id, uint8_t numBlinks, uint8_t percent, uint32_t period);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GPIO_OUT_H */

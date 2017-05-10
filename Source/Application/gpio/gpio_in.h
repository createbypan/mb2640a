/*******************************************************************************
  Filename:       gpio_in.h
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:
*******************************************************************************/

#ifndef GPIO_IN_H
#define GPIO_IN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */
#include <ti/drivers/PIN.h>

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */
#define GPIOIN_MRDY 0
#define GPIOIN_KPINT 1
#define GPIOIN_CNT (GPIOIN_KPINT + 1)

#define GPIOIN_EVT_MRDY_DOWN         0x0001
#define GPIOIN_EVT_MRDY_UP           0x0002
#define GPIOIN_EVT_KPINT            0x0004

/*********************************************************************
 * TYPEDEFS
 */
typedef void (*gpioInCB_t)(uint8_t gpioIns);

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * API FUNCTIONS
 */
void GpioIn_initPins();
bool GpioIn_registerHandler(uint8_t id, gpioInCB_t cb);
uint_t GpioIn_getStatus(uint8_t id);

/*********************************************************************
*********************************************************************/  

#ifdef __cplusplus
}
#endif

#endif /* GPIO_IN_H */

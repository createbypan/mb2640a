/*******************************************************************************
  Filename:       board_key.h
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:
*******************************************************************************/

#ifndef BOARD_KEY_H
#define BOARD_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */
#include "stdint.h"

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */
#define KEYEVT_MRDY_DOWN         0x0001
#define KEYEVT_MRDY_UP           0x0002
#define KEYEVT_KP_INT            0x0004
   
// Debounce timeout in milliseconds
#define KEY_MRDY_DEBOUNCE_TIMEOUT  100
#define KEY_KPINT_DEBOUNCE_TIMEOUT  10

/*********************************************************************
 * TYPEDEFS
 */
typedef void (*keysPressedCB_t)(uint8_t keysPressed);

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      Board_initKeys
 *
 * @brief   Enable interrupts for keys on GPIOs.
 *
 * @param   appKeyCB - application key pressed callback
 *
 * @return  none
 */
void Board_initKeys();
void Board_registerMRDYHandler(keysPressedCB_t handler);
void Board_registerKPIntHandler(keysPressedCB_t handler);

/*********************************************************************
*********************************************************************/  

#ifdef __cplusplus
}
#endif

#endif /* BOARD_KEY_H */

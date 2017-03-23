/*******************************************************************************
  Filename:       MB2640A_util.h
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:
*******************************************************************************/

#ifndef MB2640A_UTIL_H
#define MB2640A_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */

#include <stdbool.h>

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      Util_convertBdAddr2Str
 *
 * @brief   Convert Bluetooth address to string. Only needed when
 *          LCD display is used.
 *
 * @param   pAddr - BD address
 *
 * @return  BD address as a string
 */
extern uint8_t Util_convertAddr2Str(uint8_t *pSrc, uint8_t src_len, uint8_t **pDst, uint8_t dst_len);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MB2640A_UTIL_H */

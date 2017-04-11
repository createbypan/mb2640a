/*******************************************************************************
  Filename:       MB2640A_util.c
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

#include "comdef.h"
#include <string.h>
#include "MB2640A_util.h"

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
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
uint8_t Util_convertAddr2Str(uint8_t *pSrc, uint8_t src_len, uint8_t **pDst, uint8_t dst_len)
{
  uint8_t i = 0;
  char hex[] = "0123456789ABCDEF";

  uint8_t *p = NULL;
  *pDst = NULL;

  if((src_len % 2) != 0){//src length is not even
	  return FAILURE;
  }

  if((src_len * 2) < dst_len){//src has not enough length
	  return FAILURE;
  }

  *pDst = ICall_malloc(dst_len);
  if(!(*pDst)){//no memory space
	  return FAILURE;
  }

  p = *pDst;
  memset(p, 0, dst_len);
  pSrc += (src_len / 2);

  for(i=0; i<(dst_len / 2); i++){
	  *p++ = hex[*--pSrc >> 4];
	  *p++ = hex[*pSrc & 0x0F];
  }

  return SUCCESS;
}

/*********************************************************************
 * @fn      Util_crc8
 *
 * @brief
 *
 * @param
 *
 * @return
 */
uint8_t Util_crc8(uint8_t *data, size_t length)
{
    uint8_t i;
    uint8_t crc = 0;    // Initial value
    while(length--)
    {
        crc ^= *data++; // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/*********************************************************************
*********************************************************************/

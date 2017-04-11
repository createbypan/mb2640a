/**************************************************************************************************
  Filename:       cmd.h
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:    This file contains the application definitions and prototypes.
**************************************************************************************************/

#ifndef CMD_H
#define CMD_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <ti/drivers/UART.h>
/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */
#define CMD_WRITE_WAIT 500

/*********************************************************************
 * FUNCTIONS
 */
/*
 *
 */
extern int Log_write(const void *buffer, size_t size, UInt32 timeout);

/*
 * Task creation function for the cmd.
 */
extern void Cmd_createTask(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* CMD_H */

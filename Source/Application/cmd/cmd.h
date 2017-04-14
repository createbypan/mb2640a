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

/*********************************************************************
 * FUNCTIONS
 */
/*
 *
 */
#if (GL_LOG)
extern int Log_writeStr(const char *buffer);
extern int Log_printf(const char *fmt, ...);
#endif

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

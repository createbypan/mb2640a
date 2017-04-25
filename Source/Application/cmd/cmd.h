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
#define MAJOR_VER 0
#define MINOR_VER 1
#define BUILD_VER 0

#define MODEL_NO "MB2640A"
#define MODEL_DESC "A BLE MODULE FOR DOOR LOCK"
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
#if (GL_LOG)
extern void Cmd_onValueChanged(uint8_t param);
#endif


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* CMD_H */

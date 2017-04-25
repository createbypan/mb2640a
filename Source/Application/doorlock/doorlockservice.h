/*******************************************************************************
  Filename:       doorlockservice.h
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:
*******************************************************************************/

#ifndef DOORLOCKSERVICE_H
#define DOORLOCKSERVICE_H

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
#define DOORLOCK_SERV_UUID 0xFFF0

#define DOORLOCK_SWVER_UUID 0xFFF0
#define DOORLOCK_PIN_UUID 0xFFF1
//#define DOORLOCK_DM_UUID 0xFFF2
//#define DOORLOCK_TZ_UUID 0xFFF3
//#define DOORLOCK_MM_UUID 0xFFF4
//#define DOORLOCK_MDT_UUID 0xFFF5
//#define DOORLOCK_OM_UUID 0xFFF6
//#define DOORLOCK_OMT_UUID 0xFFF7
//#define DOORLOCK_HBI_UUID 0xFFF8
#define DOORLOCK_PARA_UUID 0xFFF2

// Door Lock Service Parameters
#define DOORLOCK_SWVER 0
#define DOORLOCK_PIN 1
//#define DOORLOCK_DM 2
//#define DOORLOCK_TZ 3
//#define DOORLOCK_MM 4
//#define DOORLOCK_MDT 5
//#define DOORLOCK_OM 6
//#define DOORLOCK_OMT 7
//#define DOORLOCK_HBI 8
#define DOORLOCK_PARA 2

#define DOORLOCK_SWVER_LEN 3
#define DOORLOCK_PARA_CNT 7
#define DOORLOCK_PARA_IDX_DM 0
#define DOORLOCK_PARA_IDX_TZ 1
#define DOORLOCK_PARA_IDX_MM 2
#define DOORLOCK_PARA_IDX_MDT 3
#define DOORLOCK_PARA_IDX_OM 4
#define DOORLOCK_PARA_IDX_OMT 5
#define DOORLOCK_PARA_IDX_HBI 6

/*********************************************************************
 * TYPEDEFS
 */
/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*doorLockChange_t)( uint8 paramID );

typedef struct
{
	doorLockChange_t pfnDoorLockChange;  // Called when characteristic value changes
} doorLockCBs_t;

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * API FUNCTIONS
 */
bStatus_t DoorLock_AddService( void );
bStatus_t DoorLock_RegisterAppCBs( doorLockCBs_t *appCallbacks );
bStatus_t DoorLock_InitParameter();

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* DOORLOCKSERVICE_H */

/*******************************************************************************
  Filename:       doorlockservice.c
  Revised:        $Date:  $
  Revision:       $Revision:  $

  Description:
*******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "att.h"
#include "gatt.h"
#include "gattservapp.h"
#include "gatt_uuid.h"
#include "cust_nv.h"
#include "cmd.h"
#include "doorlockservice.h"

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Door lock service
CONST uint8 doorLockServUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(DOORLOCK_SERV_UUID), HI_UINT16(DOORLOCK_SERV_UUID)
};

// Software Version
CONST uint8 swVerUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(DOORLOCK_SWVER_UUID), HI_UINT16(DOORLOCK_SWVER_UUID)
};

// Pin
CONST uint8 pinUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(DOORLOCK_PIN_UUID), HI_UINT16(DOORLOCK_PIN_UUID)
};

//// Data Mode
//CONST uint8 dmUUID[ATT_BT_UUID_SIZE] =
//{
//  LO_UINT16(DOORLOCK_DM_UUID), HI_UINT16(DOORLOCK_DM_UUID)
//};
//
//// Time Zone
//CONST uint8 tzUUID[ATT_BT_UUID_SIZE] =
//{
//  LO_UINT16(DOORLOCK_TZ_UUID), HI_UINT16(DOORLOCK_TZ_UUID)
//};
//
//// Motor Mode
//CONST uint8 mmUUID[ATT_BT_UUID_SIZE] =
//{
//  LO_UINT16(DOORLOCK_MM_UUID), HI_UINT16(DOORLOCK_MM_UUID)
//};
//
//// Motor Drive Time
//CONST uint8 mdtUUID[ATT_BT_UUID_SIZE] =
//{
//  LO_UINT16(DOORLOCK_MDT_UUID), HI_UINT16(DOORLOCK_MDT_UUID)
//};
//
//// Office Mode
//CONST uint8 omUUID[ATT_BT_UUID_SIZE] =
//{
//  LO_UINT16(DOORLOCK_OM_UUID), HI_UINT16(DOORLOCK_OM_UUID)
//};
//
//// Office Mode Time
//CONST uint8 omtUUID[ATT_BT_UUID_SIZE] =
//{
//  LO_UINT16(DOORLOCK_OMT_UUID), HI_UINT16(DOORLOCK_OMT_UUID)
//};
//
//// Heartbeat Interval
//CONST uint8 hbiUUID[ATT_BT_UUID_SIZE] =
//{
//  LO_UINT16(DOORLOCK_HBI_UUID), HI_UINT16(DOORLOCK_HBI_UUID)
//};

// Parameters
CONST uint8 paraUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(DOORLOCK_PARA_UUID), HI_UINT16(DOORLOCK_PARA_UUID)
};

/*******************************************************************************
 * EXTERNAL VARIABLES
 */
extern custNV_t custParas;

/*********************************************************************
 * LOCAL VARIABLES
 */
static doorLockCBs_t *doorLock_AppCBs = NULL;
static uint8 rProps = GATT_PROP_READ;
static uint8 rwProps = GATT_PROP_READ | GATT_PROP_WRITE;
static uint8 swVer[DOORLOCK_SWVER_LEN] = {0};
static uint8 swVerUserDesp[] = "Software Version";
static uint8 pinUserDesp[] = "Pin Code";
//static uint8 dmUserDesp[] = "Data Mode";
//static uint8 tzUserDesp[] = "Time Zone";
//static uint8 mmUserDesp[] = "Motor Mode";
//static uint8 mdtUserDesp[] = "Motor Drive Time";
//static uint8 omUserDesp[] = "Office Mode";
//static uint8 omtUserDesp[] = "Office Mode Time";
//static uint8 hbiUserDesp[] = "Heartbeat Interval";
static uint8 para[DOORLOCK_PARA_CNT] = {0};
static uint8 paraUserDesp[] = "Door Lock Parameters";

/*********************************************************************
 * Profile Attributes - variables
 */
// Doorlock Service attribute
static CONST gattAttrType_t doorLockService = { ATT_BT_UUID_SIZE, doorLockServUUID };

static gattAttribute_t doorLockAttrTbl[] =
{
// Door Lock Service
  {
	{ ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
	GATT_PERMIT_READ,                         /* permissions */
	0,                                        /* handle */
	(uint8 *)&doorLockService                 /* pValue */
  },
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
	// Software Version Declaration
	{
	  { ATT_BT_UUID_SIZE, characterUUID },
	  GATT_PERMIT_READ,
	  0,
	  &rProps
	},

	// Software Version Value
	{
	  { ATT_BT_UUID_SIZE, swVerUUID },
	  GATT_PERMIT_READ,
	  0,
	  (uint8 *) swVer
	},

	// Software Version Description
	{
	  { ATT_BT_UUID_SIZE, charUserDescUUID },
	  GATT_PERMIT_READ,
	  0,
	  swVerUserDesp
	},
///////////////////////////////////////////////////////////////
	// pin Declaration
	{
	  { ATT_BT_UUID_SIZE, characterUUID },
	  GATT_PERMIT_READ,
	  0,
	  &rwProps
	},

	// pin Value
	{
      { ATT_BT_UUID_SIZE, pinUUID },
      GATT_PERMIT_READ | GATT_PERMIT_WRITE,
      0,
      (uint8 *) custParas.pin
	},

	// Pin Code Description
	{
	  { ATT_BT_UUID_SIZE, charUserDescUUID },
	  GATT_PERMIT_READ,
	  0,
	  pinUserDesp
	},
/////////////////////////////////////////////////////////////////
//	// Data Mode Declaration
//	{
//	  { ATT_BT_UUID_SIZE, characterUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  &rwProps
//	},
//
//	// Data Mode Value
//	{
//	  { ATT_BT_UUID_SIZE, dmUUID },
//	  GATT_PERMIT_READ | GATT_PERMIT_WRITE,
//	  0,
//	  (uint8 *) &custParas.dataMode
//	},
//
//	// Data Mode Description
//	{
//	  { ATT_BT_UUID_SIZE, charUserDescUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  dmUserDesp
//	},
/////////////////////////////////////////////////////////////////
//	// Time Zone Declaration
//	{
//	  { ATT_BT_UUID_SIZE, characterUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  &rwProps
//	},
//
//	// Time Zone Value
//	{
//	  { ATT_BT_UUID_SIZE, tzUUID },
//	  GATT_PERMIT_READ | GATT_PERMIT_WRITE,
//	  0,
//	  (uint8 *) &custParas.timeZone
//	},
//
//	// Time Zone Description
//	{
//	  { ATT_BT_UUID_SIZE, charUserDescUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  tzUserDesp
//	},
/////////////////////////////////////////////////////////////////
//	// Motor Mode Declaration
//	{
//	  { ATT_BT_UUID_SIZE, characterUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  &rwProps
//	},
//
//	// Motor Mode Value
//	{
//	  { ATT_BT_UUID_SIZE, mmUUID },
//	  GATT_PERMIT_READ | GATT_PERMIT_WRITE,
//	  0,
//	  (uint8 *) &custParas.motorMode
//	},
//
//	// Motor Mode Description
//	{
//	  { ATT_BT_UUID_SIZE, charUserDescUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  mmUserDesp
//	},
/////////////////////////////////////////////////////////////////
//	// Motor Drive Time Declaration
//	{
//	  { ATT_BT_UUID_SIZE, characterUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  &rwProps
//	},
//
//	// Motor Drive Time Value
//	{
//	  { ATT_BT_UUID_SIZE, mdtUUID },
//	  GATT_PERMIT_READ | GATT_PERMIT_WRITE,
//	  0,
//	  (uint8 *) &custParas.motorDrvTime
//	},
//
//	// Motor Drive Time Description
//	{
//	  { ATT_BT_UUID_SIZE, charUserDescUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  mdtUserDesp
//	},
/////////////////////////////////////////////////////////////////
//	// Office Mode Declaration
//	{
//	  { ATT_BT_UUID_SIZE, characterUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  &rwProps
//	},
//
//	// Office Mode Value
//	{
//	  { ATT_BT_UUID_SIZE, omUUID },
//	  GATT_PERMIT_READ | GATT_PERMIT_WRITE,
//	  0,
//	  (uint8 *) &custParas.officeMode
//	},
//
//	// Office Mode Description
//	{
//	  { ATT_BT_UUID_SIZE, charUserDescUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  omUserDesp
//	},
/////////////////////////////////////////////////////////////////
//	// Office Mode Time Declaration
//	{
//	  { ATT_BT_UUID_SIZE, characterUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  &rwProps
//	},
//
//	// Office Mode Time Value
//	{
//	  { ATT_BT_UUID_SIZE, omtUUID },
//	  GATT_PERMIT_READ | GATT_PERMIT_WRITE,
//	  0,
//	  (uint8 *) &custParas.officeTime
//	},
//
//	// Office Mode Time Description
//	{
//	  { ATT_BT_UUID_SIZE, charUserDescUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  omtUserDesp
//	},
/////////////////////////////////////////////////////////////////
//	// Heartbeat Interval Declaration
//	{
//	  { ATT_BT_UUID_SIZE, characterUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  &rwProps
//	},
//
//	// Heartbeat Interval Value
//	{
//	  { ATT_BT_UUID_SIZE, hbiUUID },
//	  GATT_PERMIT_READ | GATT_PERMIT_WRITE,
//	  0,
//	  (uint8 *) &custParas.hbInterval
//	},
//
//	// Heartbeat Interval Description
//	{
//	  { ATT_BT_UUID_SIZE, charUserDescUUID },
//	  GATT_PERMIT_READ,
//	  0,
//	  hbiUserDesp
//	},
///////////////////////////////////////////////////////////////
	// Parameters Declaration
	{
	  { ATT_BT_UUID_SIZE, characterUUID },
	  GATT_PERMIT_READ,
	  0,
	  &rwProps
	},

	// Parameters Value
	{
	  { ATT_BT_UUID_SIZE, paraUUID },
	  GATT_PERMIT_READ | GATT_PERMIT_WRITE,
	  0,
	  (uint8 *) para
	},

	// Parameters Description
	{
	  { ATT_BT_UUID_SIZE, charUserDescUUID },
	  GATT_PERMIT_READ,
	  0,
	  paraUserDesp
	},
};

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern void* memcpy(void *dest, const void *src, size_t len);

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t doorLock_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                     uint8 *pValue, uint16 *pLen, uint16 offset,
                                     uint16 maxLen, uint8 method );

static bStatus_t doorLock_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
									 uint8_t *pValue, uint16_t len,
									 uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Door Lock Service Callbacks
CONST gattServiceCBs_t doorLockCBs =
{
  doorLock_ReadAttrCB,  // Read callback function pointer
  doorLock_WriteAttrCB, // Write callback function pointer
  NULL                  // Authorization callback function pointer
};

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      DoorLock_AddService
 *
 * @brief
 *
 * @return  Success or Failure
 */
bStatus_t DoorLock_AddService( void )
{
	DoorLock_InitParameter();

	// Register GATT attribute list and CBs with GATT Server App
	return GATTServApp_RegisterService( doorLockAttrTbl,
                                      GATT_NUM_ATTRS( doorLockAttrTbl ),
                                      GATT_MAX_ENCRYPT_KEY_SIZE,
                                      &doorLockCBs );
}

/*********************************************************************
 * @fn      DoorLock_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t DoorLock_RegisterAppCBs( doorLockCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
	doorLock_AppCBs = appCallbacks;

    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      DoorLock_SetParameter
 *
 * @brief
 *
 * @return  Success or Failure
 */
bStatus_t DoorLock_InitParameter()
{
	swVer[0] = MAJOR_VER;
	swVer[1] = MINOR_VER;
	swVer[2] = BUILD_VER;

	para[DOORLOCK_PARA_IDX_DM] = (uint8)custParas.dataMode;
	para[DOORLOCK_PARA_IDX_TZ] = custParas.timeZone;
	para[DOORLOCK_PARA_IDX_MM] = (uint8)custParas.motorMode;
	para[DOORLOCK_PARA_IDX_MDT] = custParas.motorDrvTime;
	para[DOORLOCK_PARA_IDX_OM] = (uint8)custParas.officeMode;
	para[DOORLOCK_PARA_IDX_OMT] = custParas.officeTime;
	para[DOORLOCK_PARA_IDX_HBI] = (uint8)custParas.hbInterval;

	return SUCCESS;
}

/*********************************************************************
 * @fn          doorLock_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t doorLock_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                     uint8 *pValue, uint16 *pLen, uint16 offset,
                                     uint16 maxLen, uint8 method )
{
	bStatus_t status = SUCCESS;

	uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);

	switch (uuid)
	{
	case DOORLOCK_SWVER_UUID:{
		// verify offset
		if (offset > DOORLOCK_SWVER_LEN)
		{
			status = ATT_ERR_INVALID_OFFSET;
		}
		else
		{
			// determine read length
			*pLen = MIN(maxLen, (DOORLOCK_SWVER_LEN - offset));

			// copy data
			memcpy(pValue, &pAttr->pValue[offset], *pLen);
		}
		break;
	}
	case DOORLOCK_PIN_UUID:{
		// verify offset
		if (offset > PIN_LEN)
		{
			status = ATT_ERR_INVALID_OFFSET;
		}
		else
		{
			// determine read length
			*pLen = MIN(maxLen, (PIN_LEN - offset));

			// copy data
			memcpy(pValue, &pAttr->pValue[offset], *pLen);
		}
		break;
	}
//	case DOORLOCK_DM_UUID:
//	case DOORLOCK_TZ_UUID:
//	case DOORLOCK_MM_UUID:
//	case DOORLOCK_MDT_UUID:
//	case DOORLOCK_OM_UUID:
//	case DOORLOCK_OMT_UUID:
//	case DOORLOCK_HBI_UUID:{
//		// verify offset
//		*pLen = 1;
//		pValue[0] = *pAttr->pValue;
//		break;
//	}
	case DOORLOCK_PARA_UUID:{
		// verify offset
		if (offset > DOORLOCK_PARA_CNT)
		{
			status = ATT_ERR_INVALID_OFFSET;
		}
		else
		{
			// determine read length
			*pLen = MIN(maxLen, (DOORLOCK_PARA_CNT - offset));

			// copy data
			memcpy(pValue, &pAttr->pValue[offset], *pLen);
		}
		break;
	}
	default:{
		*pLen = 0;
		status = ATT_ERR_ATTR_NOT_FOUND;
		break;
	}
	}

	return ( status );
}

/*********************************************************************
 * @fn      doorLock_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t doorLock_WriteAttrCB(uint16_t connHandle,
									   gattAttribute_t *pAttr,
									   uint8_t *pValue, uint16_t len,
									   uint16_t offset, uint8_t method)
{
	bStatus_t status = SUCCESS;
	uint8 notifyApp = 0xFF;

	// If attribute permissions require authorization to write, return error
	if ( gattPermitAuthorWrite( pAttr->permissions ) )
	{
		// Insufficient authorization
		return ( ATT_ERR_INSUFFICIENT_AUTHOR );
	}

	if ( pAttr->type.len == ATT_BT_UUID_SIZE )
	{
		// 16-bit UUID
		uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
		switch ( uuid )
		{
		case DOORLOCK_PIN_UUID:{
			if ( offset == 0 )
			{
			  if ( len != PIN_LEN )
			  {
				status = ATT_ERR_INVALID_VALUE_SIZE;
			  }
			}
			else
			{
			  status = ATT_ERR_ATTR_NOT_LONG;
			}

			uint8_t i = 0;
			for(i=0; i<len; i++){
				if((pValue[i] < 0x30) || (pValue[i] > 0x39)){
					status = ATT_ERR_INVALID_VALUE;
					break;
				}
			}

			//Write the value
			if ( status == SUCCESS )
			{
				memcpy(pAttr->pValue, pValue, len);
				//notify APP
				notifyApp = DOORLOCK_PIN;
			}

			break;
		}
//		case DOORLOCK_DM_UUID:{
//			if(pValue[0] <= 2){
//				*((uint8 *)pAttr->pValue) = pValue[0];
//				notifyApp = DOORLOCK_DM;
//			}
//			else{
//				status = ATT_ERR_INVALID_VALUE;
//			}
//			break;
//		}
//		case DOORLOCK_TZ_UUID:{
//			if((pValue[0] <= 0x0C) || ((pValue[0] >= 0xF1) && (pValue[0] <= 0xFC))){
//				*((uint8 *)pAttr->pValue) = pValue[0];
//				notifyApp = DOORLOCK_TZ;
//			}
//			else{
//				status = ATT_ERR_INVALID_VALUE;
//			}
//			break;
//		}
//		case DOORLOCK_MM_UUID:{
//			if(pValue[0] <= 1){
//				*((uint8 *)pAttr->pValue) = pValue[0];
//				notifyApp = DOORLOCK_MM;
//			}
//			else{
//				status = ATT_ERR_INVALID_VALUE;
//			}
//			break;
//		}
//		case DOORLOCK_MDT_UUID:{
//			if((pValue[0] >= 4) && (pValue[0] <= 127)){
//				*((uint8 *)pAttr->pValue) = pValue[0];
//				notifyApp = DOORLOCK_MDT;
//			}
//			else{
//				status = ATT_ERR_INVALID_VALUE;
//			}
//			break;
//		}
//		case DOORLOCK_OM_UUID:{
//			if(pValue[0] <= 1){
//				*((uint8 *)pAttr->pValue) = pValue[0];
//				notifyApp = DOORLOCK_OM;
//			}
//			else{
//				status = ATT_ERR_INVALID_VALUE;
//			}
//			break;
//		}
//		case DOORLOCK_OMT_UUID:{
//			if((pValue[0] >= 2) && (pValue[0] <= 7)){
//				*((uint8 *)pAttr->pValue) = pValue[0];
//				notifyApp = DOORLOCK_OMT;
//			}
//			else{
//				status = ATT_ERR_INVALID_VALUE;
//			}
//			break;
//		}
//		case DOORLOCK_HBI_UUID:{
//			if(pValue[0] <= 3){
//				*((uint8 *)pAttr->pValue) = pValue[0];
//				notifyApp = DOORLOCK_HBI;
//			}
//			else{
//				status = ATT_ERR_INVALID_VALUE;
//			}
//			break;
//		}
		case DOORLOCK_PARA_UUID:{
			if ( offset == 0 )
			{
			  if ( len != DOORLOCK_PARA_CNT )
			  {
				status = ATT_ERR_INVALID_VALUE_SIZE;
			  }
			}
			else
			{
			  status = ATT_ERR_ATTR_NOT_LONG;
			}

			if(!(pValue[DOORLOCK_PARA_IDX_DM] <= 2)){
				status = ATT_ERR_INVALID_VALUE;
			}

			if(!((pValue[DOORLOCK_PARA_IDX_TZ] <= 0x0C) || ((pValue[DOORLOCK_PARA_IDX_TZ] >= 0xF1) && (pValue[DOORLOCK_PARA_IDX_TZ] <= 0xFC)))){
				status = ATT_ERR_INVALID_VALUE;
			}

			if(!(pValue[DOORLOCK_PARA_IDX_MM] <= 1)){
				status = ATT_ERR_INVALID_VALUE;
			}

			if(!((pValue[DOORLOCK_PARA_IDX_MDT] >= 4) && (pValue[DOORLOCK_PARA_IDX_MDT] <= 127))){
				status = ATT_ERR_INVALID_VALUE;
			}

			if(!(pValue[DOORLOCK_PARA_IDX_OM] <= 1)){
				status = ATT_ERR_INVALID_VALUE;
			}

			if(!((pValue[DOORLOCK_PARA_IDX_OMT] >= 2) && (pValue[DOORLOCK_PARA_IDX_OMT] <= 7))){
				status = ATT_ERR_INVALID_VALUE;
			}

			if(!(pValue[DOORLOCK_PARA_IDX_HBI] <= 3)){
				status = ATT_ERR_INVALID_VALUE;
			}

			//Write the value
			if ( status == SUCCESS )
			{
				memcpy(pAttr->pValue, pValue, len);

				custParas.dataMode = (dataMode_e)pAttr->pValue[DOORLOCK_PARA_IDX_DM];
				custParas.timeZone = pAttr->pValue[DOORLOCK_PARA_IDX_TZ];
				custParas.motorMode = (motorMode_e)pAttr->pValue[DOORLOCK_PARA_IDX_MM];
				custParas.motorDrvTime = pAttr->pValue[DOORLOCK_PARA_IDX_MDT];
				custParas.officeMode = (officeMode_e)pAttr->pValue[DOORLOCK_PARA_IDX_OM];
				custParas.officeTime = pAttr->pValue[DOORLOCK_PARA_IDX_OMT];
				custParas.hbInterval = (hbInterval_e)pAttr->pValue[DOORLOCK_PARA_IDX_HBI];
				//notify APP
				notifyApp = DOORLOCK_PARA;
			}
			break;
		}
		case GATT_CLIENT_CHAR_CFG_UUID:{
			status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
												 offset, GATT_CLIENT_CFG_NOTIFY );
			break;
		}

		default:{
			// Should never get here!
			status = ATT_ERR_ATTR_NOT_FOUND;
			break;
		}
		}
	}
	else{
		// 128-bit UUID
		status = ATT_ERR_INVALID_HANDLE;
	}

	//to notify APP
	if((notifyApp != 0xFF) && (doorLock_AppCBs) && (doorLock_AppCBs->pfnDoorLockChange)){
		doorLock_AppCBs->pfnDoorLockChange(notifyApp);
	}

	return ( status );
}

/*********************************************************************
*********************************************************************/

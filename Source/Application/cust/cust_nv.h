#ifndef CUST_NV_H
#define CUST_NV_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * NV map
 7             0
|- - - - - - - -|
|- - - - - - - -| PIN(compressed 4B)

|-                Engineer Mode(0 - 1)
 - -              Data Mode(0 - 2)
 - - - - -|       Time Zone(0 - 1, 0 - 12)

|-                Motor I/O Definition(0 - 1)
 - - - - - - -|   Motor Drive Time(4 - 127)(50ms - 1280ms)(10ms interval)

|-                Office Mode(0 - 1)
 - - -            Office Mode Time(2 - 7)(3s - 8s)
 - -              Heartbeat Interval(0 - 3)(30min - 120min)(30min interval)
 - -|             Reserved

|- - - - - - - -| Reserved
|- - - - - - - -| Reserved
|- - - - - - - -| Reserved
|- - - - - - - -| Reserved
|- - - - - - - -| Reserved
|- - - - - - - -| Reserved
|- - - - - - - -| Reserved
|- - - - - - - -| Reserved
|- - - - - - - -| Reserved
|- - - - - - - -| Reserved
|- - - - - - - -| Crc

*/
/******************************************************************************
 Include
*******************************************************************************/

/******************************************************************************
 Macro Definition
*******************************************************************************/
#define PIN_LEN 4 //PIN_LEN must be even!!!
/******************************************************************************
 Type Definition
*******************************************************************************/
typedef enum{
	DATAMODE_CMD = 0,
	DATAMODE_TRANSFER,
	DATAMODE_SILENT
} dataMode_e;

typedef enum{
	MOTORMODE_A = 0,
	MOTORMODE_B
} motorMode_e;

typedef enum{
	OFFICEMODE_DEACTIVATED = 0,
	OFFICEMODE_ACTIVATED
} officeMode_e;

typedef enum{
	HBINTERVAL_30MIN = 0,
	HBINTERVAL_60MIN,
	HBINTERVAL_90MIN,
	HBINTERVAL_120MIN
} hbInterval_e;

typedef struct{
	uint8_t pin[PIN_LEN];
	bool engMode;
	dataMode_e dataMode;
	uint8_t timeZone;
	motorMode_e motorMode;
	uint8_t motorDrvTime;
	officeMode_e officeMode;
	uint8_t officeTime;
	hbInterval_e hbInterval;
} custNV_t;
/******************************************************************************
 External Variable Definition
*******************************************************************************/
extern custNV_t custParas;
/******************************************************************************
 External Function Definition
*******************************************************************************/
extern void CustNV_init(void);
extern uint8_t CustNV_restore(void);
extern uint8_t CustNV_save(void);
extern uint8_t CustNV_getPin(char *pin);
extern uint8_t CustNV_setPin(char *pin);
extern uint8_t CustNV_getDatamode(uint8_t *dm);
extern uint8_t CustNV_setDatamode(uint8_t dm);
extern uint8_t CustNV_getTimeZone(int8_t *tz);
extern uint8_t CustNV_setTimeZone(int8_t tz);
extern uint8_t CustNV_getMotorMode(uint8_t *mode);
extern uint8_t CustNV_setMotorMode(uint8_t mode);
extern uint8_t CustNV_getMotorTime(uint8_t *time);
extern uint8_t CustNV_setMotorTime(uint8_t time);
extern uint8_t CustNV_getOfficeMode(uint8_t *mode);
extern uint8_t CustNV_setOfficeMode(uint8_t mode);
extern uint8_t CustNV_getOfficeTime(uint8_t *time);
extern uint8_t CustNV_setOfficeTime(uint8_t time);
extern uint8_t CustNV_getHBInterval(uint8_t *time);
extern uint8_t CustNV_setHBInterval(uint8_t time);

#ifdef __cplusplus
}
#endif

#endif /* CUST_NV_H */
/*******************************************************************************
 End of File
 */

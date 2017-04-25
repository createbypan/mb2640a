/******************************************************************************
 Include
*******************************************************************************/
#include "bcomdef.h"
#include "osal_snv.h"
#include "string.h"
#include "stdbool.h"
#include "mb2640a_util.h"
#include "cust_nv.h"
#include "doorlockservice.h"

#if (GL_LOG)
#include "cmd.h"
#endif
/******************************************************************************
 Macro Definition
*******************************************************************************/
#define LOG FALSE

#define CUST_NV_ID BLE_NVID_CUST_START
#define CUST_NV_SIZE 16//max 252B for each ID

#define DEFAULT_PIN "1662"
#define DEFAULT_TIMEZONE 8

#define NV_SECT_2 2//(PIN_LEN / 2)
#define NV_SECT_3 3//(NV_SECT_2 + 1)
#define NV_SECT_4 4//(NV_SECT_3 +　1)

#define CNV_ENGMODE_MASK 0x80
#define CNV_DATAMODE_MASK 0x60
#define CNV_TIMEZONE_MASK 0x1F
#define CNV_TIMEZONE_SIGN_MASK 0x10
#define CNV_MOTORMODE_MASK 0x80
#define CNV_MOTORDRVTIME_MASK 0x7F
#define CNV_OFFICEMODE_MASK 0x80
#define CNV_OFFICETIME_MASK 0x70
#define CNV_HBINTERVAL_MASK 0x0C

#define MIN_MOTORDRVTIME 4 //4 = (4 + 1) * 10ms
#define MAX_MOTORDRVTIME 127

#define MIN_OFFICETIME 2 //2 = (2 + 1) * 1s
#define MAX_OFFICETIME 7
/******************************************************************************
 Local CONST Definition
*******************************************************************************/

/******************************************************************************
 Local Function Declaration
*******************************************************************************/
static uint8_t pin_compress(uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len);
static uint8_t pin_decompress(uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len);
static uint8_t para_restore(uint8_t *data, custNV_t *paras);
static uint8_t para_read(uint8_t *data, custNV_t *paras);
static uint8_t para_write(uint8_t *data, custNV_t *paras);
/******************************************************************************
 External Function Declaration
*******************************************************************************/
custNV_t custParas = {0};
/******************************************************************************
 Local Variable Definition
*******************************************************************************/
static uint8_t custNV[CUST_NV_SIZE] = {0};

/******************************************************************************
 External Variable Declaration
*******************************************************************************/

/******************************************************************************
 Local Function Definition
*******************************************************************************/

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
void CustNV_init(void)
{
	uint8_t res = 0;
	uint8_t crc8 = 0;

	res = osal_snv_read(CUST_NV_ID, CUST_NV_SIZE, custNV);
	if(res != SUCCESS){//if never been written
		CustNV_restore();
	}

	crc8 = Util_crc8(custNV, CUST_NV_SIZE - 1);/*crc8 check*/
	if(crc8 != custNV[CUST_NV_SIZE - 1]){//if data corrupted
		CustNV_restore();
	}
	para_read(custNV, &custParas);
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_restore(void)
{
	uint8_t res = FAILURE;

#if (GL_LOG && LOG)
	Log_writeStr("CUST NV RESTORE\r\n");
#endif
	res = para_restore(custNV, &custParas) |
			osal_snv_write(CUST_NV_ID, CUST_NV_SIZE, custNV);
	DoorLock_InitParameter();
#if (GL_LOG && LOG)
	if(res != SUCCESS){
		Log_printf("CUST NV RESTORE FAIL: %d\r\n", res);
	}
#endif

	return res;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_save(void)
{
	uint8_t res = FAILURE;

#if (GL_LOG && LOG)
	Log_writeStr("CUST NV SAVE\r\n");
#endif
	res = para_write(custNV, &custParas);
	if(SUCCESS == res){
		res = osal_snv_write(CUST_NV_ID, CUST_NV_SIZE, custNV);
	}
#if (GL_LOG && LOG)
	if(res != SUCCESS){
		Log_printf("CUST NV SAVE FAIL: %d\r\n", res);
	}
#endif

	return res;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_getPin(char *pin)
{
	uint8_t i = 0;

	for(i=0; i<PIN_LEN; i++){
		pin[i] = custParas.pin[i];
	}
	pin[i] = '\0';

	return SUCCESS;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_setPin(char *pin)
{
	uint8_t i = 0;

	for(i=0; i<PIN_LEN; i++){
		custParas.pin[i] = pin[i];
	}

	return CustNV_save();
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_getDatamode(uint8_t *dm)
{
	*dm = (uint8_t)custParas.dataMode;

	return SUCCESS;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_setDatamode(uint8_t dm)
{
	if(dm > 2){
		return FAILURE;
	}

	custParas.dataMode = (dataMode_e)dm;

	return CustNV_save();
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_getTimeZone(int8_t *tz)
{
	if(custParas.timeZone & 0xF0){
		*tz = (custParas.timeZone & 0x0F) * (-1);
	}
	else{
		*tz = custParas.timeZone;
	}

	return SUCCESS;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_setTimeZone(int8_t tz)
{
	if(!((tz >= -12) && (tz <= 12))){
		return FAILURE;
	}

	if(tz < 0){
		custParas.timeZone = tz * (-1);
		custParas.timeZone |= 0xF0;
	}
	else{
		custParas.timeZone = tz;
	}

	return CustNV_save();
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_getMotorMode(uint8_t *mode)
{
	*mode = (uint8_t)custParas.motorMode;

	return SUCCESS;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_setMotorMode(uint8_t mode)
{
	if((mode != 0) && (mode != 1)){
		return FAILURE;
	}
	custParas.motorMode = (motorMode_e)mode;

	return CustNV_save();
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_getMotorTime(uint8_t *time)
{
	*time = custParas.motorDrvTime;

	return SUCCESS;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_setMotorTime(uint8_t time)
{
	if(!((time >= 4) && (time <= 127))){
		return FAILURE;
	}
	custParas.motorDrvTime = time;

	return CustNV_save();
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_getOfficeMode(uint8_t *mode)
{
	*mode = (uint8_t)custParas.officeMode;

	return SUCCESS;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_setOfficeMode(uint8_t mode)
{
	if((mode != 0) && (mode != 1)){
		return FAILURE;
	}
	custParas.officeMode = (officeMode_e)mode;

	return CustNV_save();
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_getOfficeTime(uint8_t *time)
{
	*time = custParas.officeTime;

	return SUCCESS;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_setOfficeTime(uint8_t time)
{
	if(!((time >= 2) && (time <= 7))){
		return FAILURE;
	}
	custParas.officeTime = time;

	return CustNV_save();
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_getHBInterval(uint8_t *time)
{
	*time = custParas.hbInterval;

	return SUCCESS;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t CustNV_setHBInterval(uint8_t time)
{
	if(time > 3){
		return FAILURE;
	}
	custParas.hbInterval = (hbInterval_e)time;

	return CustNV_save();
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t pin_compress(uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len)
{
	size_t i = 0;
	*dst_len = 0;

	for(i=0; i<src_len; i++){
		if((src[i] < 0x30) || (src[i] > 0x39)){
			return FAILURE;
		}
	}
	if(src_len % 2 == 0){
		for(i=0; i<src_len; i+=2){
			dst[i / 2] = (src[i] - 0x30) << 4;
			dst[i / 2] |= src[i + 1] - 0x30;
			(*dst_len)++;
		}
	}
	else{//src length is not even
		for(i=0; i<(src_len - 1); i+=2){
			dst[i / 2] = (src[i] - 0x30) << 4;
			dst[i / 2] |= src[i + 1] - 0x30;
			(*dst_len)++;
		}
		dst[i / 2] = (src[i] - 0x30) << 4;
		dst[i / 2] |= 0x0F;
		(*dst_len)++;
	}

	return SUCCESS;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t pin_decompress(uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len)
{
	size_t i = 0;
	*dst_len = 0;

	for(i=0; i<src_len; i++){
		dst[i * 2] = ((src[i] & 0xF0) >> 4) + 0x30;
		dst[i * 2 + 1] = (src[i] & 0x0F) + 0x30;
		(*dst_len) += 2;
	}
	if(0x0F == dst[*dst_len - 1]){
		(*dst_len)--;
	}

	return SUCCESS;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t para_restore(uint8_t *data, custNV_t *paras)
{
	uint8_t res = FAILURE;
	uint8_t i = 0;
	size_t len = 0;

	for(i=0; i<CUST_NV_SIZE; i++){
		data[i] = 0xFF;
	}

	res = pin_compress((uint8_t *)DEFAULT_PIN, PIN_LEN, data, &len);
	if(SUCCESS != res){
		return FAILURE;
	}
	res = pin_decompress(data, (PIN_LEN / 2), paras->pin, &len);
	if(SUCCESS != res){
		return FAILURE;
	}
	paras->engMode = false;
	paras->dataMode = DATAMODE_CMD;
	paras->timeZone = DEFAULT_TIMEZONE;
	paras->motorMode = MOTORMODE_A;
	paras->motorDrvTime = MIN_MOTORDRVTIME;
	paras->officeMode = OFFICEMODE_DEACTIVATED;
	paras->officeTime = MIN_OFFICETIME;
	paras->hbInterval = HBINTERVAL_60MIN;
	res = para_write(data, paras);

	return res;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t para_write(uint8_t *data, custNV_t *paras)
{
	uint8_t res = FAILURE;
	size_t len = 0;

	if(paras->dataMode > DATAMODE_SILENT){
		return FAILURE;
	}
	if(paras->motorMode > MOTORMODE_B){
		return FAILURE;
	}
	if((paras->motorDrvTime < MIN_MOTORDRVTIME) || (paras->motorDrvTime > MAX_MOTORDRVTIME)){
		return FAILURE;
	}
	if((paras->officeTime < MIN_OFFICETIME) || (paras->officeTime > MAX_OFFICETIME)){
		return FAILURE;
	}
	if(paras->hbInterval > HBINTERVAL_120MIN){
		return FAILURE;
	}
	if(!((paras->timeZone <= 12) || ((paras->timeZone >= 0xF1) && (paras->timeZone <= 0xFC)))){
		return FAILURE;
	}

	res = pin_compress(paras->pin, PIN_LEN, data, &len);
	if(SUCCESS != res){
		return FAILURE;
	}
	data[NV_SECT_2] = ((((uint8_t)paras->engMode) << 7) & CNV_ENGMODE_MASK) |
			((((uint8_t)paras->dataMode) << 5) & CNV_DATAMODE_MASK) |
			(paras->timeZone & CNV_TIMEZONE_MASK);
	data[NV_SECT_3] = ((((uint8_t)paras->motorMode) << 7) & CNV_MOTORMODE_MASK) |
			(paras->motorDrvTime & CNV_MOTORDRVTIME_MASK);
	data[NV_SECT_4] = ((((uint8_t)paras->officeMode) << 7) & CNV_OFFICEMODE_MASK) |
			((((uint8_t)paras->officeTime) << 4) & CNV_OFFICETIME_MASK) |
			((((uint8_t)paras->hbInterval) << 2) & CNV_HBINTERVAL_MASK);
	data[CUST_NV_SIZE - 1] = Util_crc8(data, CUST_NV_SIZE - 1);

	return SUCCESS;
}

/*********************************************************************
 * Function:
 * PreCondition:
 * Input:
 * Output:
 * Return:
 * Side Effects:
 * Overview:
 * Note:
 ********************************************************************/
uint8_t para_read(uint8_t *data, custNV_t *paras)
{
	uint8_t res = FAILURE;
	size_t len = 0;
	uint8_t value = 0;

	res = pin_decompress(data, (PIN_LEN / 2), paras->pin, &len);
	if(SUCCESS != res){
		return FAILURE;
	}

	//engineer mode context
	paras->engMode = (bool)((data[NV_SECT_2] & CNV_ENGMODE_MASK) >> 7);

	//data mode
	value = (data[NV_SECT_2] & CNV_DATAMODE_MASK) >> 5;
	if(value > (uint8_t)DATAMODE_SILENT){
		return FAILURE;
	}
	paras->dataMode = (dataMode_e)(value);

	//time zone
	paras->timeZone = data[NV_SECT_2] & CNV_TIMEZONE_MASK;
	if(paras->timeZone & CNV_TIMEZONE_SIGN_MASK){
		paras->timeZone |= 0xF0;
	}

	//motor I/O direction
	value = (data[NV_SECT_3] & CNV_MOTORMODE_MASK) >> 7;
	if(value > (uint8_t)MOTORMODE_B){
		return FAILURE;
	}
	paras->motorMode = (motorMode_e)(value);

	//motor drive time
	value = data[NV_SECT_3] & CNV_MOTORDRVTIME_MASK;
	if(value < MIN_MOTORDRVTIME){
		return FAILURE;
	}
	if(value > MAX_MOTORDRVTIME){
		return FAILURE;
	}
	paras->motorDrvTime = value;

	//office mode
	paras->officeMode = (officeMode_e)((data[NV_SECT_4] & CNV_OFFICEMODE_MASK) >> 7);

	//office mode time
	value = (data[NV_SECT_4] & CNV_OFFICETIME_MASK) >> 4;
	if(value < MIN_OFFICETIME){
		return FAILURE;
	}
	if(value > MAX_OFFICETIME){
		return FAILURE;
	}
	paras->officeTime = value;

	//heart beat interval
	value = (data[NV_SECT_4] & CNV_HBINTERVAL_MASK) >> 2;
	if(value > (uint8_t)HBINTERVAL_120MIN){
		return FAILURE;
	}
	paras->hbInterval = (hbInterval_e)value;

	return SUCCESS;
}

/*******************************************************************************
 End of File
 */

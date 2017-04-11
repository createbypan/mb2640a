/******************************************************************************
 Include
*******************************************************************************/
#include "bcomdef.h"
#include "osal_snv.h"
#include "string.h"
#include "stdbool.h"
#include "mb2640a_util.h"
/******************************************************************************
 Macro Definition
*******************************************************************************/
#define CUST_NV_SIZE (BLE_NVID_CUST_END - BLE_NVID_CUST_START + 1)

#define PIN_LEN 4
#define DEFAULT_PIN "1662"
#define DEFAULT_TIMEZONE 8

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
	bool officeMode;
	uint8_t officeTime;
	hbInterval_e hbInterval;
} custNV_t;
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

/******************************************************************************
 Local Variable Definition
*******************************************************************************/
static uint8_t custNV[CUST_NV_SIZE] = {0};
static custNV_t custParas = {0};
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

	res = osal_snv_read(BLE_NVID_CUST_START, CUST_NV_SIZE, (uint8_t *)custNV);
	if(res != SUCCESS){//if never been written
		para_restore(custNV, &custParas);
		osal_snv_write(BLE_NVID_CUST_START, CUST_NV_SIZE, (uint8_t *)custNV);
	}

	crc8 = Util_crc8(custNV, CUST_NV_SIZE - 1);/*crc8 check*/
	if(crc8 != custNV[CUST_NV_SIZE - 1]){//if data corrupted
		para_restore(custNV, &custParas);
		osal_snv_write(BLE_NVID_CUST_START, CUST_NV_SIZE, (uint8_t *)custNV);
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

	res = para_restore(custNV, &custParas);
	if(res != SUCCESS){
		return FAILURE;
	}
	res = osal_snv_write(BLE_NVID_CUST_START, CUST_NV_SIZE, (uint8_t *)custNV);

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

	res = para_write(custNV, &custParas);
	if(res != SUCCESS){
		return FAILURE;
	}
	res = osal_snv_write(BLE_NVID_CUST_START, CUST_NV_SIZE, (uint8_t *)custNV);

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
		dst[i * 2] = (src[i] & 0xF0) >> 4;
		dst[i * 2 + 1] = src[i] & 0x0F;
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
	paras->officeMode = false;
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

	res = pin_compress(paras->pin, PIN_LEN, data, &len);
	if(SUCCESS != res){
		return FAILURE;
	}
	data[2] = ((((uint8_t)paras->engMode) << 7) & CNV_ENGMODE_MASK) |
			((((uint8_t)paras->dataMode) << 5) & CNV_DATAMODE_MASK) |
			(paras->timeZone & CNV_TIMEZONE_MASK);
	data[3] = ((((uint8_t)paras->motorMode) << 7) & CNV_MOTORMODE_MASK) |
			(paras->motorDrvTime & CNV_MOTORDRVTIME_MASK);
	data[4] = ((((uint8_t)paras->officeMode) << 7) & CNV_OFFICEMODE_MASK) |
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
	paras->engMode = (bool)((data[2] & CNV_ENGMODE_MASK) >> 7);

	//data mode
	value = (data[2] & CNV_DATAMODE_MASK) >> 5;
	if(value > (uint8_t)DATAMODE_SILENT){
		return FAILURE;
	}
	paras->dataMode = (dataMode_e)(value);

	//time zone
	paras->timeZone = data[2] & CNV_TIMEZONE_MASK;

	//motor I/O direction
	value = (data[3] & CNV_MOTORMODE_MASK) >> 7;
	if(value > (uint8_t)MOTORMODE_B){
		return FAILURE;
	}
	paras->motorMode = (motorMode_e)(value);

	//motor drive time
	value = data[3] & CNV_MOTORDRVTIME_MASK;
	if(value < MIN_MOTORDRVTIME){
		return FAILURE;
	}
	if(value > MAX_MOTORDRVTIME){
		return FAILURE;
	}
	paras->motorDrvTime = value;

	//office mode
	paras->officeMode = (bool)((data[4] & CNV_OFFICEMODE_MASK) >> 7);

	//office mode time
	value = (data[4] & CNV_OFFICETIME_MASK) >> 4;
	if(value < MIN_OFFICETIME){
		return FAILURE;
	}
	if(value > MAX_OFFICETIME){
		return FAILURE;
	}
	paras->officeTime = value;

	//heart beat interval
	value = (data[4] & CNV_HBINTERVAL_MASK) >> 2;
	if(value > (uint8_t)HBINTERVAL_120MIN){
		return FAILURE;
	}
	paras->hbInterval = (hbInterval_e)value;

	return SUCCESS;
}

/*******************************************************************************
 End of File
 */

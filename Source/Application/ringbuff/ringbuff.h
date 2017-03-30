#ifndef RINGBUFF_H
#define RINGBUFF_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Include
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************
 Macro Definition
*******************************************************************************/

/******************************************************************************
 Type Definition
*******************************************************************************/
typedef struct RINGBUFF{
	uint16_t tail;
	uint16_t head;
	uint16_t count;//used unit count
    uint16_t size;//max. unit count
	bool isfull;
	bool isempty;
	uint8_t *buff;
} RingBuff;

/******************************************************************************
 External Variable Definition
*******************************************************************************/

/******************************************************************************
 External Function Definition
*******************************************************************************/
RingBuff *RingBuffInit(uint16_t size);
bool PushRingBuff(RingBuff * const buffer, const uint8_t value);
bool PopRingBuff(RingBuff * const buffer, uint8_t *value);
bool GetRingBuff(const RingBuff * const buffer, uint8_t *value, uint16_t index);
uint16_t WriteRingBuff(RingBuff * const buffer, const uint8_t *value, uint16_t size);
uint16_t ReadNDelRingBuff(RingBuff * const buffer, uint8_t *value, uint16_t size);
uint16_t ReadRingBuff(RingBuff * const buffer, uint8_t *value, uint16_t size);
uint16_t DeleteRingBuff(RingBuff * const buffer, uint16_t size);
void RingBuffIdxIncr(RingBuff * const buffer, uint16_t *idx);
void RingBuffIdxIncrBy(RingBuff * const buffer, uint16_t *idx, uint16_t step);
void RingBuffIdxDecr(RingBuff * const buffer, uint16_t *idx);
void RingBuffIdxDecrBy(RingBuff * const buffer, uint16_t *idx, uint16_t step);

#endif /* RINGBUFF_H */

#ifdef __cplusplus
}
#endif
/*******************************************************************************
 End of File
 */

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
RingBuff *Ringbuff_init(uint16_t size);
bool Ringbuff_push(RingBuff * const buffer, const uint8_t value);
bool Ringbuff_pop(RingBuff * const buffer, uint8_t *value);
bool Ringbuff_get(const RingBuff * const buffer, uint8_t *value, uint16_t index);
uint16_t Ringbuff_write(RingBuff * const buffer, const uint8_t *value, uint16_t size);
uint16_t Ringbuff_readNdel(RingBuff * const buffer, uint8_t *value, uint16_t size);
uint16_t Ringbuff_read(RingBuff * const buffer, uint8_t *value, uint16_t size);
uint16_t Ringbuff_delete(RingBuff * const buffer, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* RINGBUFF_H */
/*******************************************************************************
 End of File
 */

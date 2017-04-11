/******************************************************************************
 Include
*******************************************************************************/
#ifdef USE_ICALL
#include <ICall.h>
#else
#include <stdlib.h>
#endif

#include "ringbuff.h"
#include "string.h"
/******************************************************************************
 Macro Definition
*******************************************************************************/
#define st(x) do { x } while (__LINE__ == -1)

#define RINGBUFF_IDX_DECR(BUFFER, IDX) st( \
  if((IDX)-- == 0){ \
    (IDX) = (BUFFER)->size - 1; \
  } \
)

#define RINGBUFF_IDX_INCR(BUFFER, IDX) st( \
  if(++(IDX) >= (BUFFER)->size){ \
    (IDX) = 0; \
    } \
)

/******************************************************************************
 Local CONST Definition
*******************************************************************************/

/******************************************************************************
 Local Function Declaration
*******************************************************************************/
static uint16_t RingBuffAvail(RingBuff *buffer);
static bool IsValidIndex(const RingBuff * const buffer, uint16_t index);
static void RingBuffIdxIncr(RingBuff * const buffer, uint16_t *idx);
//static void RingBuffIdxIncrBy(RingBuff * const buffer, uint16_t *idx, uint16_t step);
//static void RingBuffIdxDecr(RingBuff * const buffer, uint16_t *idx);
//static void RingBuffIdxDecrBy(RingBuff * const buffer, uint16_t *idx, uint16_t step);

/******************************************************************************
 External Function Declaration
*******************************************************************************/

/******************************************************************************
 Local Variable Definition
*******************************************************************************/

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
RingBuff *Ringbuff_init(uint16_t size)
{
    RingBuff *buffer = ICall_malloc(sizeof(RingBuff));
    if(buffer){
        buffer->tail = 0;
        buffer->head = 0;
        buffer->count = 0;
        buffer->size = size;
        buffer->isfull = false;
        buffer->isempty = true;
        buffer->buff = ICall_malloc(buffer->size);
        if(buffer->buff){
            memset(buffer->buff, 0, buffer->size);
        }
        else{
        	ICall_free(buffer);
            buffer = NULL;
        }
    }
    
    return buffer;
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
uint16_t RingBuffAvail(RingBuff *buffer)
{
  return ((buffer->head > buffer->tail) ?
    (buffer->head - buffer->tail - 1) :
    (buffer->size - buffer->tail + buffer->head - 1));
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
bool Ringbuff_push(RingBuff * const buffer, const uint8_t value)
{
    if(buffer->isfull){
        return false;
    }
    else{
        buffer->buff[buffer->tail] = value;
        RINGBUFF_IDX_INCR(buffer, buffer->tail);
        buffer->count = buffer->size - RingBuffAvail(buffer) - 1;
        buffer->isfull = ((RingBuffAvail(buffer) == 0) ? true : false);
        buffer->isempty = false;
        return true;
    }
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
bool Ringbuff_pop(RingBuff * const buffer, uint8_t *value)
{
    if(buffer->isempty){
        return false;
    }
    else{
        *value = buffer->buff[buffer->head];
        buffer->buff[buffer->head] = 0;
        RINGBUFF_IDX_INCR(buffer, buffer->head);
        buffer->count = buffer->size - RingBuffAvail(buffer) - 1;
        buffer->isempty =((RingBuffAvail(buffer) == (buffer->size - 1)) ? true : false);
        buffer->isfull = false;
        return true;
    }
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
bool Ringbuff_get(const RingBuff * const buffer, uint8_t *value, uint16_t index)
{
    if(buffer->isempty){
        return false;
    }
    else{
        if(IsValidIndex(buffer, index)){
            *value = buffer->buff[index];
            return true;
        }
        else{
            return false;
        }
    }
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
bool IsValidIndex(const RingBuff * const buffer, uint16_t index)
{
    if(buffer->tail == buffer->head){/*no data in the buffer*//*this happens only when the Ringbuff initialized*/
        return false;
    }
    else if(buffer->tail > buffer->head){
        return ((index >= buffer->head) && (index < buffer->tail)) ? true : false;
    }
    else{
        if((index >= buffer->head) && (index < buffer->size)){
            return true;
        }
        if(index < buffer->tail){
            return true;
        }
        return false;
    }
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
uint16_t Ringbuff_write(RingBuff * const buffer, const uint8_t *value, uint16_t size)
{
    uint16_t count = 0;
    while((count < size) && (Ringbuff_push(buffer, *value))){
        count++;
        value++;
    }

    return count;
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
uint16_t Ringbuff_readNdel(RingBuff * const buffer, uint8_t *value, uint16_t size)
{
    uint16_t count = 0;
    while((count < size) && (Ringbuff_pop(buffer, value))){
        count++;
        value++;
    }

    return count;
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
uint16_t Ringbuff_read(RingBuff * const buffer, uint8_t *value, uint16_t size)
{
    uint16_t count = 0;
    uint16_t idx = buffer->head;
    while((count < size) && (Ringbuff_get(buffer, value, idx))){
        count++;
        value++;
        RingBuffIdxIncr(buffer, &idx);
    }

    return count;
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
uint16_t Ringbuff_delete(RingBuff * const buffer, uint16_t size)
{
    uint8_t value = 0;
    uint16_t count = 0;
    while((count < size) && (Ringbuff_pop(buffer, &value))){
        count++;
    }

    return count;
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
void RingBuffIdxIncr(RingBuff * const buffer, uint16_t *idx)
{
    RINGBUFF_IDX_INCR(buffer, *idx);
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
//void RingBuffIdxIncrBy(RingBuff * const buffer, uint16_t *idx, uint16_t step)
//{
//    uint16_t count = 0;
//    while(count < step){
//        RINGBUFF_IDX_INCR(buffer, *idx);
//        count++;
//    }
//}

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
//void RingBuffIdxDecr(RingBuff * const buffer, uint16_t *idx)
//{
//    RINGBUFF_IDX_DECR(buffer, *idx);
//}

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
//void RingBuffIdxDecrBy(RingBuff * const buffer, uint16_t *idx, uint16_t step)
//{
//    uint16_t count = 0;
//    while(count < step){
//        RINGBUFF_IDX_DECR(buffer, *idx);
//        count++;
//    }
//}

/*******************************************************************************
 End of File
 */

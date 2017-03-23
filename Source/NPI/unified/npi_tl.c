//******************************************************************************
//! \file           npi_tl.c
//! \brief          NPI Transport Layer API
//
//  Revised:        $Date: 2015-01-28 17:22:05 -0800 (Wed, 28 Jan 2015) $
//  Revision:       $Revision: 42106 $
//
//  Copyright 2015 Texas Instruments Incorporated. All rights reserved.
//
// IMPORTANT: Your use of this Software is limited to those specific rights
// granted under the terms of a software license agreement between the user
// who downloaded the software, his/her employer (which must be your employer)
// and Texas Instruments Incorporated (the "License").  You may not use this
// Software unless you agree to abide by the terms of the License. The License
// limits your use, and you acknowledge, that the Software may not be modified,
// copied or distributed unless used solely and exclusively in conjunction with
// a Texas Instruments radio frequency device, which is integrated into
// your product.  Other than for the foregoing purpose, you may not use,
// reproduce, copy, prepare derivative works of, modify, distribute, perform,
// display or sell this Software and/or its documentation for any purpose.
//
//  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
//  PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,l
//  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
//  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
//  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
//  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
//  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
//  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
//  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
//  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
//  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
//
//  Should you have any questions regarding your right to use this Software,
//  contact Texas Instruments Incorporated at www.TI.com.
//******************************************************************************

// ****************************************************************************
// includes
// ****************************************************************************
#include <string.h>
#include <xdc/std.h>
#include <ti/sysbios/family/arm/cc26xx/Power.h>
#include <ti/sysbios/family/arm/cc26xx/PowerCC2650.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "Board.h"
#include "hal_types.h"

#include "util.h"
#include "inc/npi_tl.h"
#include "inc/npi_data.h"
#include "inc/npi_util.h"

// ****************************************************************************
// defines
// ****************************************************************************

#if defined(NPI_USE_SPI)
#include "inc/npi_tl_spi.h"
#elif defined(NPI_USE_UART)
#include "inc/npi_tl_uart.h"
#else
#error "Must define an underlying serial bus for NPI"
#endif //NPI_USE_SPI

#ifdef POWER_SAVING
// Indexes for pin configurations in PIN_Config array
#define REM_RDY_PIN_IDX      0
#define LOC_RDY_PIN_IDX      1       

#define LocRDY_ENABLE()      PIN_setOutputValue(hNpiHandshakePins, locRdyPIN, 0)
#define LocRDY_DISABLE()     PIN_setOutputValue(hNpiHandshakePins, locRdyPIN, 1)
#else
#define LocRDY_ENABLE()
#define LocRDY_DISABLE()
#endif //POWER_SAVING

//PANMIN
#ifndef NPITL_TASK_STACK_SIZE
#define NPITL_TASK_STACK_SIZE 128
#endif

// Task configuration
#define NPITL_TASK_PRIORITY 1

//#define NPITL_PERIODIC_EVT 0x0001
//PANMIN-END

// ****************************************************************************
// typedefs
// ****************************************************************************
// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr;  // event header.
} npiTLEvt_t;

typedef enum
{
	NPITL_INIT = 0,
	NPITL_RESUME,
	NPITL_RUN,
	NPITL_SUSPEND,
	NPITL_DEINIT
} npiTLState_e;
//*****************************************************************************
// globals
//*****************************************************************************

//! \brief Flag for low power mode
static volatile bool npiPMSetConstraint = FALSE;

//! \brief Flag for ongoing NPI TX
static volatile bool npiTxActive = FALSE;

//! \brief The packet that was being sent when MRDY HWI negedge was received
static volatile uint32_t mrdyPktStamp = 0;

//! \brief Packets transmitted counter
static uint32_t txPktCount = 0;

//! \brief NPI Transport Layer receive buffer
uint8_t *npiRxBuf;

//! \brief Index to last byte written into NPI Transport Layer receive buffer
static uint16_t npiRxBufTail = 0;

//! \brief Index to first byte to read from NPI Transport Layer receive buffer
static uint16_t npiRxBufHead = 0;

//! \brief NPI Transport Layer transmit buffer
uint8_t *npiTxBuf;

//! \brief Number of bytes in NPI Transport Layer transmit buffer
static uint16_t npiTxBufLen = 0;

//PANMIN
#ifdef NPITL_TASK_PROTECT
static Semaphore_Struct npiTxSem = {0};
#endif //NPITL_TASK_PROTECT

//#ifdef NPI_USE_UART
//UART_Mode npiWriteMode = UART_MODE_CALLBACK;
//#endif
//PANMIN-END

//PANMIN
//! \brief Size of allocated Tx and Rx buffers
uint16_t npiBufSize = 0;
//npiTLCallBacks taskCBs;
//PANMIN-END

#ifdef POWER_SAVING
//! \brief PIN Config for Mrdy and Srdy signals without PIN IDs 
static PIN_Config npiHandshakePinsCfg[] =
{
    PIN_GPIO_OUTPUT_DIS | PIN_INPUT_EN | PIN_PULLUP,
    PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

static uint32_t remRdyPIN = (IOID_UNUSED & IOC_IOID_MASK);
static uint32_t locRdyPIN = (IOID_UNUSED & IOC_IOID_MASK);

//! \brief PIN State for remRdy and locRdy signals
static PIN_State npiHandshakePins;

//! \brief PIN Handles for remRdy and locRdy signals
static PIN_Handle hNpiHandshakePins;

//! \brief No way to detect whether positive or negative edge with PIN Driver
//!        Use a flag to keep track of state
static uint8_t remRdy_state;
#endif //POWER_SAVING

//PANMIN
// Task configuration
static Task_Struct npiTLTask;
static Char npiTLTaskStack[NPITL_TASK_STACK_SIZE];

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Semaphore globally used to post events to the application thread
static ICall_Semaphore sem;

static uint16_t events;
//static Clock_Struct periodicClock;
static npiTLState_e state = NPITL_INIT;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;
//PANMIN-END

//*****************************************************************************
// function prototypes
//*****************************************************************************
//PANMIN
static void NPITL_init( void );
static void NPITL_taskFxn(UArg a0, UArg a1);
static void NPITL_remRdyCB(uint8_t state);
static void NPITL_processAppMsg(npiTLEvt_t *pMsg);
//static void NPITL_clockHandler(UArg arg);
static void NPITL_initTL( void );
static void NPITL_deinitTL( void );
//PANMIN-END
//! \brief Call back function provided to underlying serial interface to be
//         invoked upon the completion of a transmission
static void NPITL_transmissionCallBack(uint16_t Rxlen, uint16_t Txlen);

#ifdef POWER_SAVING
//! \brief HWI interrupt function for remRdy
static void NPITL_remRdyPINHwiFxn(PIN_Handle hPin, PIN_Id pinId);

//! \brief This routine is used to set constraints on power manager
static void NPITL_setPM(void);

//! \brief This routine is used to release constraints on power manager
static void NPITL_relPM(void);
#endif //POWER_SAVING

//PANMIN
static NPITL_Params npiTLparams = {
	  32,
	  Board_MRDY_PIN,
	  Board_SRDY_PIN,
	  NPI_SERIAL_TYPE_UART,
	  CC2650_UART0,
	  {
			  UART_MODE_CALLBACK,   /* readMode */
			  UART_MODE_BLOCKING,   /* writeMode *//* UART_MODE_BLOCKING to protect the uart from overlap in a task */
			  0,    /* readTimeout */
			  UART_WAIT_FOREVER,    /* writeTimeout */
			  NULL,                 /* readCallback *//* no need to config */
			  NULL,                 /* writeCallback *//* no need to config */
			  UART_RETURN_NEWLINE,  /* readReturnMode */
			  UART_DATA_BINARY,       /* readDataMode */
			  UART_DATA_BINARY,       /* writeDataMode */
			  UART_ECHO_OFF,         /* readEcho */
			  115200,               /* baudRate */
			  UART_LEN_8,           /* dataLength */
			  UART_STOP_ONE,        /* stopBits */
			  UART_PAR_NONE,        /* parityType */
			  NULL                  /* custom */
	  },
	  {
			  NPITL_remRdyCB,		/* npiMrdyRtosCB_t *remRdyCB */
			  NULL					/* npiRtosCB_t *transCompleteCB */
	  }
};
//PANMIN-END

//PANMIN
/*********************************************************************
 * @fn      NPITL_createTask
 *
 * @brief   Task creation function for the NPI TL.
 *
 * @param   None.
 *
 * @return  None.
 */
void NPITL_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = npiTLTaskStack;
  taskParams.stackSize = NPITL_TASK_STACK_SIZE;
  taskParams.priority = NPITL_TASK_PRIORITY;

  Task_construct(&npiTLTask, NPITL_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      NPITL_taskFxn
 *
 * @brief   Application task entry point for the NPI TL.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void NPITL_taskFxn(UArg a0, UArg a1)
{
	// Initialize application
	NPITL_init();

	// Application main loop
	for (;;)
	{
		switch(state){
		case NPITL_INIT:{
			NPITL_initTL();
			state = NPITL_RESUME;
			break;
		}
		case NPITL_RESUME:{
			NPITL_openTL(NULL);
			state = NPITL_RUN;
			break;
		}
		case NPITL_RUN:{
			NPITL_writeTL("NPITL_RUN\r\n", 11);
			NPITLUART_handleRemRdyEvent();
			Task_sleep(100000);
			break;
		}
		case NPITL_SUSPEND:{
			NPITL_closeTL();
			state = NPITL_RESUME;
			break;
		}
		case NPITL_DEINIT:{
			NPITL_deinitTL();
			break;
		}
		}
		// Waits for a signal to the semaphore associated with the calling thread.
		// Note that the semaphore associated with a thread is signaled when a
		// message is queued to the message receive queue of the thread or when
		// ICall_signal() function is called onto the semaphore.
//		ICall_Errno errno = ICall_wait(ICALL_TIMEOUT_FOREVER);
//
//		if (errno == ICALL_ERRNO_SUCCESS)
//		{
//		  // If RTOS queue is not empty, process app message.
//		  while (!Queue_empty(appMsgQueue))
//		  {
//			npiTLEvt_t *pMsg = (npiTLEvt_t *)Util_dequeueMsg(appMsgQueue);
//			if (pMsg)
//			{
//			  // Process message.
//			  NPITL_processAppMsg(pMsg);
//
//			  // Free the space from the message.
//			  ICall_free(pMsg);
//			}
//		  }
//		}
//
//		if (events & NPITL_PERIODIC_EVT)
//		{
//		  events &= ~NPITL_PERIODIC_EVT;
//
//		  Util_startClock(&periodicClock);
//		}
	}
}

/*********************************************************************
 * @fn      NPITL_clockHandler
 *
 * @brief
 *
 * @param
 *
 * @return  None.
 */
//static void NPITL_clockHandler(UArg arg)
//{
//  // Store the event.
//  events |= arg;
//
//  // Wake up the application.
//  Semaphore_post(sem);
//}

/*********************************************************************
 * @fn      NPITL_processAppMsg
 *
 * @brief
 *
 * @param
 *
 * @return  None.
 */
static void NPITL_processAppMsg(npiTLEvt_t *pMsg)
{
	switch (pMsg->hdr.event)
	{
		default:
			  // Do nothing.
			  break;
	}
}

/*********************************************************************
 * @fn      NPITL_remRdyCB
 *
 * @brief
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void NPITL_remRdyCB(uint8_t state)
{
}

/*********************************************************************
 * @fn      NPITL_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 *
 * @param   None.
 *
 * @return  None.
 */
static void NPITL_init( void )
{
  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
//  ICall_registerApp(&selfEntity, &sem);

  // Create an RTOS queue for message from profile to be sent to app.
//  appMsgQueue = Util_constructQueue(&appMsg);

//  Util_constructClock(&periodicClock, NPITL_clockHandler,
//                        1000, 0, true, NPITL_PERIODIC_EVT);
}
//PANMIN-END

// -----------------------------------------------------------------------------
//! \brief      This routine initializes the transport layer
//!
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITL_initTL( void )
{
	_npiCSKey_t key;
	key = NPIUtil_EnterCS();

	//PANMIN
#ifdef NPITL_TASK_PROTECT
	Semaphore_Params semParams;
	Semaphore_Params_init(&semParams);
	semParams.mode = Semaphore_Mode_BINARY;
	Semaphore_construct(&npiTxSem, 1, &semParams);
	Semaphore_pend(Semaphore_handle(&npiTxSem), UART_WAIT_FOREVER);
#endif //NPITL_TASK_PROTECT
	//PANMIN-END

	// Allocate memory for Transport Layer Tx/Rx buffers
	npiBufSize = npiTLparams.npiTLBufSize;
	npiRxBuf = NPIUTIL_MALLOC(npiBufSize);
	memset(npiRxBuf, 0, npiBufSize);
	npiTxBuf = NPIUTIL_MALLOC(npiBufSize);
	memset(npiTxBuf, 0, npiBufSize);

#ifdef POWER_SAVING
	// Assign PIN IDs to remRdy and locRrdy
#ifdef NPI_MASTER
	remRdyPIN = (npiTLparams.srdyPinID & IOC_IOID_MASK);
	locRdyPIN = (npiTLparams.mrdyPinID & IOC_IOID_MASK);
#else
	remRdyPIN = (npiTLparams.mrdyPinID & IOC_IOID_MASK);
	locRdyPIN = (npiTLparams.srdyPinID & IOC_IOID_MASK);
#endif //NPI_MASTER

	// Add PIN IDs to PIN Configuration
	npiHandshakePinsCfg[REM_RDY_PIN_IDX] |= remRdyPIN;
	npiHandshakePinsCfg[LOC_RDY_PIN_IDX] |= locRdyPIN;

	// Initialize LOCRDY/REMRDY. Enable int after callback registered
	hNpiHandshakePins = PIN_open(&npiHandshakePins, npiHandshakePinsCfg);
	PIN_registerIntCb(hNpiHandshakePins, NPITL_remRdyPINHwiFxn);
	PIN_setConfig(hNpiHandshakePins,
				  PIN_BM_IRQ,
				  remRdyPIN | PIN_IRQ_BOTHEDGES);

	// Enable wakeup
	PIN_setConfig(hNpiHandshakePins,
				  PINCC26XX_BM_WAKEUP,
				  remRdyPIN | PINCC26XX_WAKEUP_NEGEDGE);

	remRdy_state = PIN_getInputValue(remRdyPIN);

	// If MRDY is already low then we must initiate a read because there was
	// a prior MRDY negedge that was missed
	if (!remRdy_state)
	{
		NPITL_setPM();
		//PANMIN
		if (npiTLparams.npiCallBacks.remRdyCB/*taskCBs.remRdyCB*/)
		//PANMIN-END
		{
			transportRemRdyEvent();
			LocRDY_ENABLE();
		}
	}
#endif //POWER_SAVING

	NPIUtil_ExitCS(key);
}

// -----------------------------------------------------------------------------
//! \brief      This routine opens the port of the device.
//!             Note that based on project defines, either the
//!             UART, or SPI driver can be used.
//!
//! \param[in]  params - Transport Layer parameters
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITL_openTL(NPITL_Params *params)
{
	(void) params;
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();
    
#if defined(NPI_USE_UART)
    transportOpen(npiTLparams.portBoardID,
                  &npiTLparams.portParams.uartParams,
                  NPITL_transmissionCallBack);
#elif defined(NPI_USE_SPI)
    transportOpen(npiTLparams.portBoardID,
                  &npiTLparams.portParams.spiParams,
                  NPITL_transmissionCallBack);
#endif //NPI_USE_UART

#ifndef POWER_SAVING
    // This call will start repeated Uart Reads when Power Savings is disabled
    transportRead();
#endif //!POWER_SAVING

    //PANMIN
#ifdef NPITL_TASK_PROTECT
    Semaphore_post(Semaphore_handle(&npiTxSem));
#endif
    //PANMIN-END

    NPIUtil_ExitCS(key);
}

// -----------------------------------------------------------------------------
//! \brief      This routine de-init the transport layer
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITL_deinitTL( void )
{
	_npiCSKey_t key;
	key = NPIUtil_EnterCS();

	// Free Transport Layer RX/TX buffers
	npiBufSize = 0;
	NPIUTIL_FREE(npiRxBuf);
	NPIUTIL_FREE(npiTxBuf);

#ifdef POWER_SAVING
	// Clear mrdy and srdy PIN IDs
	remRdyPIN = (IOID_UNUSED & IOC_IOID_MASK); // Set to 0x000000FF
	locRdyPIN = (IOID_UNUSED & IOC_IOID_MASK); // Set to 0x000000FF

	// Clear PIN IDs from PIN Configuration
	npiHandshakePinsCfg[REM_RDY_PIN_IDX] &= ~remRdyPIN;
	npiHandshakePinsCfg[LOC_RDY_PIN_IDX] &= ~locRdyPIN;

	// Close PIN Handle
	PIN_close(hNpiHandshakePins);

	// Release Power Management
	NPITL_relPM();
#endif //POWER_SAVING

	//PANMIN
#ifdef NPITL_TASK_PROTECT
	Semaphore_destruct(&npiTxSem);
#endif //NPITL_TASK_PROTECT
	//PANMIN-END

	NPIUtil_ExitCS(key);
}

// -----------------------------------------------------------------------------
//! \brief      This routine closes the transport layer
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITL_closeTL( void )
{
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();
    
    // Close Transport Layer 
    transportClose();

    //PANMIN
#ifdef NPITL_TASK_PROTECT
    Semaphore_pend(Semaphore_handle(&npiTxSem), UART_WAIT_FOREVER);
#endif //NPITL_TASK_PROTECT
    //PANMIN-END

    NPIUtil_ExitCS(key);
}
  
// -----------------------------------------------------------------------------
//! \brief      This routine returns the state of transmission on NPI
//!
//! \return     bool - state of NPI transmission - 1 - active, 0 - not active
// -----------------------------------------------------------------------------
bool NPITL_checkNpiBusy( void )
{
#ifdef POWER_SAVING
#ifdef NPI_MASTER
    return !PIN_getOutputValue(locRdyPIN) || !PIN_getInputValue(remRdyPIN);
#else
    return !PIN_getOutputValue(locRdyPIN);
#endif //NPI_MASTER
#else
    //PANMIN
#ifdef NPI_USE_UART
    if(UART_MODE_CALLBACK == npiTLparams.portParams.uartParams.writeMode){
    	return npiTxActive;//if UART_MODE_CALLBACK we need to wait callback invoked to protect the uart from overlap
    }
    else{
    	return false;//if UART_MODE_BLOCKING we dont need to wait, the writeSem will protect the uart from overlap
    }
#else
    return npiTxActive;
#endif //NPI_USE_UART
    //PANMIN-END
#endif //POWER_SAVING
}

#ifdef POWER_SAVING
// -----------------------------------------------------------------------------
//! \brief      This routine is used to set constraints on power manager
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITL_setPM(void)
{
    if (npiPMSetConstraint)
    {
        return;
    }

    // set constraints for Standby and idle mode
    Power_setConstraint(Power_SB_DISALLOW);
    Power_setConstraint(Power_IDLE_PD_DISALLOW);
    npiPMSetConstraint = TRUE;
}
#endif //POWER_SAVING

#ifdef POWER_SAVING
// -----------------------------------------------------------------------------
//! \brief      This routine is used to release constraints on power manager
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITL_relPM(void)
{
    if (!npiPMSetConstraint)
    {
        return;
    }

    // release constraints for Standby and idle mode
    Power_releaseConstraint(Power_SB_DISALLOW);
    Power_releaseConstraint(Power_IDLE_PD_DISALLOW);
    npiPMSetConstraint = FALSE;
}
#endif //POWER_SAVING

#ifdef POWER_SAVING
// -----------------------------------------------------------------------------
//! \brief      This routine is used to handle an MRDY transition from a task
//!             context. Certain operations such as UART_read() cannot be
//!             performed from a HWI context
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITL_handleRemRdyEvent(void)
{
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();
  
    // Check to make sure this event is not occurring during the next packet
    // transmission
    if (PIN_getInputValue(remRdyPIN) == 0 || 
        (npiTxActive && mrdyPktStamp == txPktCount))
    {
        transportRemRdyEvent();
        LocRDY_ENABLE();
    }
    
    NPIUtil_ExitCS(key);
}

// -----------------------------------------------------------------------------
//! \brief      This is a HWI function handler for the MRDY pin. Some MRDY
//!             functionality can execute from this HWI context. Others
//!             must be executed from task context hence the taskCBs.remRdyCB()
//!
//! \param[in]  hPin - PIN Handle
//! \param[in]  pinId - ID of pin that triggered HWI
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITL_remRdyPINHwiFxn(PIN_Handle hPin, PIN_Id pinId)
{
    // The pin driver does not currently support returning whether the int
    // was neg or pos edge so we must use a variable to keep track of state. 
    remRdy_state ^= 1;
    
    if (remRdy_state == 0)
    {
        mrdyPktStamp = txPktCount;
        NPITL_setPM();
    }
    else
    {
        transportStopTransfer();
    }
    
    // Signal to registered task that Rem Ready signal has changed state
    //PANMIN
	if (npiTLparams.npiCallBacks.remRdyCB/*taskCBs.remRdyCB*/)
	//PANMIN-END
    {
		npiTLparams.npiCallBacks.remRdyCB(remRdy_state);
    }
    
    // Check the physical state of the pin to see if it matches the variable 
    // state. If not trigger another task call back
    if (remRdy_state != PIN_getInputValue(remRdyPIN))
    {
        remRdy_state = PIN_getInputValue(remRdyPIN);
        
        //PANMIN
		if (npiTLparams.npiCallBacks.remRdyCB/*taskCBs.remRdyCB*/)
		//PANMIN-END
        {
			npiTLparams.npiCallBacks.remRdyCB(remRdy_state);
        }
    }
}
#endif //POWER_SAVING

// -----------------------------------------------------------------------------
//! \brief      This callback is invoked on the completion of one transmission
//!             to/from the host MCU. Any bytes receives will be [0,Rxlen) in
//!             npiRxBuf.
//!             If bytes were receives or transmitted, this function notifies
//!             the NPI task via registered call backs
//!
//! \param[in]  Rxlen   - length of the data received
//! \param[in]  Txlen   - length of the data transferred
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITL_transmissionCallBack(uint16_t Rxlen, uint16_t Txlen)
{
    npiRxBufHead = 0;
    npiRxBufTail = Rxlen;
    npiTxActive = FALSE;
#ifdef NPITL_TASK_PROTECT
    Semaphore_post(Semaphore_handle(&npiTxSem));
#endif //NPITL_TASK_PROTECT
    
    // If Task is registered, invoke transaction complete callback
    //PANMIN
	if (npiTLparams.npiCallBacks.transCompleteCB/*taskCBs.transCompleteCB*/)
	//PANMIN-END
    {
    	npiTLparams.npiCallBacks.transCompleteCB(Rxlen, Txlen);
    }

#ifdef POWER_SAVING
    NPITL_relPM();
    LocRDY_DISABLE();
#endif //POWER_SAVING
}

// -----------------------------------------------------------------------------
//! \brief      This routine reads data from the transport layer based on len,
//!             and places it into the buffer.
//!
//! \param[in]  buf - Pointer to buffer to place read data.
//! \param[in]  len - Number of bytes to read.
//!
//! \return     uint16_t - the number of bytes read from transport
// -----------------------------------------------------------------------------
uint16_t NPITL_readTL(uint8_t *buf, uint16_t len)
{
    // Only copy the lowest number between len and bytes remaining in buffer
    len = (len > NPITL_getRxBufLen()) ? NPITL_getRxBufLen() : len;

    memcpy(buf, &npiRxBuf[npiRxBufHead], len);
    npiRxBufHead += len;

    return len;
}

// -----------------------------------------------------------------------------
//! \brief      This routine writes data from the buffer to the transport layer.
//!
//! \param[in]  buf - Pointer to buffer to write data from.
//! \param[in]  len - Number of bytes to write.
//!
//! \return     uint16_t - NPI error code value
// -----------------------------------------------------------------------------
uint8_t NPITL_writeTL(uint8_t *buf, uint16_t len)
{
#ifdef POWER_SAVING
    _npiCSKey_t key;
    key = NPIUtil_EnterCS();
#endif //POWER_SAVING
    
    //PANMIN
#ifdef NPITL_TASK_PROTECT
    Semaphore_pend(Semaphore_handle(&npiTxSem), UART_WAIT_FOREVER);
#endif //NPITL_TASK_PROTECT
    //PANMIN-END
    // Check to make sure NPI is not currently in a transaction
    if (NPITL_checkNpiBusy())
    {
#ifdef POWER_SAVING
        NPIUtil_ExitCS(key);
#endif // POWER_SAVING

        return NPI_BUSY;
    }
    
    // Check to make sure that write size is not greater than what is 
    // allowed
    if (len > npiBufSize)
    {
#ifdef POWER_SAVING
        NPIUtil_ExitCS(key);
#endif // POWER_SAVING

        return NPI_TX_MSG_OVERSIZE;
    }
    
    // Copy into the second byte of npiTxBuf. This will save Serial Port
    // Specific TL code from having to shift one byte later on for SOF. 
	//PANMIN - we dont need packet(unified NPI protocol)
	//refer to the url below to know details about unified NPI protocol
	//http://processors.wiki.ti.com/index.php/Unified_Network_Processor_Interface?keyMatch=NPI&tisearch=Search-EN-Everything
#ifdef NPITL_UNIFIED_PROTOCOL
    memcpy(&npiTxBuf[1], buf, len);
#else
	memcpy(npiTxBuf, buf, len);
#endif
	//PANMIN-END
    npiTxBufLen = len;
    npiTxActive = TRUE;
    txPktCount++;

    transportWrite(npiTxBufLen);
    //PANMIN
#ifdef NPITL_TASK_PROTECT
#ifdef NPI_USE_UART
    if(UART_MODE_BLOCKING == npiTLparams.portParams.uartParams.writeMode){
    	Semaphore_post(Semaphore_handle(&npiTxSem));//if UART_MODE_BLOCKING we dont wait
    }
    else{//if UART_MODE_CALLBACK we need to wait the callback invoked

    }
#else
#endif //NPI_USE_UART
#endif //NPITL_TASK_PROTECT
    //PANMIN-END

#ifdef POWER_SAVING
    //LocRDY_ENABLE();
    NPIUtil_ExitCS(key);
#endif //POWER_SAVING

    return NPI_SUCCESS;
}

// -----------------------------------------------------------------------------
//! \brief      This routine returns the max size receive buffer.
//!
//! \return     uint16_t - max size of the receive buffer
// -----------------------------------------------------------------------------
uint16_t NPITL_getMaxRxBufSize(void)
{
    return(npiBufSize);
}

// -----------------------------------------------------------------------------
//! \brief      This routine returns the max size transmit buffer.
//!
//! \return     uint16_t - max size of the transmit buffer
// -----------------------------------------------------------------------------
uint16_t NPITL_getMaxTxBufSize(void)
{
    return(npiBufSize);
}

// -----------------------------------------------------------------------------
//! \brief      Returns number of bytes that are unread in RxBuf
//!
//! \return     uint16_t - number of unread bytes
// -----------------------------------------------------------------------------
uint16_t NPITL_getRxBufLen(void)
{
    return ((npiRxBufTail - npiRxBufHead) + npiBufSize) % npiBufSize;
}

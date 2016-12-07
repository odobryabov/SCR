/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbport.h,v 1.17 2006/12/07 22:10:34 wolti Exp $
 *            mbport.h,v 1.60 2013/08/17 11:42:56 Armink Add Master Functions  $
 */

#ifndef _MB_PORT_H
#define _MB_PORT_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

#include "port.h"
/* ----------------------- Defines ------------------------------------------*/


/* ----------------------- Type definitions ---------------------------------*/



/* ----------------------- Supporting functions -----------------------------*/
BOOL            xMBPortEventInit( MBPort_InitTypeDef* mbPort );

BOOL            xMBPortEventPost( MBPort_InitTypeDef* mbPort, eMBEventType eEvent );

BOOL            xMBPortEventGet( MBPort_InitTypeDef* mbPort, eMBEventType * eEvent );

/* ----------------------- Serial port functions ----------------------------*/

BOOL 						xMBPortSerialInit( MBPort_InitTypeDef* mbPort, UCHAR ucPORT, ULONG ulBaudRate,
                        UCHAR ucDataBits, eMBParity eParity );

void            vMBPortClose( void );

void 						vMBPortSerialEnable(  MBPort_InitTypeDef* mbPort, BOOL xRxEnable, BOOL xTxEnable );

void						vMBDeWritePin(MBPort_InitTypeDef* mbPort, GPIO_PinState PinState);			

INLINE BOOL     xMBPortSerialGetByte( UART_HandleTypeDef* huart, CHAR * pucByte );

INLINE BOOL     xMBPortSerialPutByte( UART_HandleTypeDef* huart, CHAR ucByte );

/* ----------------------- Timers functions ---------------------------------*/
BOOL 						xMBPortTimersInit( MBPort_InitTypeDef* mbPort, USHORT usTim1Timerout50us );

INLINE void     vMBPortTimersEnable(TIM_HandleTypeDef *htim);

INLINE void     vMBPortTimersDisable(TIM_HandleTypeDef *htim);

/* ----------------------- Callback for the protocol stack ------------------*/

/*!
 * \brief Callback function for the porting layer when a new byte is
 *   available.
 *
 * Depending upon the mode this callback function is used by the RTU or
 * ASCII transmission layers. In any case a call to xMBPortSerialGetByte()
 * must immediately return a new character.
 *
 * \return <code>TRUE</code> if a event was posted to the queue because
 *   a new byte was received. The port implementation should wake up the
 *   tasks which are currently blocked on the eventqueue.
 */
extern          BOOL( *pxMBFrameCBByteReceived ) ( MBPort_InitTypeDef* mbPort );

extern          BOOL( *pxMBFrameCBTransmitterEmpty ) ( MBPort_InitTypeDef* mbPort  );

extern          BOOL( *pxMBPortCBTimerExpired ) ( MBPort_InitTypeDef* mbPort );



/* ----------------------- TCP port functions -------------------------------*/
BOOL            xMBTCPPortInit( USHORT usTCPPort );

void            vMBTCPPortClose( void );

void            vMBTCPPortDisable( void );

BOOL            xMBTCPPortGetRequest( UCHAR **ppucMBTCPFrame, USHORT * usTCPLength );

BOOL            xMBTCPPortSendResponse( const UCHAR *pucMBTCPFrame, USHORT usTCPLength );


/*-------------------------MASTER Supporting functions------------------------*/

BOOL            xMBMasterPortEventInit( void );

BOOL            xMBMasterPortEventPost( eMBMasterEventType eEvent );

BOOL            xMBMasterPortEventGet(  /*@out@ */ eMBMasterEventType * eEvent );

BOOL            xMBMasterRunResTake( LONG time );

void            vMBMasterRunResRelease( void );

void            vMBMasterOsResInit( void );

/* -----------------------MASTER Serial port functions -------------------------*/
BOOL            xMBMasterPortSerialInit( UCHAR ucPort, ULONG ulBaudRate,
                                   UCHAR ucDataBits, eMBParity eParity );

void            vMBMasterPortClose( void );

void            xMBMasterPortSerialClose( void );

void            vMBMasterPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable );

INLINE BOOL     xMBMasterPortSerialGetByte( CHAR * pucByte );

INLINE BOOL     xMBMasterPortSerialPutByte( CHAR ucByte );

/* -----------------------MASTER Timers functions -------------------------------*/

BOOL            xMBMasterPortTimersInit( USHORT usTimeOut50us );

void            xMBMasterPortTimersClose( void );

INLINE void     vMBMasterPortTimersT35Enable( void );

INLINE void     vMBMasterPortTimersConvertDelayEnable( void );

INLINE void     vMBMasterPortTimersRespondTimeoutEnable( void );

INLINE void     vMBMasterPortTimersDisable( void );

/* ----------------- Callback for the master error process -----------------------*/
void            vMBMasterErrorCBRespondTimeout( UCHAR ucDestAddress, const UCHAR* pucPDUData,
                                                USHORT ucPDULength );

void            vMBMasterErrorCBReceiveData( UCHAR ucDestAddress, const UCHAR* pucPDUData,
                                             USHORT ucPDULength );

void            vMBMasterErrorCBExecuteFunction( UCHAR ucDestAddress, const UCHAR* pucPDUData,
                                                 USHORT ucPDULength );

void            vMBMasterCBRequestScuuess( void );

/* -----------------------MASTER Callback for the protocol stack -----------------*/

extern          BOOL( *pxMBMasterFrameCBByteReceived ) ( void );

extern          BOOL( *pxMBMasterFrameCBTransmitterEmpty ) ( void );

extern          BOOL( *pxMBMasterPortCBTimerExpired ) ( void );

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif

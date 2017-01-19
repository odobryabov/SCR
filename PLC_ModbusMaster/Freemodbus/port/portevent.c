/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portevent.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Variables ----------------------------------------*/
static xQueueHandle xQueueHdl;
/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit( void )
{
    if( ( xQueueHdl = xQueueCreate( 1, sizeof( eMBEventType ) ) ) != 0 )
    {
        return TRUE;
    }
    return FALSE;
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    BOOL            bStatus = TRUE;
	
    if( bMBPortIsWithinException(  ) )
    {
        ( void )xQueueSendFromISR( xQueueHdl, ( const void * )&eEvent, pdFALSE );
    }
    else
    {
        ( void )xQueueSend( xQueueHdl, ( const void * )&eEvent, portTICK_RATE_MS * 200 );
    }

    return bStatus;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    BOOL            xEventHappened = FALSE;

    if( xQueueReceive( xQueueHdl, eEvent, portTICK_RATE_MS * 50 ) == pdTRUE )
    {
        xEventHappened = TRUE;
    }
    return xEventHappened;
}

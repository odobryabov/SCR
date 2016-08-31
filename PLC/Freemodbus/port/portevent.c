/*
 * FreeModbus Library: STM32F4xx Port
 * Copyright (C) 2013 Alexey Goncharov <a.k.goncharov@ctrl-v.biz>
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
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Variables ----------------------------------------*/


/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortEventInit( MBPort_InitTypeDef* mbPort )
{
		mbPort->MBEvent.xEventInQueue = FALSE;
    return TRUE;
}

BOOL xMBPortEventPost( MBPort_InitTypeDef* mbPort, eMBEventType eEvent )
{
    mbPort->MBEvent.xEventInQueue = TRUE;
    mbPort->MBEvent.eQueuedEvent = eEvent;
    return TRUE;
}

BOOL xMBPortEventGet( MBPort_InitTypeDef* mbPort, eMBEventType * eEvent )
{
    BOOL xEventHappened = FALSE;

    if ( mbPort->MBEvent.xEventInQueue )
    {
        *eEvent = mbPort->MBEvent.eQueuedEvent;
        mbPort->MBEvent.xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}

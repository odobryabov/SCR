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
 * File: $Id: porttimer.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
/* ----------------------- static functions ---------------------------------*/
static xTimerHandle timer;
static const unsigned portBASE_TYPE timerID;
static void TimerCallBack(xTimerHandle xTimer);
static BaseType_t xHigherPriorityTaskWoken;

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit(USHORT usTim1Timerout50us)
{
	xHigherPriorityTaskWoken = pdFALSE;
	
	timer 	= xTimerCreate	("slaveTimer", 														/* Just a text name, not used by the kernel */
							(50*usTim1Timerout50us)/(1000/portTICK_PERIOD_MS) ,  					/* period */
							pdFALSE, 															/* single circle timer type */
							(void *) &timerID, 													/* timer ID */
							TimerCallBack );													/* callback function */
    return TRUE;
}

void vMBPortTimersEnable( void )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
	xTimerStartFromISR(timer, &xHigherPriorityTaskWoken);
	
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void vMBPortTimersDisable( void )
{
	/* Disable any pending timers. */
	xTimerResetFromISR	( timer, &xHigherPriorityTaskWoken );
	xTimerStopFromISR	( timer, &xHigherPriorityTaskWoken );
	
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

static void TimerCallBack(xTimerHandle xTimer)
{
	vMBPortSetWithinException( TRUE );
	
    (void) pxMBPortCBTimerExpired();
	
	vMBPortSetWithinException( FALSE );
}

#endif

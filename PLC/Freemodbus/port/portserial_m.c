/*
 * FreeModbus Libary: STM32 Port
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
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

extern UART_HandleTypeDef huart2;
/* ----------------------- Start implementation -----------------------------*/

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	  if( xRxEnable )
    {
		/* Enable the UART Data Register not empty Interrupt */
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
    }
    else
    {
		/* Disable the UART Data Register not empty Interrupt */
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
    }
    if( xTxEnable )
    {
		/*	Enable DE transmit signal	*/
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		/* Enable the UART Transmit data register empty Interrupt */
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
    }
    else
    {
			/* Disable the UART Transmit data register empty Interrupt */
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
		/* Enable the UART Transmit Complete Interrupt */    
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_TC);
    }
}

BOOL xMBMasterPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
		eMBParity eParity)
{
  return TRUE;
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
	UART_HandleTypeDef *huart;
	
	huart = &huart2;
	
	huart->Instance->DR = (uint8_t)ucByte;
    return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{
	UART_HandleTypeDef *huart;
	
	huart = &huart2;
	
	*pucByte = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
	
    return TRUE;
}

#endif

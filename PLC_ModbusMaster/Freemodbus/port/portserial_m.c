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
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
extern UART_HandleTypeDef huart2;

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
        eMBParity eParity)
{
    GPIO_InitTypeDef GPIO_InitStruct;
		

	(void) ucPORT;
	/* Peripheral clock enable */
	__USART2_CLK_ENABLE();

	/**USART2 GPIO Configuration
	PA1			------> USART2_DE
	PA2     ------> USART2_TX
	PA3     ------> USART2_RX 
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
	/* Peripheral interrupt init*/
	ENTER_CRITICAL_SECTION();
	{
		HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}	
	EXIT_CRITICAL_SECTION();
	/* USER CODE BEGIN USART2_MspInit 1 */

	/* USER CODE END USART2_MspInit 1 */


	if( ucDataBits == 9 )
		huart2.Init.WordLength = UART_WORDLENGTH_9B;
	else
		huart2.Init.WordLength = UART_WORDLENGTH_8B;

	switch( eParity )
	{
	case MB_PAR_NONE:
		huart2.Init.Parity = UART_PARITY_NONE;
		break;
	case MB_PAR_ODD:
		huart2.Init.Parity = UART_PARITY_ODD;
		break;
	case MB_PAR_EVEN:
		huart2.Init.Parity = UART_PARITY_EVEN;
		break;
	default:
		huart2.Init.Parity = UART_PARITY_NONE;
		break;
	};

	huart2.Init.BaudRate = (uint32_t) ulBaudRate;

	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart2);

	return TRUE;
}

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

void vMBMasterPortClose(void)
{
    __USART2_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
    huart2.Instance->DR = (uint8_t)ucByte;
	
  return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{
    *pucByte = (uint8_t)(huart2.Instance->DR & (uint8_t)0x00FF);
	
  return TRUE;
}

#endif

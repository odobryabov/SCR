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

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

void vMBPortSerialEnable(  MBPort_InitTypeDef* mbPort, BOOL xRxEnable, BOOL xTxEnable )
{
        if( xRxEnable )
    {
		/* Enable the UART Data Register not empty Interrupt */
		__HAL_UART_ENABLE_IT(&mbPort->UARTHandler, UART_IT_RXNE);
    }
    else
    {
		/* Disable the UART Data Register not empty Interrupt */
		__HAL_UART_DISABLE_IT(&mbPort->UARTHandler, UART_IT_RXNE);
    }
    if( xTxEnable )
    {
		/*	Enable DE transmit signal	*/
		vMBDeWritePin (mbPort, GPIO_PIN_SET);
		/* Enable the UART Transmit data register empty Interrupt */
		__HAL_UART_ENABLE_IT(&mbPort->UARTHandler, UART_IT_TXE);
    }
    else
    {
		/* Disable the UART Transmit data register empty Interrupt */
		__HAL_UART_DISABLE_IT(&mbPort->UARTHandler, UART_IT_TXE);
		/* Enable the UART Transmit Complete Interrupt */    
		__HAL_UART_ENABLE_IT(&mbPort->UARTHandler, UART_IT_TC);
    }
}

void vMBDeWritePin(MBPort_InitTypeDef* mbPort, GPIO_PinState PinState)
{
	if (mbPort->UARTHandler.Instance == USART2)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, PinState);
	if (mbPort->UARTHandler.Instance == UART5)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, PinState);
}

BOOL xMBPortSerialInit( MBPort_InitTypeDef* mbPort, UCHAR ucPORT, ULONG ulBaudRate,
                        UCHAR ucDataBits, eMBParity eParity )
{
  GPIO_InitTypeDef GPIO_InitStruct;
	
	(void) ucPORT;
	
	if(mbPort->UARTHandler.Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __USART2_CLK_ENABLE();
  
    /**USART2 GPIO Configuration
		PA1			------> USART2_DE
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
		GPIO_InitStruct.Pin = GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
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
		
    HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
		
		EXIT_CRITICAL_SECTION();
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  } else if(mbPort->UARTHandler.Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspInit 0 */

  /* USER CODE END UART5_MspInit 0 */
    /* Peripheral clock enable */
    __UART5_CLK_ENABLE();
  
    /**UART5 GPIO Configuration
		PB5			------> UART5_DE    
    PC12    ------> UART5_TX
    PD2     ------> UART5_RX 
    */
		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		 /* Peripheral interrupt init*/
		ENTER_CRITICAL_SECTION();
		
    HAL_NVIC_SetPriority(UART5_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(UART5_IRQn);
		
		EXIT_CRITICAL_SECTION();
  /* USER CODE BEGIN UART5_MspInit 1 */

  /* USER CODE END UART5_MspInit 1 */
  }

	if( ucDataBits == 9 )
		mbPort->UARTHandler.Init.WordLength = UART_WORDLENGTH_9B;
  else
		mbPort->UARTHandler.Init.WordLength = UART_WORDLENGTH_8B;
	
	switch( eParity )
  {
    case MB_PAR_NONE:
        mbPort->UARTHandler.Init.Parity = UART_PARITY_NONE;
        break;
    case MB_PAR_ODD:
        mbPort->UARTHandler.Init.Parity = UART_PARITY_ODD;
        break;
    case MB_PAR_EVEN:
        mbPort->UARTHandler.Init.Parity = UART_PARITY_EVEN;
        break;
    default:
        mbPort->UARTHandler.Init.Parity = UART_PARITY_NONE;
        break;
  };
	
  mbPort->UARTHandler.Init.BaudRate = (uint32_t) ulBaudRate;
  
  mbPort->UARTHandler.Init.StopBits = UART_STOPBITS_1;
  mbPort->UARTHandler.Init.Mode = UART_MODE_TX_RX;
  mbPort->UARTHandler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  mbPort->UARTHandler.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&mbPort->UARTHandler);
	
  return TRUE;
}

BOOL xMBPortSerialPutByte( UART_HandleTypeDef* huart, CHAR ucByte )
{
	huart->Instance->DR = (uint8_t)ucByte;
	
  return TRUE;
}

BOOL xMBPortSerialGetByte( UART_HandleTypeDef* huart, CHAR * pucByte )
{
	*pucByte = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
	
  return TRUE;
}

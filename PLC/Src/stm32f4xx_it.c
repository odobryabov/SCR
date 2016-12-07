/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "cmsis_os.h"
#include "mb.h"
#include "pfc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern void xPortSysTickHandler(void);
extern ADC_HandleTypeDef hadc2;
extern PFC_TypeDef PFC;
/* Global variables --------------------------------------------------------*/
MBPort_InitTypeDef mbPortHMI, mbPortExtern;	
//ADC_HandleTypeDef hadc2;

//TIM_HandleTypeDef htim10;
//TIM_HandleTypeDef htim11;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  osSystickHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles TIM2 global interrupt.
*/
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */
	(void) pxMBPortCBTimerExpired(&mbPortHMI);
  /* USER CODE END TIM2_IRQn 0 */

  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
* @brief This function handles TIM3 global interrupt.
*/
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */
	(void) pxMBPortCBTimerExpired(&mbPortExtern);
  /* USER CODE END TIM3_IRQn 0 */
  
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
* @brief This function handles USART2 global interrupt. HMI modbus
*/
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	/* UART in mode Receiver */
	if(__HAL_UART_GET_IT_SOURCE(&mbPortHMI.UARTHandler, UART_IT_RXNE)!= RESET) 
	{
		{
			pxMBFrameCBByteReceived(&mbPortHMI);
		}
	}
	
	/* UART in mode Transmitter */
	if(__HAL_UART_GET_IT_SOURCE(&mbPortHMI.UARTHandler, UART_IT_TXE)!= RESET) 
	{
    pxMBFrameCBTransmitterEmpty(&mbPortHMI);
	}
	
	/* UART in mode Transmitter end */ 
	if(__HAL_UART_GET_IT_SOURCE(&mbPortHMI.UARTHandler, UART_IT_TC)!= RESET) 
	{
		/*	Enable DE receive signal	*/
		vMBDeWritePin(&mbPortHMI, GPIO_PIN_RESET);
		/* Disable the UART Transmit Complete Interrupt */
		__HAL_UART_DISABLE_IT(&mbPortHMI.UARTHandler, UART_IT_TC);
	}
  /* USER CODE END USART2_IRQn 0 */

  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
* @brief This function handles UART5 global interrupt. Extern modbus
*/
void UART5_IRQHandler(void)
{
	/* UART in mode Receiver */
	if(__HAL_UART_GET_IT_SOURCE(&mbPortExtern.UARTHandler, UART_IT_RXNE)!= RESET) 
	{
		{
			pxMBFrameCBByteReceived(&mbPortExtern);
		}
	}
	
	/* UART in mode Transmitter */
	if(__HAL_UART_GET_IT_SOURCE(&mbPortExtern.UARTHandler, UART_IT_TXE)!= RESET) 
	{
    pxMBFrameCBTransmitterEmpty(&mbPortExtern);
	}
	
	/* UART in mode Transmitter end */ 
	if(__HAL_UART_GET_IT_SOURCE(&mbPortExtern.UARTHandler, UART_IT_TC)!= RESET) 
	{
		/*	Enable DE receive signal	*/
		vMBDeWritePin(&mbPortExtern, GPIO_PIN_RESET);
		/* Disable the UART Transmit Complete Interrupt */
		__HAL_UART_DISABLE_IT(&mbPortExtern.UARTHandler, UART_IT_TC);
	}
}
/* USER CODE BEGIN 1 */
void ADC_IRQHandler(void)
{
	/* set flag if adc value is more than high threshold and reset if less than low threshold */
	static uint8_t 	isMoreThanHTR = RESET;
	const uint32_t 	ADC_MAX = 4095,
					ADC_MIN = 0,
					ADC_HTR_TEMP = 2000,
					ADC_LTR_TEMP = 1000;
	
	if (__HAL_ADC_GET_FLAG(&hadc2, ADC_FLAG_AWD))
	{
		if (isMoreThanHTR != SET)
		{	
			ADC2->HTR = ADC_MAX;
			ADC2->LTR = ADC_LTR_TEMP;
			
			isMoreThanHTR = SET;
			
			__HAL_TIM_SET_AUTORELOAD(&PFC.Timers.TIMSemiPeriod, 1);
			__HAL_TIM_ENABLE(&PFC.Timers.TIMSemiPeriod);
			
		} else
		{
			ADC2->HTR = ADC_HTR_TEMP;
			ADC2->LTR = ADC_MIN;
			
			isMoreThanHTR = RESET;
		}
		__HAL_ADC_CLEAR_FLAG(&hadc2, ADC_FLAG_AWD);
	}
}

/**
* @brief This function handles TIM13 global interrupt.
*/
void TIM8_UP_TIM13_IRQHandler(void)
{
	/* disable timer */
	__HAL_TIM_DISABLE(&PFC.Timers.TIMSemiPeriod);
	__HAL_TIM_CLEAR_IT(&PFC.Timers.TIMSemiPeriod,TIM_IT_UPDATE);
	/* open thyristor */
    PFCOpenGate(&PFC.SCR[0]);
}

/**
* @brief This function handles TIM14 global interrupt.
*/
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	__HAL_TIM_CLEAR_IT(&PFC.Timers.TIMOpenGate,TIM_IT_UPDATE);
	PFC.SCR[0].Mode++;
	PFCOpenGate(&PFC.SCR[0]);
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

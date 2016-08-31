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


/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "stm32f4xx_hal_conf.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( MBPort_InitTypeDef* mbPort, USHORT usTim1Timerout50us )
{
	if(mbPort->TIMHandler.Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* Peripheral clock enable */
    __TIM2_CLK_ENABLE();
  /* Peripheral interrupt init*/
		ENTER_CRITICAL_SECTION();
		
    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
		
		EXIT_CRITICAL_SECTION();
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
  else if(mbPort->TIMHandler.Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* Peripheral clock enable */
    __TIM3_CLK_ENABLE();
  /* Peripheral interrupt init*/
		ENTER_CRITICAL_SECTION();
		
    HAL_NVIC_SetPriority(TIM3_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
		
		EXIT_CRITICAL_SECTION();
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
	
		mbPort->TIMHandler.Init.Prescaler = 4200 - 1;
		mbPort->TIMHandler.Init.CounterMode = TIM_COUNTERMODE_UP;
		mbPort->TIMHandler.Init.Period = ( (uint32_t) usTim1Timerout50us );
		mbPort->TIMHandler.Init.ClockDivision = 0;
		HAL_TIM_Base_Init(&mbPort->TIMHandler);
	
		__HAL_TIM_CLEAR_IT(&mbPort->TIMHandler,TIM_IT_UPDATE);
		__HAL_TIM_ENABLE_IT(&mbPort->TIMHandler,TIM_IT_UPDATE);
	
    return TRUE;
}

void vMBPortTimersEnable(TIM_HandleTypeDef *htim)
{
	    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
		__HAL_TIM_CLEAR_IT(htim,TIM_IT_UPDATE);
		__HAL_TIM_ENABLE_IT(htim,TIM_IT_UPDATE);
		__HAL_TIM_SetCounter(htim,0);
		__HAL_TIM_ENABLE(htim);
}


void vMBPortTimersDisable(TIM_HandleTypeDef *htim)
{
		/* Disable any pending timers. */
		__HAL_TIM_DISABLE(htim);
		__HAL_TIM_SetCounter(htim,0);
		__HAL_TIM_DISABLE_IT(htim,TIM_IT_UPDATE);
		__HAL_TIM_CLEAR_IT(htim,TIM_IT_UPDATE);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	extern MBPort_InitTypeDef mbPortHMI, mbPortExtern;	
	if (htim->Instance == TIM2)
		pxMBPortCBTimerExpired(&mbPortHMI);
	else 
		pxMBPortCBTimerExpired(&mbPortExtern);
	//counter_val = __HAL_TIM_GET_COUNTER(&TimHandle);
	//printf("Counter at interrupt: %d\n", counter_val);
	//pxMBPortCBTimerExpired(htim);
}

#endif

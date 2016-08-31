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
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/
static USHORT usT35TimeOut50us;
static USHORT usPrescalerValue = 0;

extern TIM_HandleTypeDef htim2;

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortTimersInit(USHORT usTimeOut50us)
{
	return TRUE;
}

void vMBMasterPortTimersT35Enable(void)
{
	vMBMasterSetCurTimerMode(MB_TMODE_T35);
	
	htim2.Instance = TIM2;
  htim2.Init.Prescaler = usPrescalerValue;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = (uint16_t) usT35TimeOut50us;
  htim2.Init.ClockDivision = 0;
  HAL_TIM_Base_Init(&htim2);

	__HAL_TIM_CLEAR_IT(&htim2,TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);
	__HAL_TIM_SetCounter(&htim2,0);
	__HAL_TIM_ENABLE(&htim2);
}

void vMBMasterPortTimersConvertDelayEnable(void)
{
	/* Set current timer mode,don't change it.*/
	vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);
	
	htim2.Instance = TIM2;
  htim2.Init.Prescaler = usPrescalerValue;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = (uint16_t)(MB_MASTER_DELAY_MS_CONVERT * 1000 / 50);
  htim2.Init.ClockDivision = 0;
  HAL_TIM_Base_Init(&htim2);

	__HAL_TIM_CLEAR_IT(&htim2,TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);
	__HAL_TIM_SetCounter(&htim2,0);
	__HAL_TIM_ENABLE(&htim2);
}

void vMBMasterPortTimersRespondTimeoutEnable(void)
{
	/* Set current timer mode,don't change it.*/
	vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);

	htim2.Instance = TIM2;
  htim2.Init.Prescaler = usPrescalerValue;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = (uint16_t)(MB_MASTER_TIMEOUT_MS_RESPOND * 1000 / 50);
  htim2.Init.ClockDivision = 0;
  HAL_TIM_Base_Init(&htim2);

	__HAL_TIM_CLEAR_IT(&htim2,TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);
	__HAL_TIM_SetCounter(&htim2,0);
	__HAL_TIM_ENABLE(&htim2);
}

void vMBMasterPortTimersDisable(void)
{
		/* Disable any pending timers. */
		__HAL_TIM_DISABLE(&htim2);
		__HAL_TIM_SetCounter(&htim2,0);
		__HAL_TIM_DISABLE_IT(&htim2,TIM_IT_UPDATE);
		__HAL_TIM_CLEAR_IT(&htim2,TIM_IT_UPDATE);
}

#endif

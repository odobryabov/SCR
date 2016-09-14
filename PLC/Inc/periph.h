/**
  ******************************************************************************
  * @file    defines.h
  * @author  tech department
  * @version V0.1
  * @date    22-07-2016
  * @brief   Header file of user functions
	* @verbatim
	*
	*
	* @endverbatim
  * COPYRIGHT(c) 2016 
	******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PERIPH_H
#define PERIPH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_TIM1_Init(void);
void MX_TIM8_Init(void);
void MX_ADC1_Init(void);

void MX_ADC2_Init(void);
void MX_TIM10_Init(void);
void MX_TIM11_Init(void);
void MX_TIM13_Init(void);
void MX_TIM14_Init(void);


#endif
/************************ (C) COPYRIGHT *****END OF FILE****/

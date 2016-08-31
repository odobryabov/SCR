/**
  ******************************************************************************
  * @file    	pfc.c
  * @author  	tech department
  * @version 	0.1
  * @date    	16-08-2016
  * @brief   	Code for phase-fired controller functions
	* @verbatim
	*
	*
	* @endverbatim
  * COPYRIGHT(c) 2016
	******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "pfc.h"
#include <math.h>

/* Global variables */
PFC_TypeDef PFC;
SCRPhase_TypeDef SCRActive;
/* External variables */

/******************************************************************************/
/*            			Phase-fired controller functions         									*/ 
/******************************************************************************/

/** 
	* @brief 	Thyristor handles init
	* @retval	None
 */
void PFCSCRInit(void)
{
	/* start with the first SCR */
	SCRActive = L1_Plus;
	
	/* thyristor 1 */
	PFC.SCR[0].Phase = L1_Plus;
	PFC.SCR[0].GPIOx = GPIOE;
	PFC.SCR[0].GPIO_Pin = GPIO_Pin_0;

	/* thyristor 2 */
	PFC.SCR[1].Phase = L1_Minus;
	PFC.SCR[1].GPIOx = GPIOE;
	PFC.SCR[1].GPIO_Pin = GPIO_Pin_1;

	/* thyristor 3 */
	PFC.SCR[2].Phase = L2_Plus;
	PFC.SCR[2].GPIOx = GPIOE;
	PFC.SCR[2].GPIO_Pin = GPIO_Pin_2;

	/* thyristor 4 */
	PFC.SCR[3].Phase = L2_Minus;
	PFC.SCR[3].GPIOx = GPIOE;
	PFC.SCR[3].GPIO_Pin = GPIO_Pin_3;

	/* thyristor 5 */
	PFC.SCR[4].Phase = L3_Plus;
	PFC.SCR[4].GPIOx = GPIOE;
	PFC.SCR[4].GPIO_Pin = GPIO_Pin_4;

	/* thyristor 6 */
	PFC.SCR[5].Phase = L3_Minus;
	PFC.SCR[5].GPIOx = GPIOE;
	PFC.SCR[5].GPIO_Pin = GPIO_Pin_5;
        
        /* gpio and mode init */
        for (uint8_t i = 0; i < 6; i++)
        {
          PFC.SCR[i].Mode = FirstImp;
          GPIO_Init( PFC.SCR[i].GPIOx, PFC.SCR[i].GPIO_Pin, GPIO_Mode_Out_PP_High_Fast);
        }
}

/** 
	* @brief  Timers init
        * @param: TIM1: switch, TIM2: counter, TIM3: gate
	* @retval	None
 */
void PFCTimersInit(void)
{
	/* unlock impulse periods init common structure */
	PFC.Timers.FirstImpPeriod       = 20000;
	PFC.Timers.NextImpPeriod        = 2000;
	PFC.Timers.NextImpNumber 	= 4;
	PFC.Timers.SpacePeriod 		= 20000;
	
	/* Peripheral clock enable */
        CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
        CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
        CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
	
	//ENTER_CRITICAL_SECTION
        /* Peripheral interrupt init*/
//        TIM1_UpdateRequestConfig(TIM1_UpdateSource_Global);
//        TIM1_InternalClockConfig();
//        TIM2_UpdateRequestConfig(TIM2_UpdateSource_Global);
//        TIM2_InternalClockConfig();
//        TIM3_UpdateRequestConfig(TIM3_UpdateSource_Global);
//        TIM3_InternalClockConfig();
   
	//EXIT_CRITICAL_SECTION
        
	/* configuration */
        TIM1_TimeBaseInit((uint16_t)6-1,
                       TIM1_CounterMode_Down,
                       1000,
                       TRUE);
	
        TIM2_TimeBaseInit(TIM2_Prescaler_1,
                               TIM2_CounterMode_Up, 
                               1);
        
        TIM3_TimeBaseInit(TIM3_Prescaler_128,
                               TIM3_CounterMode_Down, 
                               1);
        
	TIM1_ClearITPendingBit(TIM1_IT_Update);
        TIM1_ITConfig(TIM1_IT_Update, ENABLE);
        
        TIM2_ClearITPendingBit(TIM2_IT_Update);
        TIM2_ITConfig(TIM2_IT_Update, ENABLE);
        
        TIM3_ClearITPendingBit(TIM3_IT_Update);
        TIM3_ITConfig(TIM3_IT_Update, ENABLE);
        	
	//PFC.Timers.phaseSwitchTimer->Instance->CCMR2 = 60;  /////////////default 60 degreeses
}

/**
	* @brief  ADC init
	* @retval None
 */
void PFCADCInit(void)
{
  /* Enable ADC clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
  
  /* de-initialize ADC */
  ADC_DeInit(ADC1);
	
/*  ADC configured as follow:
  - NbrOfChannel = 1 - ADC_Channel_1
  - Mode = Single ConversionMode(ContinuousConvMode disabled)
  - Resolution = 12Bit
  - Prescaler = /1
  - sampling time 9 */
  ADC_Init(ADC1, ADC_ConversionMode_Single,ADC_Resolution_12Bit, ADC_Prescaler_1);
  ADC_SamplingTimeConfig(ADC1, ADC_Group_FastChannels, ADC_SamplingTime_9Cycles);
  /* disable SchmittTrigger for ADC_Channel_1, to save power */
  ADC_SchmittTriggerConfig(ADC1, ADC_Channel_1, DISABLE);
 /* disable DMA for ADC1 */
  ADC_DMACmd(ADC1, DISABLE);
  
  /* Enable ADC1 hannel 1 */
  ADC_Cmd(ADC1, ENABLE);
  ADC_ChannelCmd(ADC1, ADC_Channel_1, ENABLE);
  
  /* a short time of delay is required after enable ADC */
  delay_10us(3);
}

/**
	* @brief  Get ADC value
	* @retval	conversion value
 */
uint16_t PFCGetADC(void)
{
  /* start ADC convertion by software */
    ADC_SoftwareStartConv(ADC1);
  /* wait until end-of-covertion */
    while( ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0 );
  /* read ADC convertion result */
    return ADC_GetConversionValue(ADC1);
}

/**
	* @brief  Filter for voltage
	* @param	voltageValue: real voltage value
	* @retval	Filtered value
 */
uint16_t PFCVoltageFilter(uint16_t voltageValue)
{
	uint16_t result;
	
	result = voltageValue;
	
        return result;
	//return trpFilter(voltageValue);
}

/** 
	* @brief 	start point definition
	* @param	voltageValue: voltage value
	* @retval	period
 */
FlagStatus PFCStartPhase(uint16_t voltageValue)
{
        const uint16_t edge = 3000;		                /* low edge */
	static uint16_t arrayVoltage[2];                        /* temp voltage array */
        FlagStatus status = RESET;                              /* return value */
        
        /* push a current value */
	arrayVoltage[1] = voltageValue;
        
        /* if the current value is greater than edge AND a previous value is less 
         * then this is a start point
         *                       /
         *                     *  <--arrayVoltage[1] NOW
         *                   /
         *    edge --------------------------
         *                 /
         *                * <--arrayVoltage[0] BEFORE
         *              /
        */
	if (arrayVoltage[1] > edge && arrayVoltage[0] <= edge)
          status = SET;
	else
          status = RESET;

        /* current value becomes a previous */
        arrayVoltage[0] = arrayVoltage[1];
        
        return status;
}

 /** 
	* @brief 	open a thyristor gate
	* @param	Thyristor: pointer to thyristor handle 
	* @retval	None
	*          ______________                     __                     __                     __                         
	* ________|              |___________________|  |___________________|  |________//_________|  |_____
	*             FirstImp           Space      2ndImp     Space       3rdImp     Space        n-Imp      
 */
void PFCOpenGate(SCR_TypeDef* Thyristor)
{
	/* local static variables */
	static uint16_t i = 0; /* counter for other impulses */
	
	/* modes switcher */
	switch (Thyristor->Mode)
	{
		/* first impulse */
		case FirstImp:
			/* open gate */
			GPIO_WriteBit(Thyristor->GPIOx, Thyristor->GPIO_Pin, SET);
			/* set period */
			TIM3_SetCounter(PFC.Timers.FirstImpPeriod);
			/* enable timer */
                        TIM3_Cmd(ENABLE);
			break;
		
		/* space between impulses */
		case Space:
			/* close gate */
			GPIO_WriteBit(Thyristor->GPIOx, Thyristor->GPIO_Pin, RESET);
			/* if it has other impulses then... Increase i after */
			if (i++ < PFC.Timers.NextImpNumber)
			{
				/* set period */
                                TIM3_SetCounter(PFC.Timers.SpacePeriod);
			} else
			{
				/* default settings */
				i = 0;
				Thyristor->Mode = FirstImp;
                                TIM3_Cmd(DISABLE);
			}
			break;
			
		/* next impulse */
		case NextImp:
			/* open gate */
			GPIO_WriteBit(Thyristor->GPIOx, Thyristor->GPIO_Pin, SET);
			/* shift mode to FirstImp to goto Space mode after delay */
			Thyristor->Mode = FirstImp;
			/* set period */
			TIM3_SetCounter(PFC.Timers.NextImpPeriod); 
			break;
	}
}

/** 
	* @brief  trapezoid area average. 
	* @brief  First return values (windows numder) are incorrect 
	* 				while temp array is not full of input values
	* @param  input: input value.
	* @param  temp: extern buffer array (window + 1 is size)
	* @param  window: smooth width
	* @retval output value
 */
uint16_t trpFilter(uint16_t input)
{
        static uint16_t temp[3];
	/* private variables */
        uint16_t window = 3;
	float sqSum = 0; /* area sum */
	
	/* place back an input value */
	temp[window] = input;
	/* push back a temp array */
	for (uint16_t i = 0; i < window; i++)
		temp[i] = temp[i+1];
	
	/* area sum calculating. */
	/* trapezoid area equals (|a - b|*h)/2 + b*h. h = 1 */
	for (uint16_t i = 0; i < window; i++)
		sqSum += fabs((float)temp[i+1] - (float)temp[i]) / 2 + temp[i];
	
	/* return an average square */
	return (uint16_t)(sqSum / window);
}

/** 
	* @brief 	main function. TIM1: switch, TIM2: counter, TIM3: gate
	* @retval	None
 */
void PFC_main(void)
{
        uint16_t openDelay = 1000; /* default delay */
	PFCSCRInit();
	PFCTimersInit();
	if (PFCStartPhase(PFCVoltageFilter(PFCGetADC())))
	{
          TIM1_ClearITPendingBit(TIM1_IT_Update);
          TIM1_SetCounter(TIM2_GetCounter()+ openDelay);
          TIM1_Cmd(ENABLE);
                
          TIM2_ClearITPendingBit(TIM2_IT_Update);
          TIM2_SetCounter(0);
          TIM2_Cmd(ENABLE);
	}
}
/************************ (C) COPYRIGHT ***** END OF FILE ****/

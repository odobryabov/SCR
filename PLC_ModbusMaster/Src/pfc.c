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
#include "pfc.h"

/* Global variables */
PFC_TypeDef PFC;		//Global PFC handler


/* External variables */

/******************************************************************************/
/*            			Phase-fired controller functions         			  */ 
/******************************************************************************/

/** 
	* @brief 	Thyristor handles init
	* @retval	None
 */
void PFCInit(void)
{
	/* unlock impulse periods init common structure */
	PFC.Timers.FirstImpPeriod   = 10000;
	PFC.Timers.NextImpPeriod    = 1000;
	PFC.Timers.NextImpNumber 	= 3;
	PFC.Timers.SpacePeriod 		= 500;
	
	/* thyristor 1 */
	PFC.SCR[0].Phase = L1_Plus;
	PFC.SCR[0].GPIOx = GPIOE;
	PFC.SCR[0].GPIO_Pin = GPIO_PIN_0;

	/* thyristor 2 */
	PFC.SCR[1].Phase = L1_Minus;
	PFC.SCR[1].GPIOx = GPIOE;
	PFC.SCR[1].GPIO_Pin = GPIO_PIN_1;

	/* thyristor 3 */
	PFC.SCR[2].Phase = L2_Plus;
	PFC.SCR[2].GPIOx = GPIOE;
	PFC.SCR[2].GPIO_Pin = GPIO_PIN_2;

	/* thyristor 4 */
	PFC.SCR[3].Phase = L2_Minus;
	PFC.SCR[3].GPIOx = GPIOE;
	PFC.SCR[3].GPIO_Pin = GPIO_PIN_3;

	/* thyristor 5 */
	PFC.SCR[4].Phase = L3_Plus;
	PFC.SCR[4].GPIOx = GPIOE;
	PFC.SCR[4].GPIO_Pin = GPIO_PIN_4;

	/* thyristor 6 */
	PFC.SCR[5].Phase = L3_Minus;
	PFC.SCR[5].GPIOx = GPIOE;
	PFC.SCR[5].GPIO_Pin = GPIO_PIN_5;
        
	/* gpio and mode init */
	for (uint8_t i = 0; i < 6; i++)
	{
		/* temp gpio handler */
		GPIO_InitTypeDef GPIO_temp;
		
		PFC.SCR[i].Mode = FirstImp;
		
		GPIO_temp.Pin = PFC.SCR[i].GPIO_Pin;
		GPIO_temp.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_temp.Pull = GPIO_NOPULL;
		GPIO_temp.Speed = GPIO_SPEED_HIGH;
		HAL_GPIO_Init(PFC.SCR[i].GPIOx, &GPIO_temp);
	}
}

/** 
	* @brief 	start point definition
	* @param	voltageValue: voltage value
    * @param    edgeHigh: high edge
	* @param    edgeLow: low edge
	* @retval	set at start moment
 */
FlagStatus PFCStartPhase(uint32_t voltageValue, uint32_t edgeHigh, uint32_t edgeLow)
{
		/* flad is set if voltage is more than edgeHigh, and it is reset when voltage less than edgeLow */
        static FlagStatus flag = RESET;

        if (voltageValue < edgeLow)
           return flag = RESET;
          
        if ((voltageValue > edgeHigh) & !flag)
        {
		  /* interruption */
          flag = SET;
          return SET;
        }
        else
          return RESET;
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
	static uint8_t i = 0; /* counter for next impulses */
	
	/* modes switcher */
	switch (Thyristor->Mode)
	{
		/* first impulse */
		case FirstImp:
			/* open gate */
			HAL_GPIO_WritePin(Thyristor->GPIOx, Thyristor->GPIO_Pin, GPIO_PIN_SET);
			/* set period */
			__HAL_TIM_SET_AUTORELOAD(&PFC.Timers.TIMOpenGate, PFC.Timers.FirstImpPeriod) ;
			/* enable timer */
            __HAL_TIM_ENABLE(&PFC.Timers.TIMOpenGate);
			break;
		
		/* space between impulses */
		case Space:
			/* close gate */
			HAL_GPIO_WritePin(Thyristor->GPIOx, Thyristor->GPIO_Pin, GPIO_PIN_RESET);
			/* if it has other impulses then... Increase i after */
			if (i++ < PFC.Timers.NextImpNumber)
			{
				/* set period */
				__HAL_TIM_SET_AUTORELOAD(&PFC.Timers.TIMOpenGate, PFC.Timers.SpacePeriod) ;
			} else
			{
				/* default settings */
				__HAL_TIM_DISABLE(&PFC.Timers.TIMOpenGate);
				i = 0;
				Thyristor->Mode = FirstImp;
                                
			}
			break;
			
		/* next impulse */
		case NextImp:
			/* open gate */
			HAL_GPIO_WritePin(Thyristor->GPIOx, Thyristor->GPIO_Pin, GPIO_PIN_SET);
			/* shift mode to FirstImp to goto Space mode after delay */
			Thyristor->Mode = FirstImp;
			/* set period */
			__HAL_TIM_SET_AUTORELOAD(&PFC.Timers.TIMOpenGate, PFC.Timers.NextImpPeriod) ;
			break;
	}
}
/************************ (C) COPYRIGHT ***** END OF FILE ****/

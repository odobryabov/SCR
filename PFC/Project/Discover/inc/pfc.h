/**
  ******************************************************************************
  * @file    	pfc.h
  * @author  	tech department
  * @version 	0.1
  * @date    	16-08-2016
  * @brief   	Header file for phase-fired controller functions
	* @verbatim
	*
	*
	* @endverbatim
  * COPYRIGHT(c) 2016
	******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PFC_H
#define PFC_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x_tim1.h"
#include "stm8l15x_tim2.h"
#include "stm8l15x_tim3.h"
#include "stm8l15x_dac.h"

/* Global types --------------------------------------------------------------*/
/** 
  * @brief  Thyristor label structure definition
  */
typedef enum
{
	L1_Plus 	= 0,
	L3_Minus 	= 1,
	L2_Plus 	= 2,
	L1_Minus 	= 3,
	L3_Plus 	= 4,
	L2_Minus 	= 5
} SCRPhase_TypeDef;

typedef enum
{
	FirstImp	= 0,
	Space 		= 1,
	NextImp 	= 2,
} SCRUnlockMode_TypeDef;

/** 
  * @brief  Thyristor init structure definition
  */
typedef struct
{
	SCRPhase_TypeDef Phase;
	SCRUnlockMode_TypeDef Mode;
	GPIO_Pin_TypeDef GPIO_Pin;
	GPIO_TypeDef* GPIOx;
} SCR_TypeDef;

/** 
  * @brief  PFC timers init structure definition
  */
typedef struct
{
	uint16_t FirstImpPeriod;
	uint16_t NextImpPeriod;
	uint16_t SpacePeriod;
	uint8_t NextImpNumber;
} PFCTimers_TypeDef;

/** 
  * @brief  PFC main structure definition
  */
typedef struct
{
	SCR_TypeDef SCR[6];
	PFCTimers_TypeDef Timers;
} PFC_TypeDef;

/* Private function prototypes -----------------------------------------------*/
void PFCSCRInit(void);
void PFCTimersInit(void);
void PFCADCInit (void);
uint16_t PFCGetADC(void);
inline uint16_t PFCVoltageFilter(uint16_t voltageValue);
FlagStatus PFCStartPhase(uint16_t voltageValue, uint16_t edge);
void PFCOpenGate(SCR_TypeDef* Thyristor);
void PFC_main(void);
void PFC_DAC_init(void);

inline void SCROpenGate(SCR_TypeDef Thyristor);

#endif
/************************ (C) COPYRIGHT *****END OF FILE****/

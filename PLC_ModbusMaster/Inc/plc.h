/**
  ******************************************************************************
  * @file    	charger.h
  * @author  	tech department
  * @version 	0.1
  * @date    	22-07-2016
  * @brief   	Header file for charger functions
	* @verbatim
	*
	*
	* @endverbatim
  * COPYRIGHT(c) 2016
	******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PLC_H
#define PLC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "mb.h"
#include "user_mb_app.h"

/* debug macros ----------------------------------------------------------*/
#define DEBUG_MODE 0

#if DEBUG_MODE == 0
	#define debugCode( code_fragment )
#elif DEBUG_MODE > 0
	//printf("4\n");
	#define debugCode( code_fragment) { code_fragment }
#endif

#if DEBUG_MODE != 0
	#include <stdio.h>

	#define ITM_port8(n) 	(*((volatile unsigned char  *)(0xE0000000+4*n)))
	#define ITM_port16(n) 	(*((volatile unsigned short *)(0xE0000000+4*n)))
	#define ITM_port32(n) 	(*((volatile unsigned long  *)(0xE0000000+4*n)))

	#define DEMCR 			(*((volatile unsigned long  *)(0xE000EDFC)))
	#define TRCENA 			0x01000000

	struct __FILE { int handle; };
	FILE __stdout;
	FILE __stdin;

	int fputc(int ch, FILE *f)
	{
		if (DEMCR & TRCENA)
		{
			while (ITM_port32(0) == 0);
			ITM_port8(0) = ch;
		}
		return (ch);
	}
#endif

/* Global types ------------------------------------------------------------*/
/**
  * @brief Charge mode structure definition  
  */ 
typedef enum
{
	CHARGE_MODE_STOP	= 0,				/* stop mode */
	CHARGE_MODE_FLOAT	= 1,				/* floating mode */
	CHARGE_MODE_BOOST	= 2,				/* fast mode */
	CHARGE_MODE_EQ		= 3,				/* equalize mode */
	CHARGE_MODE_TEST	= 4					/* battery test mode */
} ChargeMode_TypeDef;

/* Private defines ------------------------------------------------------------*/
/* flash read-write defines */
#define REGS_NUM					21				/* buffer size for flash rewriting. First registers of modbus holding buffer */

/* ADC filtered variables defines */
#define I_SENSOR_COM				fltrIsensorCom 	/* ADC1ConvertedValues[0]	PB0	Iin	*/
#define T_SENSOR					fltrTsensor 	/* ADC1ConvertedValues[1]	PB1	Tempr */
#define U_SENSOR  					fltrUsensor 	/* ADC1ConvertedValues[2] 	PC4	Uin */
#define I_SENSOR_AB					fltrIsensorAB 	/* ADC1ConvertedValues[3]	PC5	Iinab */

/* current and voltage outputs defines */
#define U_SET						TIM1->CCR1 		/* uSet PA8 Uout */
#define I_SET						TIM8->CCR2 		/* iSet PC7 Iout */

/* discrete outputs defines */
#define ALARM_ON					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, 	GPIO_PIN_SET)			/* AVAR */
#define ALARM_OFF					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, 	GPIO_PIN_RESET)			/* AVAR */
#define READY_ON					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, 	GPIO_PIN_SET)			/* Rdy */
#define READY_OFF					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, 	GPIO_PIN_RESET)			/* Rdy */
#define VENTILATION_ON				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, 	GPIO_PIN_SET)			/* Vent */
#define VENTILATION_OFF				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, 	GPIO_PIN_RESET)			/* Vent */
#define ALARM_LAMP_ON				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 	GPIO_PIN_SET)			/* LedAlarm */
#define ALARM_LAMP_OFF				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 	GPIO_PIN_RESET)			/* LedAlarm */
#define LOW_CHARGE_PROTECT_ON		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 	GPIO_PIN_SET)			/* Zogr */
#define LOW_CHARGE_PROTECT_OFF		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 	GPIO_PIN_RESET)			/* Zogr */
#define RESET_SIFU_ON				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 	GPIO_PIN_SET)			/* Res */
#define RESET_SIFU_OFF				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 	GPIO_PIN_RESET)			/* Res */
#define ALLOW_ON					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 	GPIO_PIN_SET)			/* LedVkl */
#define ALLOW_OFF					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 	GPIO_PIN_RESET)			/* LedVkl */
#define WARNING_ON					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, 	GPIO_PIN_SET)			/* pred */
#define WARNING_OFF					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, 	GPIO_PIN_RESET)			/* pred */

/* discrete inputs defines */
#define COM_SWITCH_ON				!HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0)							/* SA1 */
#define POWER_SWITCH_ON				!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0)							/* QF1 */
#define UZP_READY					!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)							/* QF2 */
#define LOAD_SWITCH_ON				!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1)							/* QF9 */
#define THYRISTOR_OVERHEAD			 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)							/* Tyr */
#define TRANSFORMER_OVERHEAD		 HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)							/* Trans */
#define VENTILATION_CTRL			!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)							/* Izol */
#define SUPPLY_220					!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)							/* P24V */
#define I_LIMIT						!HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10)							/* Iogr */
#define I_MAX						!HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9)							/* Ipmax */
#define SUPPLY_24					!HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8)							/* Power */ 
#define OVERLOAD					!HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_7)							/* Iper */
#define PHASE_BREAK					!HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11)							/* Phase */
#define POLARITY					!HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12)							/* Polarity */
#define TEST_BUTTON					!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)							/* NULL */

/* modbus map */
#define TmaxAB 						usSRegHoldBuf[0]			/*  */
#define	TminAB 						usSRegHoldBuf[1]			/*  */
#define	ABnum 						usSRegHoldBuf[2]			/* batteries number */
#define	Umax 						usSRegHoldBuf[3]			/* maximum charge voltage */
#define	Umin 						usSRegHoldBuf[4]			/* minimum charge voltage */
#define	Uzogr 						usSRegHoldBuf[5]			/* charge voltage limit */
#define	Uf 							usSRegHoldBuf[6]			/*  */
#define	Ub 							usSRegHoldBuf[7]			/*  */
#define	Ue 							usSRegHoldBuf[8]			/*  */
#define	KizU 						usSRegHoldBuf[9]			/*  */
#define	Iab 						usSRegHoldBuf[10]			/*  */
#define	Imax 						usSRegHoldBuf[11]			/* maximum current surplus */
#define	KizIab 						usSRegHoldBuf[12]			/*  */
#define	KizIt 						usSRegHoldBuf[13]			/*  */
#define	T1 							usSRegHoldBuf[14]			/*  */
#define	T2 							usSRegHoldBuf[15]			/*  */
#define	Tcorr 						usSRegHoldBuf[16]			/*  */
#define	maxcorr 					usSRegHoldBuf[17]			/* max long line offset */
#define	LineCorr 					usSRegHoldBuf[18]			/* long line offset */
#define	KU 							usSRegHoldBuf[19]			/* voltage ratio */
#define	KI 							usSRegHoldBuf[20]			/* current ratio */
#define	TestABUmin					usSRegHoldBuf[21]			/*  */
#define	TestABC 					usSRegHoldBuf[22]			/*  */
#define	TestABT 					usSRegHoldBuf[23]			/*  */
#define TestT 						usSRegHoldBuf[24]			/*  */
#define	Rzd 						usSRegHoldBuf[25]			/* charge mode: 0 - stop; 1 - sustain; 2 - rapid; 3 - equalize; 4 - battery test */
#define	TimeB 						usSRegHoldBuf[26]			/*  */
#define	TimeE 						usSRegHoldBuf[27]			/*  */
#define	SerV 						usSRegHoldBuf[28]			/*  */
//#define							usSRegHoldBuf[29]			/*  */
#define	Reset 						usSRegHoldBuf[30]			/*  */
//#define							usSRegHoldBuf[31] 			/* U_SENSOR * KizU */
//#define							usSRegHoldBuf[32] 			/* I_SENSOR_AB * KizIab */
//#define							usSRegHoldBuf[33] 			/* I_SENSOR_COM * KizIt */
//#define							usSRegHoldBuf[34] 			/* fabs((float)tpm) */
#define	Al1 						usSRegHoldBuf[37]			/* alarm word 1 */
#define	Al3 						usSRegHoldBuf[38]			/* alarm word 3 */

/* Private function prototypes -----------------------------------------------*/
void setDefaultValues (void);
void packAlarmsWarnings (void);
void temperatureAdd (void);
void Temperature (void);
void stopMode(void);
void availableMode(void);
void startWorking (void);
void offContactor(void);
void autostopEqMode (void);
void autostopEqModeTimerCallBack(xTimerHandle xTimer);
void chargeModeAutomation (void);
void changeChargeModeTimerCallBack (xTimerHandle xTimer);
void ventilationTimerCallBack (xTimerHandle xTimer);
void stopVentilation(void);
void runningLED(void);
void overLoad(void);
void commonStop(void);
void voltageStop(void);
void resetSIFU(void);
void unchargeProtect(void);
void voltageChargeCalc(void);
void voltageChargeCalcTimerCallBack(xTimerHandle xTimer);
void currentChargeCalc(void);
void voltageDefCalc(void);
void ventilationAlarm(void);

#endif
/************************ (C) COPYRIGHT *****END OF FILE****/

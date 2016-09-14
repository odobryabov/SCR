/**
  ******************************************************************************
  * @file    	charger.c
  * @author  	tech department
  * @version 	0.1
  * @date    	22-07-2016
  * @brief   	Code for charger functions
	* @verbatim
	*
	*
	* @endverbatim
  * COPYRIGHT(c) 2016
	******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "charger.h"
#include "otherfunct.h"
#include <math.h>

/* Global variables */
uint16_t Al2;									/* alarm word 2 */
float 	 DeTemp;								/* smth temperature variable */
uint16_t Imper;									/* overload flag */
uint16_t KizT;									/* smth temperature ratio */
uint16_t stop;									/* stop flag */
int16_t	 tpm;									/* AB temperature */
uint16_t UeZ;									/* contactor 1 flag */
float 	 Urout;									/* voltage precalculation variable */
uint16_t UsZ;									/* contactor 2 flag */
float 	 Uzd;									/* voltage precalculation variable */
uint16_t VentAv;								/* ventilation alarm flag */
uint16_t Zogr;									/* deep uncharge flag */

/* External variables */
/* modbus use these variables */
#if S_DISCRETE_INPUT_NDISCRETES%8
extern UCHAR    ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8+1];
#else
extern UCHAR    ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8];
#endif
#if S_COIL_NCOILS%8
extern UCHAR    ucSCoilBuf[S_COIL_NCOILS/8+1];
#else
extern UCHAR    ucSCoilBuf[S_COIL_NCOILS/8];
#endif
extern uint16_t   usSRegInBuf[S_REG_INPUT_NREGS];
extern uint16_t   usSRegHoldBuf[S_REG_HOLDING_NREGS];
/* rtos timers handles */
extern xTimerHandle autostopEqModeTimer, ventilationTimer, changeChargeModeTimer, voltageChargeCalcTimer;

/* array for I_SENSOR_AB, I_SENSOR_COM, U_SENSOR, T_SENSOR */
extern uint32_t ADC1ConvertedValues[4];
uint32_t fltrIsensorCom, fltrTsensor, fltrUsensor, fltrIsensorAB;
/* temp arrays for sensors filtering */
extern uint32_t tempIsensorCom[I_SENSOR_COM_WINDOW], 
								tempTsensor		[T_SENSOR_WINDOW], 
								tempUsensor		[U_SENSOR_WINDOW], 
								tempIsensorAB	[I_SENSOR_AB_WINDOW];

/******************************************************************************/
/*            			Charger functions         							  */ 
/******************************************************************************/

/** 
	* @brief 	Default values setup.
	* @retval	None
 */
void setDefaultValues(void)
{
	TmaxAB 		= 50;			/* maximum batteries temperature */
	TminAB 		= 1;			/* minimum batteries temperature */
	ABnum 		= 102;			/* batteries number */
	Umax 		= 278;			/* maximum charge voltage */
	Umin 		= 180;			/* minimum charge voltage */
	Uzogr 		= 0;			/* charge voltage limit */
	Uf 			= 223;			/* */
	Ub 			= 240;			/* */
	Ue 			= 270;			/* */
	KizU 		= 400;			/* */
	Iab 		= 25;			/* battery current */
	Imax 		= 60;			/* maximum current surplus */
	KizIab 		= 250;			/* */
	KizIt 		= 125;			/* */
	T1 			= 15;			/* */
	T2 			= 35;			/* */
	Tcorr 		= 45;			/* */
	maxcorr 	= 2;			/* max long line offset */
	LineCorr 	= 10;			/* long line offset */
	KU 			= 3750;			/* voltage ratio */
	KI 			= 255;			/* current ratio */
	TimeB		= 24;			/* period for autostop boost mode */
	TimeE		= 72;			/* period for autostop equalize mode */
}
	
/** 
	* @brief 	Alarms and warnings pack.
	* @retval	None
 */
void packAlarmsWarnings(void)
{
	/* pack Al1 */
	Al1 = setBitInByte(Al1, 0, tpm < -50);								/* 1st bit if temperature less than -50 */
	Al1 = setBitInByte(Al1, 1, !POWER_SWITCH_ON);						/* 2nd bit if QF1 is switched off */
	Al1 = setBitInByte(Al1, 2, !UZP_READY);								/* 3th bit if QF2 is switched off */
	Al1 = setBitInByte(Al1, 3, !LOAD_SWITCH_ON);						/* 4th bit if QF9 is switched off */
	Al1 = setBitInByte(Al1, 4, tpm < TminAB);							/* 5th bit if temperature less than minimum */
	Al1 = setBitInByte(Al1, 5, tpm > TmaxAB);							/* 6th bit if temperature more than maximum */
	Al1 = setBitInByte(Al1, 6, VentAv);									/* 7th bit if auto-ventilation is switched on */
	Al1 = setBitInByte(Al1, 7, (!SUPPLY_220) | (!SUPPLY_24));			/* 8th bit if 24V or Power are switched off */
	
	/* pack Al2 */
	Al2 = setBitInByte(Al2, 0, (Rzd != CHARGE_MODE_STOP) & I_LIMIT);	/* 1st bit if charge is not in stop mode and current limit is switched on */
	Al2 = setBitInByte(Al2, 1, I_MAX);									/* 2nd bit if I_MAX is switched on */
	Al2 = setBitInByte(Al2, 2, PHASE_BREAK & POWER_SWITCH_ON);			/* 3th bit if phase break and QF1 is switched on */
	Al2 = setBitInByte(Al2, 3, OVERLOAD | Imper);						/* 4th bit if overload or Imper */
	Al2 = setBitInByte(Al2, 4, THYRISTOR_OVERHEAD);						/* 5th bit if not Tyr */
	Al2 = setBitInByte(Al2, 5, TRANSFORMER_OVERHEAD);					/* 6th bit if not Trans */
	Al2 = setBitInByte(Al2, 6, Zogr);									/* 7th bit if Zogr */
	Al2 = setBitInByte(Al2, 7, 0);										/* 8th bit is FALSE */
	
	/* pack Al3 */
	Al3 = setBitInByte(Al3, 0, I_MAX);									/* 1st bit if current maximum surplus */
	Al3 = setBitInByte(Al3, 1, PHASE_BREAK & UZP_READY);				/* 2nd bit if phase break and QF2 is switched on */
	Al3 = setBitInByte(Al3, 2, THYRISTOR_OVERHEAD);						/* 3th bit if not Tyr */
	Al3 = setBitInByte(Al3, 3, TRANSFORMER_OVERHEAD);					/* 4th bit if not Trans */
	Al3 = setBitInByte(Al3, 4, OVERLOAD | Imper);						/* 5th bit if overload or Imper */
	Al3 = setBitInByte(Al3, 5, Zogr);									/* 6th bit if Zogr */
	Al3 = setBitInByte(Al3, 6, 0);										/* 7th bit is FALSE */
	Al3 = setBitInByte(Al3, 7, 0);										/* 8th bit is FALSE */
	
	/* if warning flag set */
	if (Al1)
	{
		WARNING_ON;
		ALARM_LAMP_ON;
	} else
	{
		WARNING_OFF;
		ALARM_LAMP_OFF;
	}
	/* alarm flag set */
	if (Al3)
	{
		ALARM_ON;
		ALARM_LAMP_ON;
	} else
	{
		ALARM_OFF;
		ALARM_LAMP_OFF;
	}
}

/** 
	* @brief 	Temperature adduction.
	* @retval	None
 */
void temperatureAdd(void)
{
	tpm = T_SENSOR - 50;
}

/** 
	* @brief 	Temperature function.
	* @retval	None
 */
void Temperature(void)
{
	/* private variables */
	float fCompareTemp;									
	float fKTemp = tpm * (float)KizT;  		/* multiplication tpm and KizT */
	
	/* if T1 <= fKTemp <= T2 then DeTemp = 0 */
	if 	( !(								/* if not */
			(fKTemp < (float)T1)     		/* multiplication less than T1 */
				| 							/* OR */
			(fKTemp > (float)T2)			/* more than T2 */
			)) 
		DeTemp = 0;
			
	/* if fKTemp out of T1 or T2 */		
	else
	{		
		if (fKTemp < (float)T1)
		{
			fCompareTemp = ((fKTemp - (float)T1) * (float)Tcorr) / 1000;
			/* DeTemp equals the least of fCompareTemp or maxcorr */
			if ( fCompareTemp < (float)maxcorr )
			{
				DeTemp = fCompareTemp;
			} else 
			{
				DeTemp = (float)maxcorr;
			}
		}
		/* if (fKTemp > (float)T2) */
		else
		{
			fCompareTemp = ((fKTemp - (float)T2) * (float)Tcorr);
			/* DeTemp equals the least of fCompareTemp or maxcorr */
			if ( fCompareTemp < (float)maxcorr )
			{
				DeTemp = (float)fCompareTemp * (-1);
			} else 
			{
				DeTemp = (float)maxcorr * (-1);
			}
		}
	}
}

/** 
	* @brief  Initiate stop mode for list of events.
	* @retval None
 */
void stopMode(void)
{
	/* if any of event occured then set stop mode */
	if (
	THYRISTOR_OVERHEAD		|		/* thyristors overhead */
	TRANSFORMER_OVERHEAD	|		/* transformer overhead */
	I_MAX 					|		/* current maximum surplus */
	OVERLOAD				|		/* overload */
	PHASE_BREAK				|		/* phase break*/
	!COM_SWITCH_ON					/* common switch off */
	)
	{
		Rzd = CHARGE_MODE_STOP;
		stop = SET;
	}
	else 
	{
		stop = RESET;
	}
}

/** 
	* @brief  Initiate availability for list of events.
	* @retval None
 */
void availableMode(void)
{
/* if every of the events occures then set availability */
	if (
	!THYRISTOR_OVERHEAD		&		/* no thyristors overhead */
	!TRANSFORMER_OVERHEAD	&		/* no transformer overhead */
	!I_MAX 					&		/* no current surplus */
	!OVERLOAD				&		/* no overload */
	!PHASE_BREAK					/* no phase break */
	)
	{
		READY_ON;
	} 
	else
	{
		READY_OFF;
	}
}

/** 
	* @brief  Initiate sustain mode if NOT stop and SA1 is ON.
	* @retval	None
 */
void startWorking(void)
{
	if ( (Rzd != CHARGE_MODE_STOP) & !stop & COM_SWITCH_ON) 
		Rzd = CHARGE_MODE_FLOAT;
}

/** 
	* @brief  Switch off the contactor in fast and equalize mode.
	* @retval	None
 */
void offContactor(void)
{
	/* fast mode */
	UsZ = ((((Rzd == CHARGE_MODE_BOOST) & !VentAv) |			/* fast mode and auto ventilation off OR */
		(U_SENSOR * KizU > 2450)) 								/* Uin * KizU > 2450 */
	? SET : RESET );														

	/* equalize mode */
	UeZ = ((((Rzd == CHARGE_MODE_EQ) & !VentAv) |				/* equalize mode and auto ventilation off OR */
		(U_SENSOR * KizU > 2600))								/* Uin * KizU > 2600 */
	? SET : RESET );
}

/** 
	* @brief 	Equalize mode autostop after 100 hours.
	* @retval	None
 */
void autostopEqMode(void)
{
	/* if equalize mode */
	if (Rzd == CHARGE_MODE_EQ)
	{
		/* if the timer is not active then start it */
		if ( xTimerIsTimerActive( autostopEqModeTimer ) == pdFALSE )
		{
			/* set period as TimeE hours */
			xTimerChangePeriod(autostopEqModeTimer, TimeE * 3600000 / portTICK_RATE_MS, 0);
			/* start timer */
			xTimerStart(autostopEqModeTimer, 0);
		}
	} else 
	{
		/* else reset and stop the timer */
		xTimerReset	( autostopEqModeTimer, 0 );
		xTimerStop	( autostopEqModeTimer, 0 );
	}
}

/** 
	* @brief 	Callback function for autostop equalize mode timer.
	* @param  xTimer: Timer handle.
	* @retval	None
 */
void autostopEqModeTimerCallBack(xTimerHandle xTimer)
{
	/* change charge mode to sustain */
	Rzd = CHARGE_MODE_FLOAT;
}

/** 
	* @brief 	Equilise and boost mode automation. Ventilation control
	* @retval	None
 */
void chargeModeAutomation(void)
{
	/* if mode is equalise or boost */
	if (Rzd == CHARGE_MODE_BOOST | Rzd == CHARGE_MODE_EQ)
	{
		/* switch on ventilation */
		VENTILATION_ON;
		/* if timers is not active then start them */
		if ( xTimerIsTimerActive( ventilationTimer ) == pdFALSE )
			xTimerStart(ventilationTimer, 0);
		
		if ( xTimerIsTimerActive( changeChargeModeTimer ) == pdFALSE )
		{
			/* set period as TimeB hours */
			xTimerChangePeriod(changeChargeModeTimer, TimeB * 3600000 / portTICK_RATE_MS, 0);
			xTimerStart(changeChargeModeTimer, 0);
		}
	} else
	{
		/* stop ventilation and reset alarm flag */
		VENTILATION_OFF;
		VentAv = RESET;
		/* reset and stop the timers */
		xTimerReset	( ventilationTimer, 0);
		xTimerStop	( ventilationTimer, 0);
		xTimerReset	( changeChargeModeTimer, 0);
		xTimerStop	( changeChargeModeTimer, 0);
	}
	
	if ((Iab * 9.7) == (I_SENSOR_AB * KizIab) & 
		Rzd != CHARGE_MODE_EQ & Rzd != CHARGE_MODE_TEST & Iab)
		Rzd = CHARGE_MODE_FLOAT;
}

/** 
	* @brief 	Callback function for autostop equalize mode timer.
	* @param  	xTimer: Timer handle.
	* @retval	None
 */
void ventilationTimerCallBack(xTimerHandle xTimer)
{
	VentAv = !VENTILATION_CTRL;		
}

/** 
	* @brief 	Callback function for change charge mode timer.
	* @param  	xTimer: Timer handle.
	* @retval	None
 */
void changeChargeModeTimerCallBack(xTimerHandle xTimer)
{
	if (Rzd != CHARGE_MODE_EQ & Rzd != CHARGE_MODE_TEST & Iab)
		Rzd = CHARGE_MODE_FLOAT;
}

/** 
	* @brief 	Stop ventilation.
	* @retval	None
 */
void stopVentilation(void)
{
	if (((Rzd == CHARGE_MODE_FLOAT) & !VentAv) 	|	/* if charge in sustain mode and autoVentilation is off OR */
		(Rzd == CHARGE_MODE_TEST) 				|	/* charge in test mode OR */
		stop)										/* stop */ 
		VENTILATION_OFF;							/* then stop ventilation, else start */
	else 
		VENTILATION_ON;																				
}

/** 
	* @brief 	if STOP then Switch LED off 
	* @retval	None
 */
void runningLED(void)
{
	stop ? ALLOW_OFF : ALLOW_ON;
}

/** 
	* @brief 	Overload
	* @retval	None
 */
void overLoad(void)
{
	Imper = ((I_SENSOR_COM * KizIt) > (Imax * 10) ? SET : RESET);
}

/** 
	* @brief 	If stop mode then voltage and current equal zero.
	* @retval	None
 */
void commonStop(void)
{
	if ((Rzd == CHARGE_MODE_STOP) | stop)
	{
		U_SET = 0;
		I_SET = 0;
	}
}
	
/** 
	* @brief 	If Umax * 10.5 less than Uin * KizU OR RESET then STOP
	* @retval	None
 */
void voltageStop(void)
{
	stop = (((Umax * 10.5) < (U_SENSOR * KizU)) | Reset
	? SET : RESET);
}

/**
	* @brief 	Reset SIFU if Reset
	* @retval	None
 */
void resetSIFU(void)
{
	if (Reset == 1)
	{
		stop = SET;
		RESET_SIFU_ON;
	}
	else
	{
		stop = RESET;
		RESET_SIFU_OFF;
	}
}

/** 
	* @brief 	Deep uncharge protection
	* @retval	None
 */
void unchargeProtect(void)
{
	if((U_SENSOR * KizU) < (Uzogr * 10))
	{
		Zogr = SET;
		LOW_CHARGE_PROTECT_ON;
	} 
	else
	{
		Zogr = RESET;
		LOW_CHARGE_PROTECT_OFF;
	}
}

/**
	* @brief 	Calculation for voltage
	* @retval	None
 */
void voltageChargeCalc(void)
{															
	/* voltage calculation */
	Urout = (((I_SENSOR_AB * KizIab * LineCorr) / 100 ) / KU) +
	(DeTemp / 1000) +
	(Uzd / KU);
	
	/* voltage value for PWM. fabc - module */
	if ((fabs(Urout - U_SET)) < 10)
		U_SET = Urout;
	
	/* if Urout is not equal Uout and SA1 is ON */
	if ((Urout != U_SET) & COM_SWITCH_ON)
	{
		/* if the timer is not active then start it */
		if ( xTimerIsTimerActive( voltageChargeCalcTimer ) == pdFALSE )
			xTimerStart(voltageChargeCalcTimer, 0);
	} else 
	{
		/* else reset and stop the timer */
		xTimerReset	( voltageChargeCalcTimer, 0 );
		xTimerStop	( voltageChargeCalcTimer, 0 );
	}
}

/** 
	* @brief 	Callback function for the voltage calculation.
	* @param  xTimer: Timer handle.
	* @retval	None
 */
void voltageChargeCalcTimerCallBack(xTimerHandle xTimer)
{
	/* rising voltage */
	if (Urout > U_SET)
		U_SET += 5;
	/* decrease voltage */
	else
		U_SET -= 5;
}

/** 
	* @brief 	Calculation for current
	* @retval	None
 */
void currentChargeCalc(void)
{
	/* Iab cannot be more than Imax */
	if (Iab < Imax)
	{
		I_SET = 	((float)KI / 1000) *
						((float)Iab / Imax) * 10;
	}
}

/** 
	* @brief 	Calculation for voltage definition
	* @retval	None
 */
void voltageDefCalc(void)
{
	float UzdTemp;
	if (ABnum < 132)
	{
		/* if Ventilation is ok */
		if (!VentAv)
		{
			switch (Rzd)
			{
				case CHARGE_MODE_STOP:
					UzdTemp = 0;
					break;
				case CHARGE_MODE_FLOAT:
					UzdTemp = ABnum * Uf;
					break;
				case CHARGE_MODE_BOOST:
					UzdTemp = ABnum * Ub;
					break;
				case CHARGE_MODE_EQ:
					UzdTemp = ABnum * Ue;
					break;
				case CHARGE_MODE_TEST:
					UzdTemp = ABnum * Uf * 0.8;
					break;
			}
		/* if ventilation in alarm mode */
		} else
		{
			UzdTemp = ABnum * Uf;
		}
		/* if less then voltage limit */
		if (Umax * 100 > UzdTemp)
			Uzd = UzdTemp;
		else
			Uzd = Umax * 100;
	}
}

/** 
	* @brief 	Ventilation control
	* @retval	None
 */
void ventilationAlarm(void)
{
	if (Rzd == CHARGE_MODE_BOOST | Rzd == CHARGE_MODE_EQ)
		VENTILATION_ON;
}

/************************ (C) COPYRIGHT ***** END OF FILE ****/

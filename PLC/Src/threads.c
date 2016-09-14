/**
  ******************************************************************************
  * @file    	threads.c
  * @author  	tech department
  * @version 	0.1
  * @date    	22-07-2016
  * @brief   	Code for os threads
	* @verbatim
	*
	*
	* @endverbatim
  * COPYRIGHT(c) 2016
	******************************************************************************
  */
#include "stm32f4xx_hal.h"
#include "threads.h"
#include "charger.h"
#include "otherfunct.h"
#include <math.h>

/* External variables */
/* Modbus registers variables */
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

extern ADC_HandleTypeDef hadc2;
/******************************************************************************/
/*            				FreeRTOS threads         						  */ 
/******************************************************************************/
/* start task for calculating */
void StartCalcThread(void const * argument)
{
	/* External variables */
	extern uint32_t fltrIsensorCom, fltrTsensor, fltrUsensor, fltrIsensorAB;
	extern uint32_t ADC1ConvertedValues[4];
	/* Private variables */
	/* temp arrays for filter buffering */
	uint32_t 	tempIsensorCom	[I_SENSOR_COM_WINDOW 	+ 1], 
				tempTsensor		[T_SENSOR_WINDOW 		+ 1], 
				tempUsensor		[U_SENSOR_WINDOW 		+ 1], 
				tempIsensorAB	[I_SENSOR_AB_WINDOW 	+ 1];
	
	HAL_ADC_Start(&hadc2);
	/* Infinite loop */
	for (;;)
	{
		/* filtering ADC */
		fltrIsensorCom 	= trpFilter(ADC1ConvertedValues[0], tempIsensorCom, I_SENSOR_COM_WINDOW);
		fltrTsensor 	= trpFilter(ADC1ConvertedValues[1], tempTsensor, 	T_SENSOR_WINDOW);
		fltrUsensor 	= trpFilter(ADC1ConvertedValues[2], tempUsensor, 	U_SENSOR_WINDOW);
		fltrIsensorAB 	= trpFilter(ADC1ConvertedValues[3], tempIsensorAB, 	I_SENSOR_AB_WINDOW);	
		
		/* calculation functions */
		temperatureAdd ();
		Temperature ();
		autostopEqMode ();
		chargeModeAutomation ();
		overLoad();
		unchargeProtect();
		voltageChargeCalc();
		currentChargeCalc();
		voltageDefCalc();
		
		/* delay for calculating (0.5 sec is normal) */
		osDelay(100);
	}
}
//******************************************************************************
/* start fast task */
void StartFastThread(void const * argument)
{
	/* External variables */
	extern int16_t	tpm;
	extern uint32_t ADC1ConvertedValues[4];	/* 	extern array for I_SENSOR_AB, I_SENSOR_COM, U_SENSOR, T_SENSOR */
	extern uint32_t fltrIsensorCom, fltrTsensor, fltrUsensor, fltrIsensorAB;
	
	/* Infinite loop */
  for(;;)
  {
		packAlarmsWarnings();
		stopMode();
		availableMode();
		startWorking ();
		stopVentilation();
		runningLED();
		commonStop();
		voltageStop();
		resetSIFU();
		ventilationAlarm();

		/* get values for registers */
		usSRegHoldBuf[31] = U_SENSOR * KizU;
		usSRegHoldBuf[32] = I_SENSOR_AB * KizIab;
		usSRegHoldBuf[33] = I_SENSOR_COM * KizIt;
		usSRegHoldBuf[34] = fabs((float)tpm);

		osDelay(1);
  }
}

//******************************************************************************
/* Task for HMI modbus init and polling */
void StartModbusHMIThread(void const * argument)
{
	/* External variables */
	extern MBPort_InitTypeDef mbPortHMI;			/* modbus handler */
	
	/* Private variables */
	eMBErrorCode    	eStatus = MB_ENOERR;		/* Init and enable modbus error code */
	
	/* modbus handler init */
	mbPortHMI.UARTHandler.Instance = USART2;
	mbPortHMI.TIMHandler.Instance = TIM2;
	
	/* specify modbus type, slave address, baud rate, parity */
	eStatus = eMBInit(&mbPortHMI, MB_RTU, 0x10, 0, 9600, MB_PAR_NONE);
	
	/* Enable modbus */
	eStatus = eMBEnable(&mbPortHMI);
	
	/* if error */
	if ( eStatus != MB_ENOERR ) 
	{
		/* modbus alarm */
	}
	
	/* Infinite loop */
	for(;;)
	{
		/* start modbus polling */
		(void) eMBPoll(&mbPortHMI);
		
		osDelay(100);
	}
}

//******************************************************************************
/* Task for extern modbus init and polling */
void StartModbusExternThread(void const * argument)
{
	/* External variables */
	extern MBPort_InitTypeDef mbPortExtern;		/* modbus handler */
	
	/* Private variables */
	eMBErrorCode    	eStatus = MB_ENOERR;		/* Init and enable modbus error code */
	
	/* modbus handler init */
	mbPortExtern.UARTHandler.Instance = UART5;
	mbPortExtern.TIMHandler.Instance = TIM3;
	
	/* specify modbus type, slave address, baud rate, parity */
	eStatus = eMBInit(&mbPortExtern, MB_RTU, 0x0A, 0, 115200, MB_PAR_NONE);
	
	/* Enable modbus */
	eStatus = eMBEnable(&mbPortExtern);
	/* if error */
	if ( eStatus != MB_ENOERR ) 
	{
		/* modbus alarm */
	}
  /* Infinite loop */
  for(;;)
  {
		/* start modbus polling */
		(void) eMBPoll(&mbPortExtern);
	
		osDelay(100);
  }
}

//******************************************************************************
/* start cheking parameters changes */
void StartChangeParamCheckThread(void const * argument)
{
	/* External variables */
	extern uint16_t memoryTemp[REGS_NUM];   	/* temp array to compare old and new values in modbus holding buffer */
	
	/* Infinite loop */
	for(;;)
	{
		/* compare every register in memoryTemp and modbus holding buffer */
		for (uint16_t i = 0; i < REGS_NUM - 1; i++)
		{
			/* if not equals */
			if (memoryTemp[i] != usSRegHoldBuf[i])
			{
				portENTER_CRITICAL();
				{
					/* then rewrite flash from modbus holding buffer */
					writeFlash(ADDRESS, usSRegHoldBuf, REGS_NUM);
				}
				portEXIT_CRITICAL();
			}
			/* write to memoryTemp from modbus holding buffer after check */
			memoryTemp[i] = usSRegHoldBuf[i];
		}
		
		osDelay(100);
	}
}

/************************ (C) COPYRIGHT *****END OF FILE****/

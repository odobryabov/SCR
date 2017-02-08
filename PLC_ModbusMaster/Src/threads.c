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
#include "otherfunct.h"
#include <math.h>

/* External variables */
/* Modbus slave registers variables */
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

/* Modbus master registers variables */
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
#if      M_DISCRETE_INPUT_NDISCRETES%8
extern UCHAR    ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8+1];
#else
extern UCHAR    ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8];
#endif
#if      M_COIL_NCOILS%8
extern UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8+1];
#else
extern UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8];
#endif
extern USHORT   usMRegInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS];
extern USHORT   usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];
#endif

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
	/* ADC filter ratio (3 is normal) */
	const uint16_t 	windowISensorCom 	= 3,
					windowTSensor 		= 3,
					windowUSensor 		= 3,
					windowISensorAB 	= 3;
	
	/* temp arrays for filter buffering */
	uint32_t 	tempIsensorCom	[windowISensorCom 	+ 1], 
				tempTsensor		[windowTSensor 		+ 1], 
				tempUsensor		[windowUSensor 		+ 1], 
				tempIsensorAB	[windowISensorAB 	+ 1];
	
	HAL_ADC_Start(&hadc2);
	/* Infinite loop */
	for (;;)
	{
		/* filtering ADC */
		fltrIsensorCom 	= trpFilter(ADC1ConvertedValues[0], tempIsensorCom, windowISensorCom);
		fltrTsensor 	= trpFilter(ADC1ConvertedValues[1], tempTsensor, 	windowTSensor);
		fltrUsensor 	= trpFilter(ADC1ConvertedValues[2], tempUsensor, 	windowUSensor);
		fltrIsensorAB 	= trpFilter(ADC1ConvertedValues[3], tempIsensorAB, 	windowISensorAB);	
		
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
		usSRegHoldBuf[33] = ADC1ConvertedValues[0];//I_SENSOR_COM * KizIt;
		usSRegHoldBuf[34] = fabs((float)tpm);

		osDelay(1);
  }
}

//******************************************************************************
/* Task for HMI modbus init and polling */
void StartModbusHMIThread(void const * argument)
{
	/* External variables */

	
	/* Private variables */
	eMBErrorCode    	eStatus = MB_ENOERR;		/* Init and enable modbus error code */
	
	/* specify modbus type, slave address, baud rate, parity */
	eStatus = eMBInit(MB_RTU, 0x10, 0, 9600, MB_PAR_NONE);
	
	/* Enable modbus */
	eStatus = eMBEnable();
	
	/* if error */
	if ( eStatus != MB_ENOERR ) 
	{
		/* modbus alarm */
	}
	
	/* Infinite loop */
	for(;;)
	{
		/* start modbus polling */
		(void) eMBPoll();
		osDelay(1);
	}
}

//******************************************************************************
/* Task for extern modbus init and polling */
void StartModbusExternThread(void const * argument)
{
//	/* External variables */
//	extern MBPort_InitTypeDef mbPortExtern;		/* modbus handler */
//	
//	/* Private variables */
//	eMBErrorCode    	eStatus = MB_ENOERR;		/* Init and enable modbus error code */
//	
//	/* modbus handler init */
//	mbPortExtern.UARTHandler.Instance = UART5;
//	mbPortExtern.TIMHandler.Instance = TIM3;
//	
//	/* specify modbus type, slave address, baud rate, parity */
//	eStatus = eMBInit(&mbPortExtern, MB_RTU, 0x10, 0, 9600, MB_PAR_NONE);
//	
//	/* Enable modbus */
//	eStatus = eMBEnable(&mbPortExtern);
//	/* if error */
//	if ( eStatus != MB_ENOERR ) 
//	{
//		/* modbus alarm */
//	}
//  /* Infinite loop */
  for(;;)
  {
//		/* start modbus polling */
//		(void) eMBPoll(&mbPortExtern);
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
		osDelay(500);
  }
}

//******************************************************************************
/* start cheking parameters changes */
void StartChangeParamCheckThread(void const * argument)
{
	/* External variables */
//	extern uint16_t memoryTemp[REGS_NUM];   	/* temp array to compare old and new values in modbus holding buffer */
	
	/* Infinite loop */
	for(;;)
	{
		/* compare every register in memoryTemp and modbus holding buffer */
//		for (uint16_t i = 0; i < REGS_NUM - 1; i++)
//		{
//			/* if not equals */
//			if (memoryTemp[i] != usSRegHoldBuf[i])
//			{
//				portENTER_CRITICAL();
//				{
//					/* then rewrite flash from modbus holding buffer */
//					writeDataToMemory(usSRegHoldBuf, REGS_NUM);
//				}
//				portEXIT_CRITICAL();
//			}
//			/* write to memoryTemp from modbus holding buffer after check */
//			memoryTemp[i] = usSRegHoldBuf[i];
//		}
		
		osDelay(100);
	}
}

void StartModbusMaster(void const * argument)
{
//	uint32_t const requestNumber = 1;
//	uint32_t requestCounter = 0;
	eMBErrorCode		eStatus;
	eMBMasterReqErrCode eErrCode;
	
	eStatus = eMBMasterInit(MB_RTU, 0, 9600, MB_PAR_NONE);
	eStatus = eMBMasterEnable();
	
	for (;;)
	{	
//			switch (requestCounter % requestNumber)
//			{
//				case 1:
//					eMBMasterReqReadHoldingRegister(1, 1, 1, 1000);
//					break;
//				
//				default:
//					break;
//			}
//			requestCounter++;
		
		eMBMasterReqReadHoldingRegister(1, 1, 1, 500);

//		if (usMRegHoldBuf[1 - 1][1] == 1111)
//			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
//		else 
//			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

		(void)eMBMasterPoll();
		osDelay(100);
	}
}
/************************ (C) COPYRIGHT *****END OF FILE****/

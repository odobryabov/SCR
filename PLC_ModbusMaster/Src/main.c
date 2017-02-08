/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
	

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "periph.h"
#include "pfc.h"
#include "threads.h"
#include "otherfunct.h"
/* USER CODE END Includes */

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

uint16_t memoryTemp[REGS_NUM];   	/* 	temp array to compare old and new values in modbus holding buffer */
/* Global variables ----------------------------------------------------------*/
/* rtos timers variables */
/* handles */
xTimerHandle 					autostopEqModeTimer, 
								ventilationTimer, 
								changeChargeModeTimer, 
								voltageChargeCalcTimer;
/* IDs */
const unsigned portBASE_TYPE 	autostopEqModeTimerID, 
								changeChargeModeTimerID, 
								ventilationTimerID, 
								voltageChargeCalcTimerID;																
/* Private variables ---------------------------------------------------------*/
osThreadId threadHandle;

/* USER CODE BEGIN PV */
UART_HandleTypeDef huart2;
/* USER CODE END PV */

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_TIM1_Init();
	MX_TIM8_Init();
	MX_ADC1_Init();

//	MX_ADC2_Init();
//	MX_TIM10_Init();
//	MX_TIM11_Init();
//	MX_TIM13_Init();
//	MX_TIM14_Init();
  
//	PFCInit();
	/* USER CODE BEGIN 2 */
	
//	/* first load from flash */
//	readDataFromMemory(usSRegHoldBuf, REGS_NUM);
//	/* if flash is empty (check first word, all bits are 1 (NAND type of flash))*/
//	if (usSRegHoldBuf[0] == 0xFFFF)
//	{
//		/* then set default config */
//		setDefaultValues();
//		/* and write it to memory */
//		writeDataToMemory(usSRegHoldBuf, REGS_NUM);
//	}
//	/* copy from memory to memoryTemp */
//	readDataFromMemory(memoryTemp, 		REGS_NUM);
	
	TimeB			= 24;				/* initialise period for autostop boost mode */
	TimeE			= 72;				/* initialise period for autostop equalize mode */
	/* USER CODE END 2 */

	/* USER CODE BEGIN RTOS_MUTEX */
  
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	autostopEqModeTimer 	= xTimerCreate(	"CreatingAutostopEqModeTimer", 				/* Just a text name, not used by the kernel */
											TimeE * 3600000 / portTICK_RATE_MS,  		/* period is TimeE hours */
											pdFALSE, 									/* single circle timer type */
											(void *) &autostopEqModeTimerID, 			/* timer ID */
											autostopEqModeTimerCallBack );				/* callback function */
	
	ventilationTimer 		= xTimerCreate(	"CreatingVentilationTimer", 				/* Just a text name, not used by the kernel */
											10000			/ portTICK_RATE_MS,  		/* period is 10 sec */
											pdFALSE, 									/* single circle timer type */
											(void *) &ventilationTimerID, 				/* timer ID */
											ventilationTimerCallBack );					/* callback function */

	changeChargeModeTimer	= xTimerCreate(	"CreatingChangeChargeModeTimer", 			/* Just a text name, not used by the kernel */
											TimeB * 3600000 / portTICK_RATE_MS,  		/* period is TimeB hours */
											pdFALSE, 									/* single circle timer type */
											(void *) &changeChargeModeTimerID, 			/* timer ID */
											changeChargeModeTimerCallBack	);			/* callback function */
																							
	voltageChargeCalcTimer 	= xTimerCreate(	"CreatingVoltageChargeCalcTimer", 			/* Just a text name, not used by the kernel */
											500 			/ portTICK_RATE_MS,  		/* period 0.5 sec */
											pdTRUE, 									/* auto-reload timer type */
											(void *) &voltageChargeCalcTimerID, 		/* timer ID */
											voltageChargeCalcTimerCallBack );			/* callback function */
	/* USER CODE END RTOS_TIMERS */
																							
	/* USER CODE BEGIN RTOS_THREADS */
	/* definition and creation of calcTask */
	osThreadDef(calcThread, StartCalcThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
	threadHandle = osThreadCreate(osThread(calcThread), NULL);
																							
	/* definition and creation of alarmTask */
	osThreadDef(fastThread, StartFastThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
	threadHandle = osThreadCreate(osThread(fastThread), NULL);
																							
	/* definition and creation of modbusHMITask */
//	osThreadDef(modbusHMIThread, StartModbusHMIThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
//	threadHandle = osThreadCreate(osThread(modbusHMIThread), NULL);
																							
	/* definition and creation of modbusExternTask */
	osThreadDef(modbusExternThread, StartModbusExternThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
	threadHandle = osThreadCreate(osThread(modbusExternThread), NULL);
																																												
	/* definition and creation of changeParamCheckTask */
	osThreadDef(changeParamCheckThread, StartChangeParamCheckThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
	threadHandle = osThreadCreate(osThread(changeParamCheckThread), NULL);
	
	/* definition and creation of changeParamCheckTask */
	osThreadDef(modbusMaster, StartModbusMaster, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
	threadHandle = osThreadCreate(osThread(modbusMaster), NULL);
	
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */
 
	/* Start scheduler */
	osKernelStart();
  
	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

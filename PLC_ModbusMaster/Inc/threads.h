/**
  ******************************************************************************
  * @file    	threads.h
  * @author  	tech department
  * @version 	0.1
  * @date    	22-07-2016
  * @brief   	Header file for os threads
	* @verbatim
	*
	*
	* @endverbatim
  * COPYRIGHT(c) 2016
	******************************************************************************
  */
#ifndef THREADS_H
#define THREADS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"
#include "user_mb_app.h"
#include "plc.h"

/* Private function prototypes -----------------------------------------------*/
void StartModbusExternThread		(void const * argument);
void StartModbusHMIThread			(void const * argument);
void StartChangeParamCheckThread	(void const * argument);
void StartCalcThread				(void const * argument);
void StartFastThread				(void const * argument);
void StartModbusMaster				(void const * argument);

#endif
/************************ (C) COPYRIGHT *****END OF FILE****/

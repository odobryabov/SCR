/**
  ******************************************************************************
  * @file    	otherfunct.h
  * @author  	tech department
  * @version 	0.1
  * @date    	22-07-2016
  * @brief   	Header file for other user functions
	* @verbatim
	*
	*
	* @endverbatim
  * COPYRIGHT(c) 2016
	******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OTHERFUNCT_H
#define OTHERFUNCT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "cmsis_os.h"

uint16_t setBitInByte (uint16_t iByte, uint16_t iPosition, BaseType_t btCondition);
FlagStatus readBitInByte(uint16_t Byte, uint16_t Position);
uint32_t EMA(uint32_t input, float alfa, float temp);
uint32_t trpFilter (uint32_t input, uint32_t *temp, uint32_t window);
void writeFlash(uint32_t dataAddress, uint16_t *Data, uint32_t lenght);
uint16_t readFlash(uint32_t dataAddress);

#endif
/************************ (C) COPYRIGHT *****END OF FILE****/

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

void setBitInByte (uint16_t *Byte, uint16_t Position, FlagStatus Condition);
FlagStatus readBitInByte(uint16_t Byte, uint16_t Position);
uint32_t EMA(uint32_t input, float alfa, float temp);
uint32_t trpFilter (uint32_t input, uint32_t *temp, uint32_t window);
void convertArray16to8(uint16_t* arr16, uint8_t* arr8, uint16_t sizeOfArr16);
void convertArray8to16(uint16_t* arr16, uint8_t* arr8, uint16_t sizeOfArr16);
void writeDataToMemory(uint16_t *data, uint16_t size);
void readDataFromMemory(uint16_t *data, uint16_t size);
void writeFlash(uint32_t dataAddress, uint16_t *data, uint16_t size);
void readFlash(uint32_t dataAddress, uint16_t *data, uint16_t size);
void writeEEPROM(uint16_t dataAddress, uint16_t *data, uint16_t size);
void readEEPROM(uint16_t dataAddress, uint16_t *data, uint16_t size);

#endif
/************************ (C) COPYRIGHT *****END OF FILE****/

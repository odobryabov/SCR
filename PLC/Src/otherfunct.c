/**
  ******************************************************************************
  * @file    	otherfunct.c
  * @author  	tech department
  * @version 	0.1
  * @date    	22-07-2016
  * @brief   	Code for other user functions
	* @verbatim
	*
	*
	* @endverbatim
  * COPYRIGHT(c) 2016
	******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "otherfunct.h"
#include <math.h>

/******************************************************************************/
/*            							Other functions         													*/ 
/******************************************************************************/

/** 
	* @brief 	Set or reset bit in word.
	* @param 	Byte: variable to be changed.
	* @param 	Position: position (0..15) to be changed in the variable.
	* @param 	Condition: value of the bit: TRUE or FALSE 
	* 				(or returning value of a function).
	* @retval Changed word.
 */
uint16_t setBitInByte(uint16_t Byte, uint16_t Position, BaseType_t Condition)
{
	if (Condition)
	{
			/* if TRUE then set bit as 1 */
			Byte |=   (uint16_t)1 << Position;
	} else 
	{
			/* if FALSE then set bit as 0 */
			Byte &= ~((uint16_t)1 << Position);
	}
	return Byte;
}

/** 
	* @brief 	Get value of a bit in a word.
	* @param 	Byte: target word.
	* @param 	Position: position (0..15) of the target bit in the word.
	* @retval Bit value.
 */
FlagStatus readBitInByte(uint16_t Byte, uint16_t Position)
{
	if (Byte & ((uint16_t)1 << Position))
	{
		return SET;
	} 
	else
	{
		return RESET;
	}
}

/** 
	* @brief  exponential moving average.
	* @param  input: input voltage value.
	* @param  alfa: ratio ( 0 < alfa < 1).
	* @param 	temp: extern buffer variable.
	* @retval output voltage value
 */
uint32_t EMA(uint32_t input, float alfa, float temp)
{
	/* private variables */
	float output;
	
	output = temp * alfa + (float)input * (1 - alfa);
	
	temp = output;
	
	return (uint32_t)temp;
}

/** 
	* @brief  trapezoid area average. 
	* @brief  First return values (windows numder) are incorrect 
	* 				while temp array is not full of input values
	* @param  input: input value.
	* @param  temp: extern buffer array (window + 1 is size)
	* @param  window: smooth width
	* @retval output value
 */
uint32_t trpFilter(uint32_t input, uint32_t* temp, uint32_t window)
{
	/* private variables */
	float sqSum = 0; /* area sum */
	
	/* place back an input value */
	temp[window] = input;
	/* push back a temp array */
	for (uint16_t i = 0; i < window; i++)
		temp[i] = temp[i+1];
	
	/* area sum calculating. */
	/* trapezoid area equals (|a - b|*h)/2 + b*h. h = 1 */
	for (uint16_t i = 0; i < window; i++)
		sqSum += fabs((float)temp[i+1] - (float)temp[i]) / 2 + temp[i];
	
	/* return an average square */
	return (uint32_t)(sqSum / window);
}

/** 
	* @brief  Write data array in flash memory.
	* @param  dataAddress: address for the first byte
	* @param	*Data: pointer to the data array to be write.
	* @param	lenght: array lenght.
	* @retval None
 */
void writeFlash(uint32_t dataAddress, uint16_t *Data, uint32_t lenght)
{
	/* private variables */
	uint32_t * flashArray = (uint32_t *) dataAddress; 	/* array as a pointer to the address */
	
	/* Unlock the FLASH control register access */
	HAL_FLASH_Unlock();
	/* Erase the specified FLASH memory sector */
	FLASH_Erase_Sector(FLASH_SECTOR_8, FLASH_VOLTAGE_RANGE_3 );

	/* waiting */
	while (FLASH->SR & FLASH_SR_BSY);
	/* ready */
	FLASH->CR |= FLASH_CR_PG;
	
	/* write array */
	for (uint16_t i = 0; i < lenght; i++)
	{
		flashArray[i] = Data[i];
		/* waiting */
		while (FLASH->SR & FLASH_SR_BSY);
	}
	
	/* finish */
	FLASH->CR &= ~FLASH_CR_PG;

	/* Lock the FLASH control register access */
	HAL_FLASH_Lock();
}

/** 
	* @brief  Read data from flash memory.
	* @param  dataAddress: pointer to the data cell to be read.
	* @retval Data
 */
uint16_t readFlash(uint32_t dataAddress)
{
	/* return value from pointer to pointer to address */
	return * (uint16_t*) dataAddress;
}

/************************ (C) COPYRIGHT ***** END OF FILE ****/

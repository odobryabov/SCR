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
#include "cmsis_os.h"
#include "plc.h"
#include "arm_math.h"
#include <math.h>

/******************************************************************************/
/*            				Other functions         						  */ 
/******************************************************************************/

/** 
	* @brief 	Set or reset bit in word.
	* @param 	Byte: variable to be changed.
	* @param 	Position: position (0..15) to be changed in the variable.
	* @param 	Condition: value of the bit: TRUE or FALSE 
	* 				(or returning value of a function).
	* @retval Changed word.
 */
void setBitInByte(uint16_t *Byte, uint16_t Position, FlagStatus Condition)
{
	if (Condition == SET)
	{
			/* if TRUE then set bit as 1 */
			*Byte |= (uint16_t)1 << Position;
	} else 
	{
			/* if FALSE then set bit as 0 */
			*Byte &= ~((uint16_t)1 << Position);
	}
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


uint16_t pfcPID (uint16_t input, uint16_t Kp, uint16_t Ki, uint16_t Kd, uint16_t delay)
{
	static uint16_t errorNew = 0, 
					output = 0;
	uint16_t errorPrev;
	
	errorPrev = errorNew;
	errorNew = input - output;
	
	//output += /* integrating part */
	//output += /*  */
	output *= Kp; /* proportional part */
	
	osDelay(delay);
	
	return output;
}
/** 
	* @brief  Convert 16-bit array to 8-bit array.
	* @param  arr16: pointer to the 16-bit array.
	* @param  arr8: pointer to the 8-bit array.
	* @param  sizeOfArr16: 16-bit array lenght.
	* @retval None
 */
void convertArray16to8(uint16_t* arr16, uint8_t* arr8, uint16_t sizeOfArr16)
{
	for (uint16_t i = 0; i < sizeOfArr16; i++)
	{
		arr8[i*2] 	= arr16[i] & 0xFF;
		arr8[i*2+1] = arr16[i] >> 8;
	}
}

/** 
	* @brief  Convert 8-bit array to 16-bit array
	* @param  arr16: pointer to the 16-bit array.
	* @param  arr8: pointer to the 8-bit array.
	* @param  sizeOfArr16: 16-bit array lenght.
	* @retval None
*/
void convertArray8to16(uint16_t* arr16, uint8_t* arr8, uint16_t sizeOfArr16)
{
	for (uint16_t i = 0; i < sizeOfArr16; i++)
		arr16[i] = arr8[i*2] | (arr8[i*2 + 1]<<8);
}

/** 
	* @brief  Write data common function.
	* @param  *data: pointer to the data array to be write.
	* @param  size: array lenght.
	* @retval None
 */
void writeDataToMemory(uint16_t *data, uint16_t size)
{
	//const uint32_t address = 0x08080000; 	  /* adress for write/read flash. flash sector 8, 0x08080000 - 0x0809FFFF, 128KB, EEPROM, NAND */
	const uint16_t address = 0x0000; 		/* adress for write/read i2c eeprom */
		
	//writeFlash(address, data, size);
	writeEEPROM(address, data, size);
}
	
/** 
	* @brief  Read data common function.
	* @param  *data: pointer to the data array to be read.
	* @param  size: array lenght.
	* @retval None
 */	
void readDataFromMemory(uint16_t *data, uint16_t size)
{
	//const uint32_t address = 0x08080000; 	  /* adress for write/read flash. flash sector 8, 0x08080000 - 0x0809FFFF, 128KB, EEPROM, NAND */
	const uint16_t address = 0x0000; 		/* adress for write/read i2c eeprom */
	
	//readFlash(MODBUS_VARS_ADDR, data, size);
	readEEPROM(address, data, size);
}
	
/** 
	* @brief  Write data array in flash memory.
	* @param  dataAddress: address for the first byte
	* @param	*Data: pointer to the data array to be write.
	* @param	size: array lenght.
	* @retval None
 */
void writeFlash(uint32_t dataAddress, uint16_t *data, uint16_t size)
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
	for (uint16_t i = 0; i < size; i++)
	{
		flashArray[i] = data[i];
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
	* @param  *data: pointer to the data array to be read.
	* @param  size: array lenght.
	* @retval None
 */
void readFlash(uint32_t dataAddress, uint16_t *data, uint16_t size)
{
	for (uint16_t i = 0; i < size; i++)
		/* get value from pointer to pointer to address */
		/* i*sizeof(uint32_t): shift address to 4 steps (one cell is 32bit (4 bytes)) */
		data[i] = * (uint16_t*) (dataAddress + i * sizeof(uint32_t));
	//return * (uint16_t*) dataAddress;
}

/** 
	* @brief  Write data to i2c eeprom.
	* @param  *data: pointer to the data array to be write.
	* @param  size: array lenght.
	* @retval None
 */
void writeEEPROM(uint16_t dataAddress, uint16_t *data, uint16_t size)
{
	extern I2C_HandleTypeDef hi2c3;
	const uint16_t deviceAddress = 0x0A;
	uint8_t* arr8;
	
	convertArray16to8(data, arr8, size);
	
	HAL_I2C_Mem_Write(&hi2c3, deviceAddress<<1, dataAddress, size*2, arr8, size, 5);
	
    osDelay(10);
}

/** 
	* @brief  Read data from i2c eeprom.
	* @param  *data: pointer to the data array to be read.
	* @param  size: array lenght.
	* @retval None
 */
void readEEPROM(uint16_t dataAddress, uint16_t *data, uint16_t size)
{
	extern I2C_HandleTypeDef hi2c3;
	const uint16_t deviceAddress = 0x0A;
	uint8_t* arr8;
	
	HAL_I2C_Mem_Read(&hi2c3, deviceAddress << 1, dataAddress, size*2, arr8, size, 5);
	
	convertArray8to16(data, arr8, size);
}
/************************ (C) COPYRIGHT ***** END OF FILE ****/

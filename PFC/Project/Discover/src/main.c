/**
  ******************************************************************************
  * @file    main.c
  * @author  Microcontroller Division
  * @version V1.2.0
  * @date    09/2010
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */
 
/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "stm8l_discovery_lcd.h"
#include "discover_board.h"
#include "icc_measure.h"
#include "discover_functions.h"
#include "pfc.h"

/* Machine status used by main for active function set by user button in interrupt handler */
uint8_t state_machine;

/* LCD bar graph: used for display active function */
extern uint8_t t_bar[2];

/* Auto_test activation flag: set by interrupt handler if user button is pressed few seconds */
extern bool Auto_test;

/* Set in interrupt handler for indicate that user button is pressed */ 
extern bool KeyPressed;

extern PFC_TypeDef PFC;
uint16_t i = 0xFFFF;
uint16_t ADC_val;

/**
  * @brief main entry point.
  * @par Parameters None
  * @retval void None
  * @par Required preconditions: None
  */
void main(void)
{ 
  /* system clocking config */
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  
  PFCSCRInit();
  PFCTimersInit();
  PFCADCInit();
  //PFC_DAC_init();
	 	
  GPIO_Init( GPIOC, GPIO_Pin_1, GPIO_Mode_In_FL_No_IT);
//  
GPIO_Init( GPIOE, GPIO_Pin_7, GPIO_Mode_Out_PP_High_Fast);
//  GPIO_Init( GPIOC, GPIO_Pin_7, GPIO_Mode_Out_PP_High_Fast);
//  
//  //GPIO_Init( GPIOA, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);
//  
//  GPIO_WriteBit(GPIOC, GPIO_Pin_7, RESET);
 GPIO_WriteBit(GPIOE, GPIO_Pin_7, RESET);
//  
//  GPIO_WriteBit(GPIOE, GPIO_Pin_0, RESET);
  GPIO_Init( GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);
  GPIO_Init( GPIOB, GPIO_Pin_2, GPIO_Mode_In_PU_No_IT);

  /* enable global interrupts */
  __enable_interrupt();
  while (1)
  {
    
    //ADC_val = PFCGetADC();
    //DAC_SetChannel1Data(DAC_Align_12b_R, ADC_val);
    if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1))
      i++;
  }
}		

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

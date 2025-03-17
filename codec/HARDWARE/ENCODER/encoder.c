/**
   ******************************************************************************
   * @file    encoder.c 
   * @author  
   * @version v1
   * @date    2024/11/18
   * @brief   编码器配置
   ******************************************************************************
**/


#include "encoder.h"


/**
   ******************************************************************************
   * @brief    编码器初始化
   *   
   * @note 	   编码器1                        编码器2
   *           ENC3   ---   PD15              E2-2   ---   PC7             最高位
   *           ENC2   ---   PD14              E2-1   ---   PC6
   *           ENC1   ---   PD13              E1-2   ---   PC5
   *           ENC0   ---   PD12              E1-1   ---   PC4             最低位
   *           
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void ENC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;                        //定义结构体变量

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	     //使能GPIOD时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	     //使能GPIOD时钟

	//GPIOD12,13,14,15初始化设置
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;                //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               //推挽输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;           //100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;            //无上拉下拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);					     //初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);					     //初始化GPIO
}


/**
   ******************************************************************************
   * @brief    读取编码器1的引脚值，编码器1控制音量
   *   
   * @note 	   要进行消抖，消除因为外界因素导致的按键抖动
   *                  
   * @param    None
   *           
   * @retval   None
   ******************************************************************************
**/
uint8_t Read_Encoder1_Value(void) 
{
	uint8_t value = 0;
	
	value |= ( GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14) == Bit_SET ? 0x01 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12) == Bit_SET ? 0x02 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13) == Bit_SET ? 0x04 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15) == Bit_SET ? 0x08 : 0x00 );
	
	return value;
}

/**
   ******************************************************************************
   * @brief    读取编码器2的引脚值，编码器2控制开关和信道
   *   
   * @note 	   要进行消抖，消除因为外界因素导致的按键抖动
   *                  
   * @param    None
   *           
   * @retval   None
   ******************************************************************************
**/
uint8_t Read_Encoder2_Value(void) 
{
	uint8_t value = 0;
	
	value |= ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4) == Bit_SET ? 0x01 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) == Bit_SET ? 0x02 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6) == Bit_SET ? 0x04 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7) == Bit_SET ? 0x08 : 0x00 );
	
	return value;
}

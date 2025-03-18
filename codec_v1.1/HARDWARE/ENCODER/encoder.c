/**
   ******************************************************************************
   * @file    encoder.c 
   * @author  
   * @version v1
   * @date    2024/11/18
   * @brief   ����������
   ******************************************************************************
**/


#include "encoder.h"


/**
   ******************************************************************************
   * @brief    ��������ʼ��
   *   
   * @note 	   ������1                        ������2
   *           ENC3   ---   PD15              E2-2   ---   PC7             ���λ
   *           ENC2   ---   PD14              E2-1   ---   PC6
   *           ENC1   ---   PD13              E1-2   ---   PC5
   *           ENC0   ---   PD12              E1-1   ---   PC4             ���λ
   *           
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void ENC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;                        //����ṹ�����

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	     //ʹ��GPIODʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	     //ʹ��GPIODʱ��

	//GPIOD12,13,14,15��ʼ������
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;                //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;           //100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;            //����������
	GPIO_Init(GPIOD, &GPIO_InitStructure);					     //��ʼ��GPIO
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);					     //��ʼ��GPIO
}


/**
   ******************************************************************************
   * @brief    ��ȡ������1������ֵ��������1��������
   *   
   * @note 	   Ҫ����������������Ϊ������ص��µİ�������
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
   * @brief    ��ȡ������2������ֵ��������2���ƿ��غ��ŵ�
   *   
   * @note 	   Ҫ����������������Ϊ������ص��µİ�������
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

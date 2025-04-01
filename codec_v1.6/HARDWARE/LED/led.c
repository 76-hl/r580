/**
   ******************************************************************************
   * @file    led.c 
   * @author  
   * @version v1
   * @date    
   * @brief   LED��ʼ��
   ******************************************************************************
**/	 


#include "led.h" 


/**
   ******************************************************************************
   * @brief    led�������úͳ�ʼ������ 
   *   
   * @note 	   PG2   ---   LED0
   *           PG3   ---   LED1
   *
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;                        //����ṹ�����

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	     //ʹ��GPIOFʱ��

	//GPIOF9,10��ʼ������
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;	 //LED0��LED1��ӦIO��
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;               //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;           //100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;            //����������
	 
	GPIO_Init(GPIOG, &GPIO_InitStructure);					     //��ʼ��GPIO
	GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);			     //GPIOG2��3���øߣ�����
}







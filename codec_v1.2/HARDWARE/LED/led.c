/**
   ******************************************************************************
   * @file    led.c 
   * @author  
   * @version v1
   * @date    
   * @brief   LED初始化
   ******************************************************************************
**/	 


#include "led.h" 


/**
   ******************************************************************************
   * @brief    led参数配置和初始化函数 
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
	GPIO_InitTypeDef  GPIO_InitStructure;                        //定义结构体变量

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	     //使能GPIOF时钟

	//GPIOF9,10初始化设置
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;	 //LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;               //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;           //100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;            //无上拉下拉
	 
	GPIO_Init(GPIOG, &GPIO_InitStructure);					     //初始化GPIO
	GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);			     //GPIOG2、3设置高，灯灭
}







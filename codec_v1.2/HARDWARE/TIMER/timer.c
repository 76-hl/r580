/**
   ******************************************************************************
   * @file    timer.c 
   * @author  
   * @version v1
   * @date    2024/11/11
   * @brief   定时器配置
   ******************************************************************************
**/


#include "timer.h" 


uint8_t Timer5msFlag = 0;    //定时5ms标志位


/**
   ******************************************************************************
   * @brief    定时器4参数配置和初始化函数 
   *   
   * @note 	   外设开发流程：
   *           1.定义定时器、NVIC外设结构体变量
   *           2.打开外设时钟
   *           3.对串口、NVIC外设结构体变量赋值和初始化
   *           4.定时器使能
   *           中断服务函数
   *
   *           WACK_UP   ---   PA0
   *
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void TIM2_Init(void)
{
	//1.定义定时器外设结构体变量
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef          NVIC_InitStructure;
	
	//2.打开外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//3.对串口、NVIC外设结构体变量赋值和初始化
	TIM_TimeBaseInitStructure.TIM_ClockDivision     = 0;                        //时钟分频，不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode       = TIM_CounterMode_Up;       //递增计数
	TIM_TimeBaseInitStructure.TIM_Period            = 50 - 1;                   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler         = 8400 - 1;                 //预分频器的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                        //重复计数器，高级定时器才有的
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);                          //初始化
	
	NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;           //设置中断通道为USART1_IRQn
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;                   //设置子优先级，优先级的值越低，则优先级越高
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;                   //设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;              //使能中断通道
	NVIC_Init(&NVIC_InitStructure);                                             //初始化
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                                    //使能更新中断
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);                                        //清除标志位，防止一上电就进入中断

	//4.使能定时器
	TIM_Cmd(TIM2,ENABLE);
}


/**
   ******************************************************************************
   * @brief    定时器4中断服务函数 
   *   
   * @note 	   中断服务函数名字是固定的,可以去启动文件startup_stm32f40_41xxx.s查看
   *
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)     //判断是否发生更新中断
	{	
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);     //清除标志位
		
		Timer5msFlag = 1;                              //给定时5ms标志位置1
	}
}


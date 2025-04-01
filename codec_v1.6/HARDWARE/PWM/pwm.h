#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	



#define GENERAL_OCPWM_PIN             GPIO_Pin_0              
#define GENERAL_OCPWM_GPIO_PORT       GPIOB                      
#define GENERAL_OCPWM_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define GENERAL_OCPWM_PINSOURCE				GPIO_PinSource0
#define GENERAL_OCPWM_AF							GPIO_AF_TIM3

#define GENERAL_TIM           		    TIM3
#define GENERAL_TIM_CLK       		    RCC_APB1Periph_TIM3

#define GENERAL_TIM_IRQn					    TIM3_IRQn
#define GENERAL_TIM_IRQHandler        TIM3_IRQHandler

void TIMx_Configuration(u8 state);
#endif

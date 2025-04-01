#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
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

/**
   ******************************************************************************
   * @file    main.c 
   * @author  
   * @version v1
   * @date    
   * @brief   
   ******************************************************************************
**/


#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"


#include "delay.h"
#include "usart.h"
#include "led.h"
#include "nau881x.h"

extern NAU881x_t        nau881x;

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);     //设置系统中断优先级分组4
	
	delay_init(168);		                            //初始化延时函数
	UART1_Init(115200);     	                        //初始化串口
	LED_Init();		                                    //初始化LED端口
	
	while(1)
    {	
	}
}
   

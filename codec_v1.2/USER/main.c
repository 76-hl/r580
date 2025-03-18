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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);     //����ϵͳ�ж����ȼ�����4
	
	delay_init(168);		                            //��ʼ����ʱ����
	UART1_Init(115200);     	                        //��ʼ������
	LED_Init();		                                    //��ʼ��LED�˿�
	
	while(1)
    {	
	}
}
   

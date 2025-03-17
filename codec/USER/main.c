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
#include "codec.h"

extern void SPI3_Init(void);
extern void NAU8814_Init(void);
extern void SPI_WriteReg(uint8_t addr,uint16_t data);


int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);     //����ϵͳ�ж����ȼ�����4
	
	delay_init(168);		                            //��ʼ����ʱ����
	UART1_Init(115200);     	                        //��ʼ������
	LED_Init();		                                    //��ʼ��LED�˿�
	SPI3_Init();

	while(1)
    {	
		delay_ms(100);
		GPIO_ResetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);			     //GPIOG2��3���øߣ�����
		delay_ms(100);
		GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
		SPI_WriteReg(0x2D,0x028);
	}
}
   

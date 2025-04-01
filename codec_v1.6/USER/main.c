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
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_clkconfig.h"
#include "tmk07.h"
#include "key_encoder.h"
#include "dac.h"
#include "pwm.h"
#include "codec.h"

extern void SPI3_Init(void);
extern void NAU8814_Init(void);
extern void SPI_WriteReg(uint8_t addr,uint16_t data);

const char intercom_sending_data[] = { 
    0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x55, 0x0A, 0x0F, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 
	0x17	
};



int main(void)
{
	/*HSI=168Mhz*/
	HSI_SetSysClock(16, 336, 2, 7);//使用内部时钟源，16MHZ
	
	#ifdef bootloader_64K
	/*set nvic address*/
	 __disable_irq();
	 SCB->VTOR = FLASH_BASE | 0X10000;
	 __enable_irq();
	#endif
	
	//sysinit
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	uart_init(115200);     	//初始化串口
    KEY_encoder_Init();
	LED_Init();		        //初始化LED端口
	rf_switch_init();
	oa_switch_init();
	pnc_mic_init();
	pnc_mic_sw(0);//打开降噪
	
	
	SPI3_Init();	
	SPI_WriteReg(0x00,0x000);
	delay_ms(1);
	NAU8814_Init();
    BSP_CodecOpenMicrophone();

	while(1)
    {	
		delay_ms(1000);
		GPIO_ResetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);			     //GPIOG2、3设置高，灯灭
		
		u8 len_a=sizeof(intercom_sending_data);//得到此次接收到的数据长度
		for(u8 ta=0;ta<len_a;ta++)
		{
			USART_SendData(USART3, intercom_sending_data[ta]);         //向串口3发送数据
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
		}
		
		//SPI_WriteReg(0x04,0x118);    //ADC和DAC数据在帧时钟左侧，I2S传输，16位宽，BCLK和FS都为下降沿触发
		//SPI_WriteReg(0x05,0x000);
		//SPI_WriteReg(0x0F,0x0FF);
		
		delay_ms(1000);
		GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
	}
}
   

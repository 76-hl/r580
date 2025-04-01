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
	HSI_SetSysClock(16, 336, 2, 7);//ʹ���ڲ�ʱ��Դ��16MHZ
	
	#ifdef bootloader_64K
	/*set nvic address*/
	 __disable_irq();
	 SCB->VTOR = FLASH_BASE | 0X10000;
	 __enable_irq();
	#endif
	
	//sysinit
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����
	uart_init(115200);     	//��ʼ������
    KEY_encoder_Init();
	LED_Init();		        //��ʼ��LED�˿�
	rf_switch_init();
	oa_switch_init();
	pnc_mic_init();
	pnc_mic_sw(0);//�򿪽���
	
	
	SPI3_Init();	
	SPI_WriteReg(0x00,0x000);
	delay_ms(1);
	NAU8814_Init();
    BSP_CodecOpenMicrophone();

	while(1)
    {	
		delay_ms(1000);
		GPIO_ResetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);			     //GPIOG2��3���øߣ�����
		
		u8 len_a=sizeof(intercom_sending_data);//�õ��˴ν��յ������ݳ���
		for(u8 ta=0;ta<len_a;ta++)
		{
			USART_SendData(USART3, intercom_sending_data[ta]);         //�򴮿�3��������
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
		
		//SPI_WriteReg(0x04,0x118);    //ADC��DAC������֡ʱ����࣬I2S���䣬16λ��BCLK��FS��Ϊ�½��ش���
		//SPI_WriteReg(0x05,0x000);
		//SPI_WriteReg(0x0F,0x0FF);
		
		delay_ms(1000);
		GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
	}
}
   

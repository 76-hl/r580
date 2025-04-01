#include "key_encoder.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pwm.h"
#define exit_switch           7
#define vox_switch            3
#define readchannel_switch    11
#define modifychannel_switch  9
#define readwork_switch       1
#define modifywork_switch     0

//const char intercom_sending_data[] = { 
//    0x00, 0x00, 0x00, 0x00, 0x00, 
//		0x00, 0x00, 0x00, 0x00, 0x00, 
//		0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x55, 0x0A, 0x0F, 0x18,
//    0x00, 0x00, 0x00, 0x00, 0x00, 
//    0x00, 0x00, 0x00, 0x00, 0x00, 
//		0x00, 0x00, 0x00, 0x00, 0x00, 
//		0x17	
//};

const char exit_sending_data[] = { 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x55, 0x0A, 
	0x0F, 0x16, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x19
};

const char vox_workmode_data[] = { 
    0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x55, 0x0A, 
		0x0F, 0x18, 0xAA, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0xBD
};


const char read_channel_para[] = {

    0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x55, 0x0A, 
		0x3F, 0x05, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x3A

};
//set 6channel as 472.53Mhz
const char modify_channel_para[] = {

    0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x55, 0x0A, 
		0xCF, 0x05, 0xF3, 0x43, 0x2A, 
		0x1C, 0x78, 0x03, 0x14, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x23

};

const char read_work_para[] = {

    0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x55, 0x0A, 
		0x03, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x03

};

const char modify_work_para[] = {

    0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x55, 0x0A, 
		0xC3, 0x00, 0x04, 0x10, 0x1E, 
		0x08, 0x78, 0x03, 0x78, 0x03, 
		0x07, 0x00, 0x00, 0x00, 0x00, 
		0x60, 0x02, 0xA4

};

const char send_data_test[] = {

    0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x55, 0x0A, 
		0x55, 0x01, 0x02, 0x03, 0x04, 
		0x05, 0x06, 0x07, 0x08, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x08, 0x55

};

enum encode_08C_state {
    exit_mode,    // 0
    vox_mode,    // 1
    read_channel_para_mode,   // 2
    modify_channel_para_mode, // 3
    read_work_para_mode,  // 4
    modify_work_para_mode,    //5
    
};

enum encode_08C_state encode_state;

typedef enum
{

  RF_POWER_ON,
	RF_POWER_OFF,
}__attribute__((packed))   RF_power_state;


//RF供电引脚初始化，默认打开RF供电
void rf_switch_init(void)
{

  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOA,GPIOE时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9 | GPIO_Pin_10; //KEY0 对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOE4 
	GPIO_SetBits(GPIOB,GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
}
//运放供电
//当switch_rf为true时打开射频供电开关,且返回0
u8  RF_POWERR(u8 switch_rf)
{
   if(switch_rf)
	 {
		 GPIO_SetBits(GPIOB,GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
	   return RF_POWER_ON;
	 }
	 else
	 {
		 GPIO_ResetBits(GPIOB,GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
	 	 return RF_POWER_OFF;
	 }

}




//运放供电初始化，射频功率控制高有效
void oa_switch_init(void)
{

  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA,GPIOE时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; //KEY0 对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOE4 
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
}


//如果初始化
void pnc_mic_init(void)
{

  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOA,GPIOE时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //KEY0 对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOE4 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOE4 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIOF13  降噪LINK高有效
	
	//GPIO_SetBits(GPIOB,GPIO_Pin_12)

}

//state=1为直通模式，否则为降噪模式,只有在降噪模式下，音频提示音和功放才会打开
u8 pnc_mic_sw(u8 state)
{
  if(state)
	{
	
	  GPIO_SetBits(GPIOB,GPIO_Pin_12);//拉高为直通模式
	  GPIO_ResetBits(GPIOD,GPIO_Pin_2|GPIO_Pin_0|GPIO_Pin_1);//拉高为直通模式   关闭PNC的供电
		
		GPIO_ResetBits(GPIOF,GPIO_Pin_12);
		return 1;
	
	}
else
  {

    GPIO_ResetBits(GPIOB,GPIO_Pin_12);//拉低为降噪模式
		GPIO_SetBits(GPIOD,GPIO_Pin_2 |GPIO_Pin_0 | GPIO_Pin_1);//音频功放，PNC供电控制，提示音控制输出
		GPIO_SetBits(GPIOF,GPIO_Pin_12);//降噪链接
		
		return 0;
  }
	


}


u8 intercon_on_off=0;
void KEY_encoder_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //KEY0 对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//下拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE4 
	
} 



void KEY_TEST(void)
{
	
		u8 len,len_a;
	  u8 t,ta;

		if(PTT==0&&intercon_on_off==0)
		{
			
				TIMx_Configuration(1);
      len=sizeof(send_data_test);//得到此次接收到的数据长度
	
			for(t=0;t<len;t++)
			{
				USART_SendData(USART3, send_data_test[t]);         //向串口3发送数据
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
			}
			GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
	
			intercon_on_off=1;
		}
	 	if(PTT==1&&intercon_on_off==1)
		{
			TIMx_Configuration(0);
			 len_a=sizeof(exit_sending_data);//得到此次接收到的数据长度
	
			for(ta=0;ta<len_a;ta++)
			{
				USART_SendData(USART3, exit_sending_data[ta]);         //向串口3发送数据
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
			}
			GPIO_ResetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
			intercon_on_off=0;
			
		}
   

}

void ENC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;                        

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	     
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	     

	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;                
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;           
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;            
	GPIO_Init(GPIOD, &GPIO_InitStructure);					     
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);					     
}


uint8_t Read_Encoder_channel_Value(void) 
{
	uint8_t value = 0;
	
	value |= ( GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14) == Bit_SET ? 0x01 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12) == Bit_SET ? 0x02 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13) == Bit_SET ? 0x04 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15) == Bit_SET ? 0x08 : 0x00 );
	
	return value;
}

uint8_t Read_Encoder_pw_voice_Value(void) 
{
	uint8_t value = 0;
	
	value |= ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4) == Bit_SET ? 0x01 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) == Bit_SET ? 0x02 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6) == Bit_SET ? 0x04 : 0x00 );
	value |= ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7) == Bit_SET ? 0x08 : 0x00 );
	
	return value;
}

u8 intercon_on_off_a=0;
void encoder_channel(void)
{

  u8 len,t;
	u8 channel_encoede_value=Read_Encoder_channel_Value();
	switch(channel_encoede_value)
{
		
		case exit_switch:                                //当编码器=7时，
			if(encode_state!=exit_mode)
			{
			
				GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
				vTaskDelay(50);
				GPIO_ResetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
				len=sizeof(exit_sending_data);
					for(t=0;t<len;t++)
				{
					USART_SendData(USART3, exit_sending_data[t]);         //向串口3发送数据
					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
				}
				encode_state=exit_mode;
		}
			break;
			
			
		case vox_switch:                                   //当编码器=3时
		if(encode_state!=vox_mode)
			{
			
					GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
					vTaskDelay(50);
					GPIO_ResetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
					len=sizeof(vox_workmode_data);
						for(t=0;t<len;t++)
					{
						USART_SendData(USART3, vox_workmode_data[t]);         //向串口3发送数据
						while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
					}
					encode_state=vox_mode;
		 }
			break;			
				case readchannel_switch:
		if(encode_state!=read_channel_para_mode)
			{
			
					GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
					vTaskDelay(50);
					GPIO_ResetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
					len=sizeof(read_channel_para);
						for(t=0;t<len;t++)
					{
						USART_SendData(USART3, read_channel_para[t]);         //向串口3发送数据
						while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
					}
					encode_state=read_channel_para_mode;
		 }
			break;
			
			
				
		case modifychannel_switch:
		if(encode_state!=modify_channel_para_mode)
			{
			
					GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
					vTaskDelay(50);
					GPIO_ResetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
					len=sizeof(modify_channel_para);
						for(t=0;t<len;t++)
					{
						USART_SendData(USART3, modify_channel_para[t]);         //向串口3发送数据
						while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
					}
					encode_state=modify_channel_para_mode;
		 }
			break;								
		case readwork_switch:
		if(encode_state!=read_work_para_mode)
			{
			
					GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
					vTaskDelay(50);
					GPIO_ResetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
					len=sizeof(read_work_para);
						for(t=0;t<len;t++)
					{
						USART_SendData(USART3, read_work_para[t]);         //向串口3发送数据
						while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
					}
					encode_state=read_work_para_mode;
		 }
			break;					
		case modifywork_switch:
		if(encode_state!=modify_work_para_mode)
			{
			
					GPIO_SetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
					vTaskDelay(50);
					GPIO_ResetBits(GPIOG,GPIO_Pin_2 | GPIO_Pin_3);
					len=sizeof(modify_work_para);
						for(t=0;t<len;t++)
					{
						USART_SendData(USART3, modify_work_para[t]);         //向串口3发送数据
						while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
					}
					encode_state=modify_work_para_mode;
		 }
			break;
			
	
	}
	
	
}



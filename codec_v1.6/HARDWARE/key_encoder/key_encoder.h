#ifndef __KEY_ENCODER_H
#define __KEY_ENCODER_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 

//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#define   PTT 	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9) //PE4

void encoder_channel(void);
void rf_switch_init(void); //射频开关高有效 初始化默认拉高

void    ENC_Init(void);  
uint8_t Read_Encoder_channel_Value(void);
uint8_t Read_Encoder_pw_voice_Value(void);

void KEY_encoder_Init(void);	//IO初始化
void KEY_TEST(void);
void oa_switch_init(void);
void pnc_mic_init(void);
u8 pnc_mic_sw(u8 state);

extern const char exit_sending_data[];
extern const char intercom_sending_data[];
extern const char QUERY_MODE_data[];




#endif


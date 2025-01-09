/**
   ******************************************************************************
   * @file    usart.h 
   * @author  
   * @version v1
   * @date    2024/10/31
   * @brief   usart头文件和函数声明
   ******************************************************************************
**/

#ifndef _USART_H
#define _USART_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "delay.h"
#include "sys.h"


#define  LEN 256                                                               //数组长度


void USART1_Config(uint32_t baud);                                              //串口1配置和使能
void USART2_Config(uint32_t baud);                                              //串口2配置和使能
void USART_SendBytes(USART_TypeDef *USARTx,uint8_t *Data,uint32_t DataLen);     //串口发送字节指令
char * USART_GetData(void);                                                     //获取串口数据
bool USART_CheakData(void);                                                     //校验函数






#define UART_Data_Null 0x00
#define UART_Data_Sync 0x55
#define UART_Data_Head 0x0A

#define UART_Control_Command 0x0F
#define UART_Control_SubCommand_Get 0x11
#define UART_Control_SubCommand_Report 0x12
#define UART_Control_SubCommand_PTT 0x18
#define UART_Control_SubCommand_Wait 0x16
#define UART_Control_SubCommand_FAULT 0xF5
#define UART_Control_SubCommand_V_UP 0x31
#define UART_Control_SubCommand_V_DOWN 0x32
#define UART_Control_SubCommand_CH_UP 0x34
#define UART_Control_SubCommand_CH_DOWN 0x37
#define UART_Control_SubCommand_Battery_Alarm 0x39
#define UART_Control_SubCommand_Battery_Normal 0x38
#define UART_Control_SubCommand_Speak 0x3B
#define UART_Control_SubCommand_PacketStatus 0x3C
#define UART_Control_SubCommand_VGA_Set 0x41
#define UART_Control_SubCommand_VGA_Get 0x42
#define UART_Control_SubCommand_Limit_Set 0x44
#define UART_Control_SubCommand_Limit_Get 0x48
#define UART_Control_SubCommand_TXen_Set 0x81
#define UART_Control_SubCommand_TXen_Get 0x82
#define UART_Control_SubCommand_Test_Enable 0xEC
#define UART_Control_SubCommand_Test_Disable 0xDC
#define UART_Control_SubCommand_Save 0xDD
#define UART_Control_SubCommand_Stop 0x00
#define UART_Control_SubCommand_Exit 0xFF
#define UART_Control_SubCommand_Default 0xDF
#define UART_Control_SubCommand_USB_Open 0xC1
#define UART_Control_SubCommand_USB_Close 0xC0
#define UART_Control_SubCommand_DMIC_SEL 0xC8

#define UART_Control_Command_Mode_Set 0xC3
#define UART_Control_Command_Mode_Get 0x03
#define UART_Control_Command_CHN_Set 0xCF
#define UART_Control_Command_CHN_Get 0x3F
#define UART_Control_Command_KEY_Set 0xCA
#define UART_Control_Command_KEY_Get 0x3A
#define UART_Control_Command_CODE_Set 0xA5


union UART_Command_Def{
	unsigned char Fields[16];

	//子命令
	struct{
	  unsigned char Sub_Command;		//0
	  unsigned char Append[15];		    //1, 
	}MK_Command;

	//工作模式数据结构
	struct{
	  unsigned char Standard;		    //0,High Half byte:0 compatible mode；1：Long range
	  unsigned char OpMode;		        //1,  Low half byte:0:VOX&PTT, 1:PTT only, 2:BT TX
	  unsigned char Language;           //3
	  unsigned char ChannelMax;		    //4
	  unsigned char LocalChannel;       //5
	  unsigned char Group[2];		    //6,7
	  unsigned char LocalNum[2];	    //8,9
	  unsigned char Priority;		    //10
	}MK_Mode;

	//信道参数数据结构
	struct{
	  unsigned char Channel;            //1~4
	  unsigned char Freq[4];            //1~4
	  unsigned char Target[2];		    //5~6
	  unsigned char TXPower;            //7
	}MK_Channel;

	//密钥参数数据结构
	struct{
	  unsigned char ConnectCode[4];    //1,2,3,4
	  unsigned char SuperKEY[4];       // 5,6,7,8
	}MK_Key;

	//VOX参数数据结构
	struct{
	  unsigned char Sub_Command;	   //0
	  unsigned char UsedChannel;	   //1, 
	  unsigned char TX_Voice_Mode;	   //2, 
	}MK_PTT_Command;

	struct{
	  unsigned char Null[16];          //1~4
	}MK_Empty;
};





#endif

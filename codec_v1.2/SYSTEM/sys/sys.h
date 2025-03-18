#ifndef __SYS_H
#define __SYS_H

#include "stm32f4xx.h" 
#include <stdbool.h>


//1,支持os;   0,不支持os
#define SYSTEM_SUPPORT_OS		1		//定义系统文件夹是否支持OS
																	    
	 
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入



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



//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址 
#endif












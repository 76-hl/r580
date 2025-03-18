/**
   ******************************************************************************
   * @file    usart.h 
   * @author  
   * @version v1
   * @date    
   * @brief   usart头文件和函数声明
   ******************************************************************************
**/

#ifndef __USART_H
#define __USART_H

#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "stdio.h"	
#include "string.h"
 	
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口3接收

#define  LEN 1024                                                               //数组长度
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

void UART1_Init(u32 bound);              //串口初始化函数
void USART_SendBytes(USART_TypeDef *USARTx,uint8_t *data,uint32_t len);         //串口发送字节指令
char * USART_GetData(void);                                                     //获取串口数据
bool USART_CheakData(void);
#endif



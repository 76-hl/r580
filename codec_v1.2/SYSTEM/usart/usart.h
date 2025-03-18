/**
   ******************************************************************************
   * @file    usart.h 
   * @author  
   * @version v1
   * @date    
   * @brief   usartͷ�ļ��ͺ�������
   ******************************************************************************
**/

#ifndef __USART_H
#define __USART_H

#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "stdio.h"	
#include "string.h"
 	
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������3����

#define  LEN 1024                                                               //���鳤��
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

void UART1_Init(u32 bound);              //���ڳ�ʼ������
void USART_SendBytes(USART_TypeDef *USARTx,uint8_t *data,uint32_t len);         //���ڷ����ֽ�ָ��
char * USART_GetData(void);                                                     //��ȡ��������
bool USART_CheakData(void);
#endif



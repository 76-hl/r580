/**
   ******************************************************************************
   * @file    delay.h 
   * @author  
   * @version v1
   * @date    2024/10/30
   * @brief   delayͷ�ļ��ͺ�������
   ******************************************************************************
**/


#ifndef _DELAY_H
#define _DELAY_H

#include "stm32f4xx.h"
#include "sys.h"

void delay_init(uint16_t sysclk);   //��ʼ���ӳٺ���
void delay_us(uint32_t us);         //΢����ʱ����
void delay_ms(uint32_t ms);         //������ʱ����

#endif

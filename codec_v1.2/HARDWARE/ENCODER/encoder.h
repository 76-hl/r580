/**
   ******************************************************************************
   * @file    key.h 
   * @author  
   * @version v1
   * @date    2024/11/18
   * @brief   keyͷ�ļ��ͺ�������
   ******************************************************************************
**/

#ifndef _ENCODER_H
#define _ENCODER_H

#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "timer.h"

void    ENC_Init(void);               //��������ʼ��
uint8_t Read_Encoder1_Value(void);    //��ȡ������1��ֵ
uint8_t Read_Encoder2_Value(void);    //��ȡ������2��ֵ

#endif

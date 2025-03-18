/**
   ******************************************************************************
   * @file    key.h 
   * @author  
   * @version v1
   * @date    2024/11/18
   * @brief   key头文件和函数声明
   ******************************************************************************
**/

#ifndef _ENCODER_H
#define _ENCODER_H

#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "timer.h"

void    ENC_Init(void);               //编码器初始化
uint8_t Read_Encoder1_Value(void);    //读取编码器1的值
uint8_t Read_Encoder2_Value(void);    //读取编码器2的值

#endif

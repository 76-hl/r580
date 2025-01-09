/**
   ******************************************************************************
   * @file    delay.h 
   * @author  
   * @version v1
   * @date    2024/10/30
   * @brief   delay头文件和函数声明
   ******************************************************************************
**/


#ifndef _DELAY_H
#define _DELAY_H

#include "stm32f4xx.h"
#include "sys.h"

void delay_init(uint16_t sysclk);   //初始化延迟函数
void delay_us(uint32_t us);         //微秒延时函数
void delay_ms(uint32_t ms);         //毫秒延时函数

#endif

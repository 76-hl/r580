/**
   ******************************************************************************
   * @file    init.h 
   * @author  
   * @version v1
   * @date    2024/11/18
   * @brief   init头文件和函数声明
   ******************************************************************************
**/

#ifndef _KEY_H
#define _KEY_H

#include "stm32f4xx.h"

#include "timer.h"
#include "usart.h"


void IO_Config(void);       //IO口初始化
void SYSTEM_Init(void);     //初始化函数


#endif


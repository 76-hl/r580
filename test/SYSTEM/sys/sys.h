/**
   ******************************************************************************
   * @file    sys.h 
   * @author  
   * @version v1
   * @date    2024/11/21
   * @brief   指令宏定义和参数数据结构
   ******************************************************************************
**/

#ifndef _SYS_H
#define _SYS_H

#include "stm32f4xx.h"

#define SYSTEM_SUPPORT_OS    1

void WFI_SET(void);         //执行WFI指令
void INTX_DISABLE(void);    //关闭所有中断
void INTX_ENABLE(void);         //开启所有中断
void MSR_MSP(u32 addr);     //设置堆栈地址

#endif

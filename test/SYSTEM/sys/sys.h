/**
   ******************************************************************************
   * @file    sys.h 
   * @author  
   * @version v1
   * @date    2024/11/21
   * @brief   ָ��궨��Ͳ������ݽṹ
   ******************************************************************************
**/

#ifndef _SYS_H
#define _SYS_H

#include "stm32f4xx.h"

#define SYSTEM_SUPPORT_OS    1

void WFI_SET(void);         //ִ��WFIָ��
void INTX_DISABLE(void);    //�ر������ж�
void INTX_ENABLE(void);         //���������ж�
void MSR_MSP(u32 addr);     //���ö�ջ��ַ

#endif

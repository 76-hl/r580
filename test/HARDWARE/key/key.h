/**
   ******************************************************************************
   * @file    key.h 
   * @author  
   * @version v1
   * @date    2024/11/18
   * @brief   keyͷ�ļ��ͺ�������
   ******************************************************************************
**/

#ifndef _KEY_H
#define _KEY_H

#include "stm32f4xx.h"
#include "delay.h"
#include "timer.h"


#define KEY1STATE   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEY0STATE   GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)


#define SHORT_PRESS_TIME     100             //�̰���ʱ����ֵ����λms
#define LONG_PRESS_TIME      2000            //������ʱ����ֵ����λms
#define MULTI_PRESS_INTERVAL 300             //��ΰ��µ������ (��λ: ms)


typedef enum {
    KEY_IDLE,                                //��������
    KEY_PRESS,                               //��������
} KeyState;


typedef struct {
    uint8_t  State;                          //������ǰ״̬
    uint16_t PressTime;                      //��������ʱ����ms��
    uint8_t  ClickCount;                     //�����������
    uint16_t ClickInterval;                  //���ζ̰������ʱ��ms��
} Key_t;


void Key_Scan(void);                         //����ɨ�躯��
void SendMessage(unsigned char Command);     //�������ݺ���
void Volume_UpPressed(void);                 //�����Ӱ�������
void Volume_DownPressed(void);
void Channel_UpPressed(void);
void Channel_DownPressed(void);
void Voice_Broadcast(char *Data);


#endif

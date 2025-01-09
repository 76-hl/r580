/**
   ******************************************************************************
   * @file    key.h 
   * @author  
   * @version v1
   * @date    2024/11/18
   * @brief   key头文件和函数声明
   ******************************************************************************
**/

#ifndef _KEY_H
#define _KEY_H

#include "stm32f4xx.h"
#include "delay.h"
#include "timer.h"


#define KEY1STATE   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEY0STATE   GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)


#define SHORT_PRESS_TIME     100             //短按的时间阈值，单位ms
#define LONG_PRESS_TIME      2000            //长按的时间阈值，单位ms
#define MULTI_PRESS_INTERVAL 300             //多次按下的最大间隔 (单位: ms)


typedef enum {
    KEY_IDLE,                                //按键空闲
    KEY_PRESS,                               //按键按下
} KeyState;


typedef struct {
    uint8_t  State;                          //按键当前状态
    uint16_t PressTime;                      //按键按下时长（ms）
    uint8_t  ClickCount;                     //按键点击次数
    uint16_t ClickInterval;                  //两次短按间隔计时（ms）
} Key_t;


void Key_Scan(void);                         //按键扫描函数
void SendMessage(unsigned char Command);     //发送数据函数
void Volume_UpPressed(void);                 //音量加按键操作
void Volume_DownPressed(void);
void Channel_UpPressed(void);
void Channel_DownPressed(void);
void Voice_Broadcast(char *Data);


#endif

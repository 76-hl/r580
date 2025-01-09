/**
   ******************************************************************************
   * @file    init.c 
   * @author  
   * @version v1
   * @date    2024/11/11
   * @brief   IO口初始化
   ******************************************************************************
**/


#include "init.h"


/**
   ******************************************************************************
   * @brief    IO口配置和初始化函数 
   *   
   * @note 	   外设开发流程：
   *           1.定义外设结构体变量
   *           2.打开外设时钟
   *           3.对外设的结构体变量赋值
   *           4.初始化外设
   *
   *           PF9   ---   LED0
   *           PF10  ---   LED1
   *           PA0   ---   KEY_UP
   *           PE4   ---   KEY0
   *
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void IO_Config(void)
{
	//1.定义GPIO外设的结构体变量
	GPIO_InitTypeDef   GPIO_InitStructure;
	
	//2.打开端口A、E、F的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//3.配置结构体变量
	
	//led灯IO口配置
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;   //PF9 和 PF10
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;              //输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;          //高速
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //推挽
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;           //无上拉下拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);                      //初始化端口
	
	//按键IO口配置
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;                 //PA0
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;               //输入模式
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;               //上拉
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);                      //初始化端口
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;                 //PE4
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;               //输入模式
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;               //上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);                      //初始化端口
	
	//设置灯的初始状态为不亮，高电平不亮
	GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);
}


/**
   ******************************************************************************
   * @brief    初始化函数
   *   
   * @note 	   外设和IO口的初始化
   *                  
   * @param    None
   *           
   * @retval   None
   ******************************************************************************
**/
void SYSTEM_Init(void)
{
	IO_Config();               //IO口初始化
	
	USART1_Config(115200);     //串口1初始化
	
	USART2_Config(115200);     //串口2初始化
	
	Timer_Config();            //定时器初始化
}


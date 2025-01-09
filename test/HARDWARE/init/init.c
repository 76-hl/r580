/**
   ******************************************************************************
   * @file    init.c 
   * @author  
   * @version v1
   * @date    2024/11/11
   * @brief   IO�ڳ�ʼ��
   ******************************************************************************
**/


#include "init.h"


/**
   ******************************************************************************
   * @brief    IO�����úͳ�ʼ������ 
   *   
   * @note 	   ���迪�����̣�
   *           1.��������ṹ�����
   *           2.������ʱ��
   *           3.������Ľṹ�������ֵ
   *           4.��ʼ������
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
	//1.����GPIO����Ľṹ�����
	GPIO_InitTypeDef   GPIO_InitStructure;
	
	//2.�򿪶˿�A��E��F��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//3.���ýṹ�����
	
	//led��IO������
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;   //PF9 �� PF10
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;              //���ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;          //����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;           //����������
	GPIO_Init(GPIOF, &GPIO_InitStructure);                      //��ʼ���˿�
	
	//����IO������
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;                 //PA0
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;               //����ģʽ
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;               //����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);                      //��ʼ���˿�
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;                 //PE4
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;               //����ģʽ
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;               //����
	GPIO_Init(GPIOE, &GPIO_InitStructure);                      //��ʼ���˿�
	
	//���õƵĳ�ʼ״̬Ϊ�������ߵ�ƽ����
	GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);
}


/**
   ******************************************************************************
   * @brief    ��ʼ������
   *   
   * @note 	   �����IO�ڵĳ�ʼ��
   *                  
   * @param    None
   *           
   * @retval   None
   ******************************************************************************
**/
void SYSTEM_Init(void)
{
	IO_Config();               //IO�ڳ�ʼ��
	
	USART1_Config(115200);     //����1��ʼ��
	
	USART2_Config(115200);     //����2��ʼ��
	
	Timer_Config();            //��ʱ����ʼ��
}


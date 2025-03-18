/**
   ******************************************************************************
   * @file    timer.c 
   * @author  
   * @version v1
   * @date    2024/11/11
   * @brief   ��ʱ������
   ******************************************************************************
**/


#include "timer.h" 


uint8_t Timer5msFlag = 0;    //��ʱ5ms��־λ


/**
   ******************************************************************************
   * @brief    ��ʱ��4�������úͳ�ʼ������ 
   *   
   * @note 	   ���迪�����̣�
   *           1.���嶨ʱ����NVIC����ṹ�����
   *           2.������ʱ��
   *           3.�Դ��ڡ�NVIC����ṹ�������ֵ�ͳ�ʼ��
   *           4.��ʱ��ʹ��
   *           �жϷ�����
   *
   *           WACK_UP   ---   PA0
   *
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void TIM2_Init(void)
{
	//1.���嶨ʱ������ṹ�����
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef          NVIC_InitStructure;
	
	//2.������ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//3.�Դ��ڡ�NVIC����ṹ�������ֵ�ͳ�ʼ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision     = 0;                        //ʱ�ӷ�Ƶ������Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode       = TIM_CounterMode_Up;       //��������
	TIM_TimeBaseInitStructure.TIM_Period            = 50 - 1;                   //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler         = 8400 - 1;                 //Ԥ��Ƶ����ֵ
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                        //�ظ����������߼���ʱ�����е�
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);                          //��ʼ��
	
	NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;           //�����ж�ͨ��ΪUSART1_IRQn
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;                   //���������ȼ������ȼ���ֵԽ�ͣ������ȼ�Խ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;                   //������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;              //ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);                                             //��ʼ��
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                                    //ʹ�ܸ����ж�
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);                                        //�����־λ����ֹһ�ϵ�ͽ����ж�

	//4.ʹ�ܶ�ʱ��
	TIM_Cmd(TIM2,ENABLE);
}


/**
   ******************************************************************************
   * @brief    ��ʱ��4�жϷ����� 
   *   
   * @note 	   �жϷ����������ǹ̶���,����ȥ�����ļ�startup_stm32f40_41xxx.s�鿴
   *
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)     //�ж��Ƿ��������ж�
	{	
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);     //�����־λ
		
		Timer5msFlag = 1;                              //����ʱ5ms��־λ��1
	}
}


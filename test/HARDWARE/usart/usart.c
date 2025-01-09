/**
   ******************************************************************************
   * @file    usart.c 
   * @author  
   * @version v1
   * @date    2024/10/31
   * @brief   �������ú��жϷ�����
   ******************************************************************************
**/


#include "usart.h"


uint8_t  USART1_RxBuf[LEN] = {0};   //����1�Ľ�������
uint32_t USART1_RxCnt      =  0;    //����1�Ľ��ռ�����

uint8_t  USART1_TxBuf[LEN] = {0};   //����1�ķ�������
uint32_t USART1_TxCnt      =  0;    //����1�ķ��ͼ�����

uint8_t  USART2_RxBuf[LEN] = {0};   //����2�Ľ�������
uint32_t USART2_RxCnt      =  0;    //����2�Ľ��ռ�����

uint8_t  USART2_TxBuf[LEN] = {0};   //����2�ķ�������
uint32_t USART2_TxCnt      =  0;    //����2�ķ��ͼ�����


/**
   ******************************************************************************
   * @brief    Ϊ����KEIL��ʹ��printf
   *   
   * @note 	   ������û�м�����ʾ��,ʹ��printf��ӡ������,������Ҫ��printf�����ض�
   *           ��,��Ϊprintf�����ڲ������fputc����,��fputc�������¶�����ǰ�fputc
   *           ��������ַ���Ŀ�ĵ��޸�Ϊ���ڽӿ�,����ʵ����keil��ʹ��printf�ڴ���
   *           ���������ϴ�ӡ
   *           
   *           Cortex M3��M4Ӣ�ĵ��ں��ĵ�18.2.2����˵��
   ******************************************************************************
**/
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) 
{
    USART_SendData(USART1,ch);                                      //����һ���ַ�
    while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET );   //�ȴ����ֽڷ�����ɺ󣬲ſ��Է�����һ���ֽ�
    return (ch);
}

void _sys_exit(int return_code) 
{

}



/**
   ******************************************************************************
   * @brief    USART1�������úͳ�ʼ������ 
   *   
   * @note 	   ���迪�����̣�
   *           1.����GPIO�����ڡ�NVIC����ṹ�����
   *           2.������ʱ��
   *           3.ѡ��IO�ڸ��õĹ���
   *           4.��GPIO�����ڡ�NVIC����ṹ�������ֵ�ͳ�ʼ��
   *           5.ѡ�񴮿��ж�Դ
   *           6.����ʹ��
   *           7.�жϷ�����
   *
   *           USART1_TX   ---   PA9
   *           USART1_RX   ---   PA10
   *
   * @param    ������
   *
   * @retval   None
   ******************************************************************************
**/
void USART1_Config(uint32_t baud)
{
	//1.����GPIO�����ڡ�NVIC����Ľṹ�����
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	//2.������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//3.ѡ��IO�ڸ��õĹ���
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//4.����GPIO�ṹ������ͳ�ʼ��
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;                           //PA9 �� PA10
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                                       //����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                  //����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                      //����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                                       //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);                                              //��ʼ��
	
	//5.���ô��ڽṹ������ͳ�ʼ��
	USART_InitStructure.USART_BaudRate            = baud;                               //�����ʣ�����
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;                //����λ��8bit
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;                   //ֹͣλ��1bit
	USART_InitStructure.USART_Parity              = USART_Parity_No;                    //У��λ����
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //������
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;      //ȫ˫��
	USART_Init(USART1, &USART_InitStructure);                                           //��ʼ��
	
	//6.����NVIC�ṹ������ͳ�ʼ��
	NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;                 //�����ж�ͨ��ΪUSART1_IRQn
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;                           //���������ȼ������ȼ���ֵԽ�ͣ������ȼ�Խ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;                           //������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;                      //ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);                                                     //��ʼ��
	
	//7.ѡ�񴮿��ж�Դ�����յ����ݵ�ʱ�򴥷��жϣ������жϷ�����
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//8.����ʹ��
	USART_Cmd(USART1,ENABLE);
}



/**
   ******************************************************************************
   * @brief    USART1�жϷ����� 
   *   
   * @note 	   �жϷ����������ǹ̶���,����ȥ�����ļ�startup_stm32f40_41xxx.s�鿴
   *
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void USART1_IRQHandler(void)
{	
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)            //�ж��Ƿ���յ�����
	{
		USART1_RxBuf[USART1_RxCnt++] = USART_ReceiveData(USART1);   //�ѽ��յ������ݴ洢�ڴ���1����������
		
		if(USART1_RxCnt >= LEN)                                     //������������������������������
		{
			USART1_RxCnt = 0;
		}
	}
}



/**
   ******************************************************************************
   * @brief    USART2�������úͳ�ʼ������ 
   *   
   * @note 	   ���迪�����̣�
   *           1.����GPIO�����ڡ�NVIC����ṹ�����
   *           2.������ʱ��
   *           3.ѡ��IO�ڸ��õĹ���
   *           4.��GPIO�����ڡ�NVIC����ṹ�������ֵ�ͳ�ʼ��
   *           5.ѡ�񴮿��ж�Դ
   *           6.����ʹ��
   *           7.�жϷ�����
   *
   *           USART2_TX   ---   PA2
   *           USART2_RX   ---   PA3
   *
   * @param    ������
   *
   * @retval   None
   ******************************************************************************
**/
void USART2_Config(uint32_t baud)
{
	//1.����GPIO�����ڡ�NVIC����Ľṹ�����
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	//2.������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);      //����2��APB1������
	
	//3.ѡ��IO�ڸ��õĹ���
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//4.����GPIO�ṹ������ͳ�ʼ��
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;                            //PA2 �� PA3
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                                       //����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                  //����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                      //����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                                       //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);                                              //��ʼ��
	
	//5.���ô��ڽṹ������ͳ�ʼ��
	USART_InitStructure.USART_BaudRate            = baud;                               //�����ʣ�����
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;                //����λ��8bit
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;                   //ֹͣλ��1bit
	USART_InitStructure.USART_Parity              = USART_Parity_No;                    //У��λ����
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //������
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;      //ȫ˫��
	USART_Init(USART2, &USART_InitStructure);                                           //��ʼ��
	
	//6.����NVIC�ṹ������ͳ�ʼ��
	NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;                 //�����ж�ͨ��ΪUSART2_IRQn
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;                           //���������ȼ������ȼ���ֵԽ�ͣ������ȼ�Խ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;                           //������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;                      //ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);                                                     //��ʼ��
	
	//7.ѡ�񴮿��ж�Դ�����յ����ݵ�ʱ������жϣ������жϷ�����
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	//8.����ʹ��
	USART_Cmd(USART2,ENABLE);
}



/**
   ******************************************************************************
   * @brief    USART2�жϷ����� 
   *   
   * @note 	   �жϷ����������ǹ̶���,����ȥ�����ļ�startup_stm32f40_41xxx.s�鿴
   *
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void USART2_IRQHandler(void)
{	
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)            //�ж��Ƿ���յ�����
	{
		USART2_RxBuf[USART2_RxCnt++] = USART_ReceiveData(USART2);   //�ѽ��յ������ݴ洢�ڴ���2����������
		
		if(USART2_RxCnt >= LEN)                                     //������������������������������
		{
			USART2_RxCnt = 0;
		}
	}
}



/**
   ******************************************************************************
   * @brief    USART�����ֽں��� 
   *   
   * @note 	   ���ڷ������ݵĺ���һ��ֻ�ܴ���һ���ֽڵ����ݣ�����ָ��϶��Ǵ���һ
   *           ���ֽڵģ�������Ҫ����ѭ��ȥ��������
   *            
   * @param    ��һ���������ĸ�����Ҫ��������
   *           �ڶ���������Ҫ���͵�����
   *           ������������Ҫ�������ݵĳ���
   *
   * @retval   None
   ******************************************************************************
**/
void USART_SendBytes(USART_TypeDef *USARTx,uint8_t *Data,uint32_t DataLen)
{
	while(DataLen--)
	{
		USART_SendData(USARTx,*Data++);
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
	}
}



/**
   ******************************************************************************
   * @brief    ��ȡ���ڽ��յ�������
   *   
   * @note 	   Ϊ�˿������ļ�����Ҳ��ʹ�������е����� 
   *                  
   * @param    None
   *           
   * @retval   ����2���յ�������
   ******************************************************************************
**/
char *USART_GetData(void)
{	
	return (char *)USART2_RxBuf;
}



/**
   ******************************************************************************
   * @brief    У�麯��
   *   
   * @note 	   У����յ����ݸ�ʽ�Ƿ���ȷ
   *                  
   * @param    None
   *           
   * @retval   None
   ******************************************************************************
**/
bool USART_CheakData(void)
{
	uint8_t Check = 0;
	
	uint32_t Len = sizeof(USART2_RxBuf);
	
	for(uint32_t i=0; i<Len; i++)
	{
		if(USART2_RxBuf[i] == UART_Data_Null)
		{
			continue;
		}
		
		for(int j=i+2;j<i+17;j++)
		{
			Check ^= USART2_RxBuf[j];
		}
		
		if(Check == USART2_RxBuf[i+19])
		{
			if(USART2_RxBuf[i++] == UART_Data_Sync)
			{
				if(USART2_RxBuf[i++] == UART_Data_Head)
				{
					if(i+20>Len)
					{
						return true;
					}
					else if(i+20<Len && USART2_RxBuf[i+20] == UART_Data_Null)
					{
						return true;
					}
					else if(i+20<Len && USART2_RxBuf[i+20] != UART_Data_Null)
					{
						i+=17;
					}
				}
			}
		}	
		
	}
	
	return false; 
}


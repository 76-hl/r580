/**
   ******************************************************************************
   * @file    usart.c 
   * @author  
   * @version v1
   * @date    
   * @brief   �������ú��жϷ�����
   ******************************************************************************
**/


#include "usart.h"	

#if EN_USART1_RX
uint8_t  USART1_RX_BUF[LEN] = {0};    //UART1�Ľ��ջ�����
uint32_t USART1_RX_CNT      = 0;      //UART1�Ľ��ջ������ļ�����
uint16_t USART_RX_STA       = 0;      //����״̬���
#endif
	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOSʹ��	  
#endif
 	  

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
//Cortex M3��M4Ӣ�ĵ��ں��ĵ�18.2.2����˵��
#if 1
#pragma import(__use_no_semihosting)   

//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
}

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 		
	while((USART1->SR & 0X40) == 0);   //ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}

#endif
 

//���ʹ���˽���
#if EN_USART1_RX   
 
/**
   ******************************************************************************
   * @brief    USART1��ʼ������ 
   *   
   * @note 	   ��ȡUSARTx->SR�ܱ���Ī������Ĵ���
   *           USART1_TX   ---   PA9
   *           USART1_RX   ---   PA10
   *
   *           ����״̬
   *           bit15��	������ɱ�־
   *           bit14��	���յ�0x0d
   *           bit13~0, ���յ�����Ч�ֽ���Ŀ
   *           
   * @param    ������
   *
   * @retval   None
   ******************************************************************************
**/
void UART1_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);                                 //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);                                //ʹ��USART3ʱ��
 
	//����3��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART2);                              //GPIOD5����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART2);                              //GPIOD6����ΪUSART2
	
	//USART3�˿�����
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;                             //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                                        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	                                 //�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                       //���츴�����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                                        //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);                                                //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate            = bound;                               //����������
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;                 //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;                    //һ��ֹͣλ
	USART_InitStructure.USART_Parity              = USART_Parity_No;                     //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;      //��Ӳ������������
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;	     //�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);                                            //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);                                                           //ʹ�ܴ���1 
	
	//USART_ClearFlag(USART2, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                                       //��������ж�

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;                  //����3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;                            //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;		                     //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;			             //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                                                     //����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
	
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
void USART1_IRQHandler(void)                	            
{
	//�ж��Ƿ���յ�����
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
		
        //�Ӵ��������������  1��ֻ�ܽ���1���ֽ�����
        USART1_RX_BUF[USART1_RX_CNT++] = USART_ReceiveData(USART1);
        
        //�жϽ��ջ������Ƿ�����
        if(USART1_RX_CNT >= LEN)
        {
            USART1_RX_CNT = 0;
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
void USART_SendBytes(USART_TypeDef *USARTx,uint8_t *data,uint32_t len)
{	
	while(len--)
	{
		USART_SendData(USARTx,*data++);
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
	return (char *)USART1_RX_BUF;
}



bool USART_CheakData(void)
{
//	uint8_t Check = 0;
//	
//	uint32_t Len = sizeof(USART2_RxBuf);
//	
//	for(uint32_t i=0; i<Len; i++)
//	{
//		if(USART2_RxBuf[i] == UART_Data_Null)
//		{
//			continue;
//		}
//		
//		for(int j=i+2;j<i+17;j++)
//		{
//			Check ^= USART2_RxBuf[j];
//		}
//		
//		if(Check == USART2_RxBuf[i+19])
//		{
//			if(USART2_RxBuf[i++] == UART_Data_Sync)
//			{
//				if(USART2_RxBuf[i++] == UART_Data_Head)
//				{
//					if(i+20>Len)
//					{
//						return true;
//					}
//					else if(i+20<Len && USART2_RxBuf[i+20] == UART_Data_Null)
//					{
//						return true;
//					}
//					else if(i+20<Len && USART2_RxBuf[i+20] != UART_Data_Null)
//					{
//						i+=17;
//					}
//				}
//			}
//		}		
//	}
//	
	return false; 

}




#endif	

 




/**
   ******************************************************************************
   * @file    usart.c 
   * @author  
   * @version v1
   * @date    
   * @brief   串口配置和中断服务函数
   ******************************************************************************
**/


#include "usart.h"	

#if EN_USART1_RX
uint8_t  USART1_RX_BUF[LEN] = {0};    //UART1的接收缓冲区
uint32_t USART1_RX_CNT      = 0;      //UART1的接收缓冲区的计数器
uint16_t USART_RX_STA       = 0;      //接收状态标记
#endif
	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOS使用	  
#endif
 	  

//加入以下代码,支持printf函数,而不需要选择use MicroLIB
//Cortex M3和M4英文的内核文档18.2.2中有说明
#if 1
#pragma import(__use_no_semihosting)   

//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 		
	while((USART1->SR & 0X40) == 0);   //循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}

#endif
 

//如果使能了接收
#if EN_USART1_RX   
 
/**
   ******************************************************************************
   * @brief    USART1初始化函数 
   *   
   * @note 	   读取USARTx->SR能避免莫名其妙的错误
   *           USART1_TX   ---   PA9
   *           USART1_RX   ---   PA10
   *
   *           接收状态
   *           bit15，	接收完成标志
   *           bit14，	接收到0x0d
   *           bit13~0, 接收到的有效字节数目
   *           
   * @param    波特率
   *
   * @retval   None
   ******************************************************************************
**/
void UART1_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);                                 //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);                                //使能USART3时钟
 
	//串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART2);                              //GPIOD5复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART2);                              //GPIOD6复用为USART2
	
	//USART3端口配置
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;                             //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                                        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	                                 //速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                       //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                                        //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);                                                //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate            = bound;                               //波特率设置
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;                 //字长为8位数据格式
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;                    //一个停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No;                     //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;      //无硬件数据流控制
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;	     //收发模式
	USART_Init(USART1, &USART_InitStructure);                                            //初始化串口1
	
	USART_Cmd(USART1, ENABLE);                                                           //使能串口1 
	
	//USART_ClearFlag(USART2, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                                       //开启相关中断

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;                  //串口3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;                            //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;		                     //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;			             //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	                                                     //根据指定的参数初始化VIC寄存器、

#endif
	
}


/**
   ******************************************************************************
   * @brief    USART2中断服务函数 
   *   
   * @note 	   中断服务函数名字是固定的,可以去启动文件startup_stm32f40_41xxx.s查看
   *
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void USART1_IRQHandler(void)                	            
{
	//判断是否接收到数据
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
		
        //从串口外设接收数据  1次只能接收1个字节数据
        USART1_RX_BUF[USART1_RX_CNT++] = USART_ReceiveData(USART1);
        
        //判断接收缓冲区是否已满
        if(USART1_RX_CNT >= LEN)
        {
            USART1_RX_CNT = 0;
        }
    }
} 



/**
   ******************************************************************************
   * @brief    USART发送字节函数 
   *   
   * @note 	   串口发送数据的函数一次只能传输一个字节的数据，但是指令肯定是大于一
   *           个字节的，所以需要采用循环去发送数据
   *            
   * @param    第一个参数是哪个串口要发送数据
   *           第二个参数是要发送的数据
   *           第三个参数是要发送数据的长度
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
   * @brief    获取串口接收到的数据
   *   
   * @note 	   为了可以在文件外面也能使用数组中的数据 
   *                  
   * @param    None
   *           
   * @retval   串口2接收到的数据
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

 




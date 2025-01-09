/**
   ******************************************************************************
   * @file    usart.c 
   * @author  
   * @version v1
   * @date    2024/10/31
   * @brief   串口配置和中断服务函数
   ******************************************************************************
**/


#include "usart.h"


uint8_t  USART1_RxBuf[LEN] = {0};   //串口1的接收数组
uint32_t USART1_RxCnt      =  0;    //串口1的接收计数器

uint8_t  USART1_TxBuf[LEN] = {0};   //串口1的发送数组
uint32_t USART1_TxCnt      =  0;    //串口1的发送计数器

uint8_t  USART2_RxBuf[LEN] = {0};   //串口2的接收数组
uint32_t USART2_RxCnt      =  0;    //串口2的接收计数器

uint8_t  USART2_TxBuf[LEN] = {0};   //串口2的发送数组
uint32_t USART2_TxCnt      =  0;    //串口2的发送计数器


/**
   ******************************************************************************
   * @brief    为了在KEIL中使用printf
   *   
   * @note 	   开发板没有集成显示器,使用printf打印看不到,所以需要对printf进行重定
   *           向,因为printf函数内部会调用fputc函数,对fputc函数重新定向就是把fputc
   *           函数输出字符的目的地修改为串口接口,即可实现在keil中使用printf在串口
   *           调试助手上打印
   *           
   *           Cortex M3和M4英文的内核文档18.2.2中有说明
   ******************************************************************************
**/
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) 
{
    USART_SendData(USART1,ch);                                      //发送一个字符
    while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET );   //等待该字节发送完成后，才可以发送下一个字节
    return (ch);
}

void _sys_exit(int return_code) 
{

}



/**
   ******************************************************************************
   * @brief    USART1参数配置和初始化函数 
   *   
   * @note 	   外设开发流程：
   *           1.定义GPIO、串口、NVIC外设结构体变量
   *           2.打开外设时钟
   *           3.选择IO口复用的功能
   *           4.对GPIO、串口、NVIC外设结构体变量赋值和初始化
   *           5.选择串口中断源
   *           6.串口使能
   *           7.中断服务函数
   *
   *           USART1_TX   ---   PA9
   *           USART1_RX   ---   PA10
   *
   * @param    波特率
   *
   * @retval   None
   ******************************************************************************
**/
void USART1_Config(uint32_t baud)
{
	//1.定义GPIO、串口、NVIC外设的结构体变量
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	//2.打开外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//3.选择IO口复用的功能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//4.配置GPIO结构体变量和初始化
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;                           //PA9 和 PA10
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                                       //复用模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                  //高速
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                      //推挽
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                                       //上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);                                              //初始化
	
	//5.配置串口结构体变量和初始化
	USART_InitStructure.USART_BaudRate            = baud;                               //波特率，传参
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;                //数据位，8bit
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;                   //停止位，1bit
	USART_InitStructure.USART_Parity              = USART_Parity_No;                    //校验位，无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //无流控
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;      //全双工
	USART_Init(USART1, &USART_InitStructure);                                           //初始化
	
	//6.配置NVIC结构体变量和初始化
	NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;                 //设置中断通道为USART1_IRQn
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;                           //设置子优先级，优先级的值越低，则优先级越高
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;                           //设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;                      //使能中断通道
	NVIC_Init(&NVIC_InitStructure);                                                     //初始化
	
	//7.选择串口中断源，接收到数据的时候触发中断，进入中断服务函数
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//8.串口使能
	USART_Cmd(USART1,ENABLE);
}



/**
   ******************************************************************************
   * @brief    USART1中断服务函数 
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
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)            //判断是否接收到数据
	{
		USART1_RxBuf[USART1_RxCnt++] = USART_ReceiveData(USART1);   //把接收到的数据存储在串口1接收数组中
		
		if(USART1_RxCnt >= LEN)                                     //如果大于数组最大容量，计数器清零
		{
			USART1_RxCnt = 0;
		}
	}
}



/**
   ******************************************************************************
   * @brief    USART2参数配置和初始化函数 
   *   
   * @note 	   外设开发流程：
   *           1.定义GPIO、串口、NVIC外设结构体变量
   *           2.打开外设时钟
   *           3.选择IO口复用的功能
   *           4.对GPIO、串口、NVIC外设结构体变量赋值和初始化
   *           5.选择串口中断源
   *           6.串口使能
   *           7.中断服务函数
   *
   *           USART2_TX   ---   PA2
   *           USART2_RX   ---   PA3
   *
   * @param    波特率
   *
   * @retval   None
   ******************************************************************************
**/
void USART2_Config(uint32_t baud)
{
	//1.定义GPIO、串口、NVIC外设的结构体变量
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	//2.打开外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);      //串口2在APB1总线下
	
	//3.选择IO口复用的功能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//4.配置GPIO结构体变量和初始化
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;                            //PA2 和 PA3
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                                       //复用模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                                  //高速
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                      //推挽
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                                       //上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);                                              //初始化
	
	//5.配置串口结构体变量和初始化
	USART_InitStructure.USART_BaudRate            = baud;                               //波特率，传参
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;                //数据位，8bit
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;                   //停止位，1bit
	USART_InitStructure.USART_Parity              = USART_Parity_No;                    //校验位，无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //无流控
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;      //全双工
	USART_Init(USART2, &USART_InitStructure);                                           //初始化
	
	//6.配置NVIC结构体变量和初始化
	NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;                 //设置中断通道为USART2_IRQn
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;                           //设置子优先级，优先级的值越低，则优先级越高
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;                           //设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;                      //使能中断通道
	NVIC_Init(&NVIC_InitStructure);                                                     //初始化
	
	//7.选择串口中断源，接收到数据的时候出发中断，进入中断服务函数
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	//8.串口使能
	USART_Cmd(USART2,ENABLE);
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
void USART2_IRQHandler(void)
{	
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)            //判断是否接收到数据
	{
		USART2_RxBuf[USART2_RxCnt++] = USART_ReceiveData(USART2);   //把接收到的数据存储在串口2接收数组中
		
		if(USART2_RxCnt >= LEN)                                     //如果大于数组最大容量，计数器清零
		{
			USART2_RxCnt = 0;
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
	return (char *)USART2_RxBuf;
}



/**
   ******************************************************************************
   * @brief    校验函数
   *   
   * @note 	   校验接收的数据格式是否正确
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


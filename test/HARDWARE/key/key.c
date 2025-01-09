/**
   ******************************************************************************
   * @file    main.c 
   * @author  
   * @version v1
   * @date    2024/11/18
   * @brief   按键扫描函数
   ******************************************************************************
**/


#include "key.h"
#include "sys.h"


extern uint8_t  USART2_RxBuf[LEN];                  //串口2的接收数组
extern uint32_t USART2_RxCnt;                       //串口2的接收计数器
extern uint8_t  USART2_TxBuf[LEN];                  //串口2的发送数组
extern uint32_t USART2_TxCnt;                       //串口2的发送计数器
extern uint8_t  Timer5msFlag;                       //定时5ms标志位


union    UART_Command_Def UART_TX_Command = {0};    //串口传输命令的联合体，里面是数组和多个命令数据结构体   
Key_t    Key = {KEY_IDLE, 0,0,0};                   //初始化按键状态
uint32_t SpeakValue = 8;                            //音量值


/**
   ******************************************************************************
   * @brief    按键扫描函数
   *   
   * @note 	   要进行消抖，消除因为外界因素导致的按键抖动
   *                  
   * @param    None
   *           
   * @retval   None
   ******************************************************************************
**/
void Key_Scan(void) 
{
	Timer5msFlag = 0;                           //清除定时器标志
	
	memset(&UART_TX_Command,0,sizeof(UART_TX_Command));
	
    static  uint8_t  LastState = 0;             //按键上一次状态
    static  uint16_t DebounceCounter = 0;       //消抖计数器
    uint8_t CurrentState = KEY0STATE;           //读取当前按键状态
	char    *data = {0};                        //存储串口2接收到的数据
	
    if (CurrentState != LastState)              //当前按键状态发生改变，启动消抖
	{  
        DebounceCounter = 0;
    } 
	else 
	{
        if (DebounceCounter < 5)               //计数未达到消抖时间
		{  
            DebounceCounter++;
        } 
		else 
		{  
			//消抖完成
            if (CurrentState == 1) 
			{  
				// 按键被按下
                if (Key.State == KEY_IDLE) 
				{
                    Key.State     = KEY_PRESS;
                    Key.PressTime = 0;
                }
            } 
			else 
			{  
				// 按键松开
                if (Key.State == KEY_PRESS) 
				{
                    if (Key.PressTime >= LONG_PRESS_TIME)                                            //长按
					{ 
                        GPIO_ResetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);
						Volume_DownPressed();
                    } 
					else if (Key.PressTime >= SHORT_PRESS_TIME && Key.PressTime < LONG_PRESS_TIME)   //短按 
					{
						Key.ClickCount++;                                                            // 累加短按计数
                        Key.ClickInterval = 0;                                                       // 重置间隔计时器
                    } 
					else                                                                             // 按键时间太短，认定为无效按键
					{
						Key.State = KEY_IDLE;
					}
					
					Key.PressTime = 0;                                                               //清空按键按下时间
                }
            }
        }
    }

    LastState = CurrentState;                                                                        //更新上次按键状态

    //按键按下时间累加
    if (Key.State == KEY_PRESS) 
	{
        Key.PressTime += 5;  
    }
	
	
	//检测多次短按间隔
	if(Key.ClickCount > 0 && Key.State == KEY_IDLE) 
	{
		Key.ClickInterval += 5;                                                                      // 累加间隔时间
		if (Key.ClickInterval > MULTI_PRESS_INTERVAL)                                                // 超过最大间隔，认为短按完成
		{  
			// 根据点击次数执行功能
			
			switch(Key.ClickCount)
			{
				case 1:
					
					//单击事件
					GPIO_ResetBits(GPIOF,GPIO_Pin_10);
					
					Volume_UpPressed(); 
				
					delay_ms(1000);
					GPIO_SetBits(GPIOF,GPIO_Pin_10);
							
					if(USART_CheakData())
					{
						printf("\r\n");
						printf("1.true\r\n");
						printf("\r\n");
					}
					else
					{
						printf("\r\n");
						printf("1.接收数据不正确\r\n");
						printf("\r\n");
					}
					
					data = USART_GetData();				
					for(int i=0; i<256; i++)
					{
						printf("%02x\r\n",data[i]);
					}
					printf("\r\n");
					
					break;
					
				case 2:
					
					//双击事件
					GPIO_ResetBits(GPIOF,GPIO_Pin_9);
					Volume_DownPressed();                                                      
				
					delay_ms(1000);
					GPIO_SetBits(GPIOF,GPIO_Pin_9);
					
					if(USART_CheakData())
					{
						printf("\r\n");
						printf("2.true\r\n");
						printf("\r\n");
					}
					else
					{
						printf("\r\n");
						printf("2.接收数据不正确\r\n");
						printf("\r\n");
					}
					
					data = USART_GetData();
					for(int i=0; i<256; i++)
					{
						printf("%02x\r\n",data[i]);
					}
					
					printf("\r\n");
					
					break;
				
				case 3:
					
					//三击事件
					GPIO_ResetBits(GPIOF,GPIO_Pin_10 | GPIO_Pin_9);
					Channel_UpPressed();                                                     
				
					delay_ms(1000);
					GPIO_SetBits(GPIOF,GPIO_Pin_10 | GPIO_Pin_9);
				
					if(USART_CheakData())
					{
						printf("\r\n");
						printf("3.true\r\n");
						printf("\r\n");
					}
					else
					{
						printf("\r\n");
						printf("3.接收数据不正确\r\n");
						printf("\r\n");
					}
					
					data = USART_GetData();
					for(int i=0; i<256; i++)
					{
						printf("%02x\r\n",data[i]);
					}
					
					printf("\r\n");
				
					break;
				
				case 4:
					
					//四击事件
					GPIO_ResetBits(GPIOF,GPIO_Pin_10 | GPIO_Pin_9);
					Channel_DownPressed();                                                     
				
					delay_ms(1000);
					GPIO_SetBits(GPIOF,GPIO_Pin_10 | GPIO_Pin_9);
				
					if(USART_CheakData())
					{
						printf("\r\n");
						printf("4.true\r\n");
						printf("\r\n");
					}
					else
					{
						printf("\r\n");
						printf("4.接收数据不正确\r\n");
						printf("\r\n");
					}
					
					data = USART_GetData();
					for(int i=0; i<256; i++)
					{
						printf("%02x\r\n",data[i]);
					}
					
					printf("\r\n");
				
					break;
				
				case 5:
					
					//五击事件
					GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);
				
					break;
				
				default:
					break;
			}

			memset(&UART_TX_Command,0,sizeof(UART_TX_Command));
			memset(data,0,256);
			Key.ClickCount = 0;                                                                    //清空短按计数
			Key.ClickInterval = 0;                                                                 //清空短按间隔
		}
	}
    
}


/**
   ******************************************************************************
   * @brief    加音量操作函数
   *   
   * @note 	   发送数据之前线进行音量的判断，看看是否超过最大音量
   *                  
   * @param    None
   *           
   * @retval   None
   ******************************************************************************
**/
void Volume_UpPressed(void)
{	
	UART_TX_Command.MK_Command.Sub_Command = UART_Control_SubCommand_V_UP;				//将命令字节放进对应的结构体中
	
	printf("子命令赋值：\r\n");
	printf("%02x\r\n",UART_TX_Command.MK_Command.Sub_Command);
	printf("\r\n");
	
	if(SpeakValue < 0x0F)
	{
		SpeakValue++;
		//SetSpeakvalue
	}
	
	SendMessage(UART_Control_Command);                                                  //将主命令字节作为参数进行传输
}


void Volume_DownPressed(void)
{
	UART_TX_Command.MK_Command.Sub_Command = UART_Control_SubCommand_V_DOWN;				//将命令字节放进对应的结构体中
	
	printf("子命令赋值：\r\n");
	printf("%02x\r\n",UART_TX_Command.MK_Command.Sub_Command);
	printf("\r\n");
	
	if(SpeakValue)
	{
		SpeakValue--;
		//SetSpeakvalue
	}
	
	SendMessage(UART_Control_Command);                                                  //将主命令字节作为参数进行传输
}


void Channel_UpPressed(void)
{
	UART_TX_Command.MK_Command.Sub_Command = UART_Control_SubCommand_CH_UP;					//将命令字节放进对应的结构体中
	
	printf("子命令赋值：\r\n");
	printf("%02x\r\n",UART_TX_Command.MK_Command.Sub_Command);
	printf("\r\n");
	
	SendMessage(UART_Control_Command);					                                    //将主命令字节作为参数进行传输
}


void Channel_DownPressed(void)
{
	UART_TX_Command.MK_Command.Sub_Command = UART_Control_SubCommand_CH_DOWN;               //将命令字节放进对应的结构体中
	
	printf("子命令赋值：\r\n");
	printf("%02x\r\n",UART_TX_Command.MK_Command.Sub_Command);
	printf("\r\n");
	
	SendMessage(UART_Control_Command);					                                    //将主命令字节作为参数进行传输
}


void Voice_Broadcast(char *Type)
{
	//将命令字节放进对应的结构体中
	UART_TX_Command.MK_Command.Sub_Command = UART_Control_SubCommand_Speak;
	
	if(strcmp(Type,"Battery") == 0)
	{	
		UART_TX_Command.MK_Command.Append[0] = 0;
	}
	
	if(strcmp(Type,"Channel") == 0)
	{
		UART_TX_Command.MK_Command.Append[0] = 1;
	}
	
	if(strcmp(Type,"Corridor") == 0)
	{
		UART_TX_Command.MK_Command.Append[0] = 2;
	}
	
	//将主命令字节作为参数进行传输
	SendMessage(UART_Control_Command);
}



/**
   ******************************************************************************
   * @brief    发送数据函数
   *   
   * @note 	   根据mdk指令按照顺序将数据打包在数组中，发送给串口
   *                  
   * @param    主命令参数
   *           
   * @retval   None
   ******************************************************************************
**/
void SendMessage(unsigned char Command)
{
	uint8_t Check = Command;
	
	//至少8个唤醒字节
	for(uint32_t i=0; i<16; i++)
	{
		USART2_TxBuf[USART2_TxCnt++] = UART_Data_Null;
	}
	
	USART2_TxBuf[USART2_TxCnt++] = UART_Data_Sync;           //波特率提取字节，固定
	USART2_TxBuf[USART2_TxCnt++] = UART_Data_Head;           //命令起始字节，固定
	USART2_TxBuf[USART2_TxCnt++] = Command;                  //主命令字节
	
	//命令参数字节
	for(uint32_t i=0; i<16; i++)
	{
		USART2_TxBuf[USART2_TxCnt++] = UART_TX_Command.Fields[i];
		
		Check^= UART_TX_Command.Fields[i];
	}
	
	printf("发送函数中子命令：\r\n");
	printf("%02x\r\n",UART_TX_Command.Fields[0]);
	printf("\r\n");
	
	//校验和
	USART2_TxBuf[USART2_TxCnt++] = Check;
	
	printf("发送的数据是：\r\n");
	for(uint32_t i=0; i<USART2_TxCnt; i++)
	{
		printf("%02x\r\n",USART2_TxBuf[i]);
	}
	printf("\r\n");
	
	//将数组发送给串口
	USART_SendBytes(USART2,USART2_TxBuf,USART2_TxCnt);
	
	USART2_TxCnt = 0;
}


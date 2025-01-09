/**
   ******************************************************************************
   * @file    main.c 
   * @author  
   * @version v1
   * @date    2024/11/18
   * @brief   ����ɨ�躯��
   ******************************************************************************
**/


#include "key.h"
#include "sys.h"


extern uint8_t  USART2_RxBuf[LEN];                  //����2�Ľ�������
extern uint32_t USART2_RxCnt;                       //����2�Ľ��ռ�����
extern uint8_t  USART2_TxBuf[LEN];                  //����2�ķ�������
extern uint32_t USART2_TxCnt;                       //����2�ķ��ͼ�����
extern uint8_t  Timer5msFlag;                       //��ʱ5ms��־λ


union    UART_Command_Def UART_TX_Command = {0};    //���ڴ�������������壬����������Ͷ���������ݽṹ��   
Key_t    Key = {KEY_IDLE, 0,0,0};                   //��ʼ������״̬
uint32_t SpeakValue = 8;                            //����ֵ


/**
   ******************************************************************************
   * @brief    ����ɨ�躯��
   *   
   * @note 	   Ҫ����������������Ϊ������ص��µİ�������
   *                  
   * @param    None
   *           
   * @retval   None
   ******************************************************************************
**/
void Key_Scan(void) 
{
	Timer5msFlag = 0;                           //�����ʱ����־
	
	memset(&UART_TX_Command,0,sizeof(UART_TX_Command));
	
    static  uint8_t  LastState = 0;             //������һ��״̬
    static  uint16_t DebounceCounter = 0;       //����������
    uint8_t CurrentState = KEY0STATE;           //��ȡ��ǰ����״̬
	char    *data = {0};                        //�洢����2���յ�������
	
    if (CurrentState != LastState)              //��ǰ����״̬�����ı䣬��������
	{  
        DebounceCounter = 0;
    } 
	else 
	{
        if (DebounceCounter < 5)               //����δ�ﵽ����ʱ��
		{  
            DebounceCounter++;
        } 
		else 
		{  
			//�������
            if (CurrentState == 1) 
			{  
				// ����������
                if (Key.State == KEY_IDLE) 
				{
                    Key.State     = KEY_PRESS;
                    Key.PressTime = 0;
                }
            } 
			else 
			{  
				// �����ɿ�
                if (Key.State == KEY_PRESS) 
				{
                    if (Key.PressTime >= LONG_PRESS_TIME)                                            //����
					{ 
                        GPIO_ResetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);
						Volume_DownPressed();
                    } 
					else if (Key.PressTime >= SHORT_PRESS_TIME && Key.PressTime < LONG_PRESS_TIME)   //�̰� 
					{
						Key.ClickCount++;                                                            // �ۼӶ̰�����
                        Key.ClickInterval = 0;                                                       // ���ü����ʱ��
                    } 
					else                                                                             // ����ʱ��̫�̣��϶�Ϊ��Ч����
					{
						Key.State = KEY_IDLE;
					}
					
					Key.PressTime = 0;                                                               //��հ�������ʱ��
                }
            }
        }
    }

    LastState = CurrentState;                                                                        //�����ϴΰ���״̬

    //��������ʱ���ۼ�
    if (Key.State == KEY_PRESS) 
	{
        Key.PressTime += 5;  
    }
	
	
	//����ζ̰����
	if(Key.ClickCount > 0 && Key.State == KEY_IDLE) 
	{
		Key.ClickInterval += 5;                                                                      // �ۼӼ��ʱ��
		if (Key.ClickInterval > MULTI_PRESS_INTERVAL)                                                // �������������Ϊ�̰����
		{  
			// ���ݵ������ִ�й���
			
			switch(Key.ClickCount)
			{
				case 1:
					
					//�����¼�
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
						printf("1.�������ݲ���ȷ\r\n");
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
					
					//˫���¼�
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
						printf("2.�������ݲ���ȷ\r\n");
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
					
					//�����¼�
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
						printf("3.�������ݲ���ȷ\r\n");
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
					
					//�Ļ��¼�
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
						printf("4.�������ݲ���ȷ\r\n");
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
					
					//����¼�
					GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);
				
					break;
				
				default:
					break;
			}

			memset(&UART_TX_Command,0,sizeof(UART_TX_Command));
			memset(data,0,256);
			Key.ClickCount = 0;                                                                    //��ն̰�����
			Key.ClickInterval = 0;                                                                 //��ն̰����
		}
	}
    
}


/**
   ******************************************************************************
   * @brief    ��������������
   *   
   * @note 	   ��������֮ǰ�߽����������жϣ������Ƿ񳬹��������
   *                  
   * @param    None
   *           
   * @retval   None
   ******************************************************************************
**/
void Volume_UpPressed(void)
{	
	UART_TX_Command.MK_Command.Sub_Command = UART_Control_SubCommand_V_UP;				//�������ֽڷŽ���Ӧ�Ľṹ����
	
	printf("�����ֵ��\r\n");
	printf("%02x\r\n",UART_TX_Command.MK_Command.Sub_Command);
	printf("\r\n");
	
	if(SpeakValue < 0x0F)
	{
		SpeakValue++;
		//SetSpeakvalue
	}
	
	SendMessage(UART_Control_Command);                                                  //���������ֽ���Ϊ�������д���
}


void Volume_DownPressed(void)
{
	UART_TX_Command.MK_Command.Sub_Command = UART_Control_SubCommand_V_DOWN;				//�������ֽڷŽ���Ӧ�Ľṹ����
	
	printf("�����ֵ��\r\n");
	printf("%02x\r\n",UART_TX_Command.MK_Command.Sub_Command);
	printf("\r\n");
	
	if(SpeakValue)
	{
		SpeakValue--;
		//SetSpeakvalue
	}
	
	SendMessage(UART_Control_Command);                                                  //���������ֽ���Ϊ�������д���
}


void Channel_UpPressed(void)
{
	UART_TX_Command.MK_Command.Sub_Command = UART_Control_SubCommand_CH_UP;					//�������ֽڷŽ���Ӧ�Ľṹ����
	
	printf("�����ֵ��\r\n");
	printf("%02x\r\n",UART_TX_Command.MK_Command.Sub_Command);
	printf("\r\n");
	
	SendMessage(UART_Control_Command);					                                    //���������ֽ���Ϊ�������д���
}


void Channel_DownPressed(void)
{
	UART_TX_Command.MK_Command.Sub_Command = UART_Control_SubCommand_CH_DOWN;               //�������ֽڷŽ���Ӧ�Ľṹ����
	
	printf("�����ֵ��\r\n");
	printf("%02x\r\n",UART_TX_Command.MK_Command.Sub_Command);
	printf("\r\n");
	
	SendMessage(UART_Control_Command);					                                    //���������ֽ���Ϊ�������д���
}


void Voice_Broadcast(char *Type)
{
	//�������ֽڷŽ���Ӧ�Ľṹ����
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
	
	//���������ֽ���Ϊ�������д���
	SendMessage(UART_Control_Command);
}



/**
   ******************************************************************************
   * @brief    �������ݺ���
   *   
   * @note 	   ����mdkָ���˳�����ݴ���������У����͸�����
   *                  
   * @param    ���������
   *           
   * @retval   None
   ******************************************************************************
**/
void SendMessage(unsigned char Command)
{
	uint8_t Check = Command;
	
	//����8�������ֽ�
	for(uint32_t i=0; i<16; i++)
	{
		USART2_TxBuf[USART2_TxCnt++] = UART_Data_Null;
	}
	
	USART2_TxBuf[USART2_TxCnt++] = UART_Data_Sync;           //��������ȡ�ֽڣ��̶�
	USART2_TxBuf[USART2_TxCnt++] = UART_Data_Head;           //������ʼ�ֽڣ��̶�
	USART2_TxBuf[USART2_TxCnt++] = Command;                  //�������ֽ�
	
	//��������ֽ�
	for(uint32_t i=0; i<16; i++)
	{
		USART2_TxBuf[USART2_TxCnt++] = UART_TX_Command.Fields[i];
		
		Check^= UART_TX_Command.Fields[i];
	}
	
	printf("���ͺ����������\r\n");
	printf("%02x\r\n",UART_TX_Command.Fields[0]);
	printf("\r\n");
	
	//У���
	USART2_TxBuf[USART2_TxCnt++] = Check;
	
	printf("���͵������ǣ�\r\n");
	for(uint32_t i=0; i<USART2_TxCnt; i++)
	{
		printf("%02x\r\n",USART2_TxBuf[i]);
	}
	printf("\r\n");
	
	//�����鷢�͸�����
	USART_SendBytes(USART2,USART2_TxBuf,USART2_TxCnt);
	
	USART2_TxCnt = 0;
}


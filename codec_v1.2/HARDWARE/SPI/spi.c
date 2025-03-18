/**
   ******************************************************************************
   * @file    codec.c 
   * @author  
   * @version v1
   * @date    
   * @brief   ��Ƶ����
   ******************************************************************************
**/

#include "spi.h"


/**
   ******************************************************************************
   * @brief    SPI3��ʼ��
   *   
   * @note 	   CS     ---   PA15
   *           SCK    ---   PC10
   *           MOSI   ---   PC12
   *           
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void SPI3_Init(void) 
{
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef  SPI_InitStruct;

    // 1. ����SPI��GPIOʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

    // 2. ����SPI���ţ�PC10: SCK, PC12: MOSI��
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    // 3. ����SPI����ΪSPI3�ĸ��ù���
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

    // 4. ����CS���ţ�PA15��
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);  // ����CSΪ�ߵ�ƽ
	
	// 5.���õ���˫������ѡ������
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);  // ���øߵ�ƽ

    // 6. ����SPI����
    SPI_InitStruct.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;   // ȫ˫��
    SPI_InitStruct.SPI_Mode              = SPI_Mode_Master;                   // ��ģʽ
    SPI_InitStruct.SPI_DataSize          = SPI_DataSize_16b;                  // 16λ����
    SPI_InitStruct.SPI_CPOL              = SPI_CPOL_High;                     // ʱ�Ӽ��Ը�
    SPI_InitStruct.SPI_CPHA              = SPI_CPHA_2Edge;                    // ʱ����λ2����
    SPI_InitStruct.SPI_NSS               = SPI_NSS_Soft;                      // �������NSS
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;           // ʱ�ӷ�Ƶ����
    SPI_InitStruct.SPI_FirstBit          = SPI_FirstBit_MSB;                  // MSB����
    SPI_Init(SPI3, &SPI_InitStruct);

    // 6. ʹ��SPI
    SPI_Cmd(SPI3, ENABLE);
}

/**
   ******************************************************************************
   * @brief    д�Ĵ�������
   *   
   * @note 	   ����ʱҪƬѡ���ű���Ҫ�ӵͱ��
   *           SPI�����ֲ�ͬ�Ľṹ��һ����16bit(Ĭ��)��һ����24bit
   *           16bit��7bit�Ĵ�����ַ + 9bit�Ĵ�������
   *           24bit��8bit�豸��ַ + 7bit�Ĵ�����ַ + 9bit����
   *           
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void MySPI_Write(uint8_t addr,uint16_t data)
{
	uint16_t spi_data;
	spi_data = (uint16_t)(addr << 9);
	spi_data |= (data & 0x01FF);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);  // ��Ƭѡ��������
	
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI3,spi_data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);  // ��Ƭѡ��������
	delay_us(5);
	GPIO_SetBits(GPIOA, GPIO_Pin_15);    // ��Ƭѡ��������
}


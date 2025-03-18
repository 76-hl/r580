/**
   ******************************************************************************
   * @file    codec.c 
   * @author  
   * @version v1
   * @date    
   * @brief   音频配置
   ******************************************************************************
**/

#include "spi.h"


/**
   ******************************************************************************
   * @brief    SPI3初始化
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

    // 1. 启用SPI和GPIO时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

    // 2. 配置SPI引脚（PC10: SCK, PC12: MOSI）
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    // 3. 配置SPI引脚为SPI3的复用功能
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

    // 4. 配置CS引脚（PA15）
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);  // 设置CS为高电平
	
	// 5.配置单刀双掷开关选择引脚
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);  // 设置高电平

    // 6. 配置SPI参数
    SPI_InitStruct.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;   // 全双工
    SPI_InitStruct.SPI_Mode              = SPI_Mode_Master;                   // 主模式
    SPI_InitStruct.SPI_DataSize          = SPI_DataSize_16b;                  // 16位数据
    SPI_InitStruct.SPI_CPOL              = SPI_CPOL_High;                     // 时钟极性高
    SPI_InitStruct.SPI_CPHA              = SPI_CPHA_2Edge;                    // 时钟相位2边沿
    SPI_InitStruct.SPI_NSS               = SPI_NSS_Soft;                      // 软件管理NSS
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;           // 时钟分频因子
    SPI_InitStruct.SPI_FirstBit          = SPI_FirstBit_MSB;                  // MSB优先
    SPI_Init(SPI3, &SPI_InitStruct);

    // 6. 使能SPI
    SPI_Cmd(SPI3, ENABLE);
}

/**
   ******************************************************************************
   * @brief    写寄存器操作
   *   
   * @note 	   结束时要片选引脚必须要从低变高
   *           SPI有两种不同的结构，一个是16bit(默认)，一个是24bit
   *           16bit：7bit寄存器地址 + 9bit寄存器数据
   *           24bit：8bit设备地址 + 7bit寄存器地址 + 9bit数据
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
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);  // 将片选引脚拉低
	
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI3,spi_data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);  // 将片选引脚拉低
	delay_us(5);
	GPIO_SetBits(GPIOA, GPIO_Pin_15);    // 将片选引脚拉高
}


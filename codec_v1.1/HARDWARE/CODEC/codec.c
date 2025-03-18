/**
   ******************************************************************************
   * @file    codec.c 
   * @author  
   * @version v1
   * @date    
   * @brief   音频配置
   ******************************************************************************
**/

#include "codec.h"

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
void SPI_WriteReg(uint8_t addr,uint16_t data)
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

/**
   ******************************************************************************
   * @brief    NAU9914寄存器配置
   *   
   * @note 	   根据数据手册进行配置
   *           
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void NAU8814_Init(void)
{	
	//使能
	
	
	
	//需要在0x01中使能MICBIASEN，MICBIASEN[4]置1
	//1.Input Signal Control Register 输入信号控制寄存器
	SPI_WriteReg(0x2C,0x003);    //MIC+ 和 MIC-连接到PGA正端和负端，采用差分输入模式，有效抑制共模噪声
	                             //(addr0x3A，MICBIASM[4] = 0)麦克风偏置电压为0.9*3.3(VDDA) = 2.97V
	
	//需要在0x02中使能PGA，PGAEN[2]置1
	//2.PGA Gain Control RegisterPGA PAG增益控制寄存器
	SPI_WriteReg(0x2D,0x028);    //可编程器放大增益设置为18dB，PGA正常输出，不静音，若静音了则信号不会被放大或者传输到后续电路中
	                             //不启用零交叉检测，当增益寄存器更改时，更新增益
	                             
	//需要在0x02中使能BOOST，BSTEN[4]置1
	//3.ADC Boost Control Registers ADC升压控制寄存器
	SPI_WriteReg(0x2F,0x100);    //PGA输出增益为20dB，AUX和MIC都不连接到boost
	                             //如果MIC+引脚要到输入提升阶段，需要设置PMICPAG[0] = 0(0x2C)
	                         
	
	
	//需要在0x02中使能ADC，ADCEN[0]置1
	//4.ADC Control Register ADC控制寄存器
	SPI_WriteReg(0x0E,0x1e8);    //极性默认，过采样率为128x，为最佳信噪比
	                             //采用应用模式，此时高通滤波器是二阶的，截止频率为327Hz，截至频率与采样频率相关联，采样频率为8kHz
	                             //HPFEN[8]置1，使能高通滤波器
	                             
	//5.NOTCH FILTER REGISTERS 缺口滤波器寄存器
	SPI_WriteReg(0x1B,0x000);    //暂时不启用
	SPI_WriteReg(0x1C,0x000);    //暂时不启用
	SPI_WriteReg(0x1D,0x000);    //暂时不启用
	SPI_WriteReg(0x1E,0x000);    //暂时不启用
	
	//6.ADC Gain Control Register ADC增益控制寄存器
	SPI_WriteReg(0x0F,0x0FF);    //增益为0dB
	
	//7.ALC REGISTER 自动电平控制寄存器
	SPI_WriteReg(0x20,0x038);    //ALC1,暂时默认，不启用
	                             //若启用ALC，ALC监控ADC的输出，调整增益，并反馈给PGA
	SPI_WriteReg(0x21,0x00B);    //ALC2,暂时默认，不启用
	SPI_WriteReg(0x22,0x032);    //ALC3,暂时默认，不启用
	
	
	
	//8.DAC Control Register DAC控制寄存器
	//需要在0x03中使能DAC，DACEN[0]置1
	SPI_WriteReg(0x0A,0x008);    //极性默认，软静音和自动静音都不启动，可以在需要的时候在进行配置
	                             //oversample设置为128x，为最佳信噪比
								 
	//9.DAC Gain Control Register DAC增益控制寄存器				 
	SPI_WriteReg(0x0B,0x0FF);    //增益为0dB

	//10.DIGITAL TO ANALOG CONVERTER (DAC) LIMITER REGISTERS 数模转换器（DAC）限制器寄存器
	SPI_WriteReg(0x18,0x032);    //默认，不启用
	SPI_WriteReg(0x19,0x000);    //默认，不启用
								 
	//11.5-BAND EQUALIZER CONTROL REGISTERS5 5波段均衡器控制寄存器
	SPI_WriteReg(0x12,0x16C);    //EQ1，中心频率为175 ，增益为0dB，DAC通道
	SPI_WriteReg(0x13,0x026);    //EQ2，中心频率为300 ，增益为6dB，DAC通道
	SPI_WriteReg(0x14,0x063);    //EQ3，中心频率为1.4k，增益为9dB，DAC通道
	SPI_WriteReg(0x15,0x066);    //EQ4，中心频率为4.1k，增益为6dB，DAC通道
	SPI_WriteReg(0x16,0x04C);    //EQ5，中心频率为9.0k，增益为3dB，DAC通道
	
	//12.Attenuation Control Register 衰减控制寄存器
	SPI_WriteReg(0x28,0x002);    //扬声混合器的旁路路径衰减-10dB， MONO  mixer input为0dB
	
	
	
	//13.GPIO Control Register GPIO控制寄存器
	SPI_WriteReg(0x08,0x000);    //作为SPI接口，即片选引脚，设置为Csb Input，PLL输出时钟不分频
	
	//14.Audio Sample Rate Control Register 音频采样速率控制寄存器
	SPI_WriteReg(0x07,0x00a);    //采样频率8kHz，不启用慢时钟
	
	//要使能PLL
	//15.PLL Control Registers配置PLL寄存器
	SPI_WriteReg(0x24,0x008);    //MCLK进入不进行2分频，并且N = 8，K = 0
	SPI_WriteReg(0x25,0x000);
	SPI_WriteReg(0x26,0x000);
	SPI_WriteReg(0x27,0x000);
	
	//16.Clock Control Register 配置时钟寄存器
	SPI_WriteReg(0x06,0x1e1);    //设置主时钟模式,采用PLL，分频系数为12
	
	//17.MISC CONTROL REGISTER 杂项控制寄存器
	SPI_WriteReg(0x49,0x000);    //设置SPIEN[8],和mode pin去选择I2C还是SPI模式
	                             //mode pin已经接高电平，SPIEN[8]设为0，选择SPI 16bit模式
	
	//18.Audio Interface Control 音频控制寄存器
	SPI_WriteReg(0x04,0x190);    //ADC和DAC数据在帧时钟左侧，I2S传输，16位宽，BCLK和FS都为下降沿触发
	
	//19.Audio Interface Companding Control 音频接口压缩控制
	SPI_WriteReg(0x05,0x000);    //音频接口压缩寄存器，音频不压缩，不环回
	
	
	
	
	//需要在0x03中使能SPKMIX，SPKMXEN[2]置1
	//20.Speaker Mixer Control Register 扬声器混合器控制寄存器
	SPI_WriteReg(0x32,0x003);    //DAC 和 BYPASS 到扬声器混合器输出，AUX不连接到扬声器输出
	
	//21.Speaker Gain Control Register 扬声器增益控制寄存器
	SPI_WriteReg(0x36,0x0b9);    //扬声器增益为0dB，使能扬声器，立刻更改增益
	
	//需要在0x03中使能MOUT，MOUTMXEN[3]置1
	//22.MONO Mixer Control Register MONO混频器控制寄存器
	SPI_WriteReg(0x38,0x001);    //DAC to MONO Mixer，AUX 和 BYPASS都不连接到MONO Mixer，并且MOUT不静音
	
	//23.Output Register 输出寄存器
	SPI_WriteReg(0x31,0x002);    //mono输出和扬声器输出增益提升都不启用，启用过热保护，模拟输出阻抗~1kΩ
}




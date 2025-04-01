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
   * @note     CS     ---   PA15
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

    // 1. 启用SPI和GPIO时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // 2. 配置SPI引脚（PC10: SCK, PC12: MOSI）
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_SetBits(GPIOC, GPIO_Pin_10);  // SCK空闲高电平，设置SCK为高电平

    // 4. 配置CS引脚（PA15）
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);  // CS空闲高电平，设置CS为高电平

    // 5.配置单刀双掷开关选择引脚
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);  // 设置高电平
}

/**
   ******************************************************************************
   * @brief    写寄存器操作
   *
   * @note     结束时要片选引脚必须要从低变高
   *           SPI有两种不同的结构，一个是16bit(默认)，一个是24bit
   *           16bit：7bit寄存器地址 + 9bit寄存器数据
   *           24bit：8bit设备地址 + 7bit寄存器地址 + 9bit数据
   *
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void SPI_WriteReg(uint8_t addr,uint16_t val)
{
    uint16_t spi_data = addr;

    spi_data = (spi_data<<9)|(val & 0x01FF);

    for(u8 i=0; i<16; i++)
    {
        if( spi_data & 0x8000)
        {
            MYSPI_MOSI(1);
        }
        else
        {
            MYSPI_MOSI(0);
        }

        spi_data <<= 1;

        MYSPI_SCK(0);//下降沿出现
        delay_us(5);

        if(i==15)
        {
            MYSPI_CS(0);//提前拉低片选
        }

        MYSPI_SCK(1);
        delay_us(5);
    }

    MYSPI_CS(1);
}

/**
   ******************************************************************************
   * @brief    NAU9914寄存器配置
   *
   * @note     根据数据手册进行配置
   *
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/


int SpeakValue=8;
const unsigned short SpeakValue_Setting[16]=
{
 0x0A1,0x0A3,0x0A5,0x0A7,0x0A9,0x0AB,0x0AD,0x0AF,
 0x0B1,0x0B3,0x0B5,0x0B7,0x0B9,0x0BB,0x0BD,0x0BF};
void SetSpeakvalue(void)
{
 int ValueControl=SpeakValue&0x0F;
 if (ValueControl>=sizeof(SpeakValue_Setting)/sizeof(short))
     ValueControl=sizeof(SpeakValue_Setting)/sizeof(short)-1;

 ValueControl=SpeakValue_Setting[ValueControl];

 SPI_WriteReg(0x36,ValueControl);       //54, Speaker value
}


void NAU8814_Init(void)
{
    SPI_WriteReg(0x04,0x118);    //ADC和DAC数据在帧时钟左侧，I2S传输，16位宽，BCLK和FS都为下降沿触发
    SPI_WriteReg(0x05,0x000);    //音频接口压缩寄存器，音频不压缩，不环回
    SPI_WriteReg(0x06,0x0AD);    //设置主时钟模式,采用PLL，分频系数为12
    SPI_WriteReg(0x07,0x00A);    //采样频率8kHz，不启用慢时钟
    SPI_WriteReg(0x08,0x000);    //作为SPI接口，即片选引脚，设置为Csb Input，PLL输出时钟不分频
    SPI_WriteReg(0x0A,0x008);    //极性默认，软静音和自动静音都不启动，可以在需要的时候在进行配置
                                 //oversample设置为128x，为最佳信噪比
    SPI_WriteReg(0x0B,0x0FF);    //增益为0dB
    SPI_WriteReg(0x0E,0x1F8);    //极性默认，过采样率为128x，为最佳信噪比
                                 //采用应用模式，此时高通滤波器是二阶的，截止频率为327Hz，截至频率与采样频率相关联，采样频率为8kHz
                                 //HPFEN[8]置1，使能高通滤波器
    SPI_WriteReg(0x0F,0x0FF);    //增益为0dB
    SPI_WriteReg(0x12,0x078);    //EQ1，中心频率为175 ，增益为0dB，DAC通道
    SPI_WriteReg(0x13,0x000);    //EQ2，中心频率为300 ，增益为6dB，DAC通道
    SPI_WriteReg(0x14,0x000);    //EQ3，中心频率为1.4k，增益为9dB，DAC通道
    SPI_WriteReg(0x15,0x000);    //EQ4，中心频率为4.1k，增益为6dB，DAC通道
    SPI_WriteReg(0x16,0x000);    //EQ5，中心频率为9.0k，增益为3dB，DAC通道
    SPI_WriteReg(0x18,0x000);    //默认，不启用
    SPI_WriteReg(0x19,0x000);    //默认，不启用
    SPI_WriteReg(0x1B,0x000);    //暂时不启用
    SPI_WriteReg(0x1C,0x000);    //暂时不启用
    SPI_WriteReg(0x1D,0x000);    //暂时不启用
    SPI_WriteReg(0x1E,0x000);    //暂时不启用
    SPI_WriteReg(0x20,0x000);    //ALC1,暂时默认，不启用
                                 //若启用ALC，ALC监控ADC的输出，调整增益，并反馈给PGA
    SPI_WriteReg(0x21,0x000);    //ALC2,暂时默认，不启用
    SPI_WriteReg(0x22,0x000);    //ALC3,暂时默认，不启用
    SPI_WriteReg(0x23,0x000);
    SPI_WriteReg(0x24,0x00C);    //MCLK进入不进行2分频，并且N = 8，K = 0
    SPI_WriteReg(0x25,0x00C);
    SPI_WriteReg(0x26,0x0DD);
    SPI_WriteReg(0x27,0x05E);
    SPI_WriteReg(0x28,0x000);    //扬声混合器的旁路路径衰减-10dB， MONO  mixer input为0dB
    SPI_WriteReg(0x2C,0x004);    //MIC+ 和 MIC-连接到PGA正端和负端，采用差分输入模式，有效抑制共模噪声
                                 //(addr0x3A，MICBIASM[4] = 0)麦克风偏置电压为0.9*3.3(VDDA) = 2.97V
    SPI_WriteReg(0x2D,0x0B2);    //可编程器放大增益设置为18dB，PGA正常输出，不静音，若静音了则信号不会被放大或者传输到后续电路中
                                 //不启用零交叉检测，当增益寄存器更改时，更新增益
    SPI_WriteReg(0x31,0x002);    //mono输出和扬声器输出增益提升都不启用，启用过热保护，模拟输出阻抗~1kΩ
    SPI_WriteReg(0x2F,0x100);    //PGA输出增益为20dB，AUX和MIC都不连接到boost
                                 //如果MIC+引脚要到输入提升阶段，需要设置PMICPAG[0] = 0(0x2C)
    SPI_WriteReg(0x31,0x002);    //mono输出和扬声器输出增益提升都不启用，启用过热保护，模拟输出阻抗~1kΩ
    SPI_WriteReg(0x32,0x001);    //DAC 和 BYPASS 到扬声器混合器输出，AUX不连接到扬声器输出
    SPI_WriteReg(0x38,0x040);    //DAC to MONO Mixer，AUX 和 BYPASS都不连接到MONO Mixer，并且MOUT不静音
}

void BSP_CodecOpenMicrophone(void)
{
    SPI_WriteReg(WM8974_POWER_MANAGE1,    0x00F);   //1,Buffer Enabled, Auxilliary input buffer disable
    SPI_WriteReg(WM8974_POWER_MANAGE2,    0x015);   //2,Boost stage ON,ADC and boost work
    SPI_WriteReg(WM8974_POWER_MANAGE3,    0x000);   //3,DAC, Speak out Not work
    SPI_WriteReg(WM8974_INPUT_CTRL,       0x003);       //44, Microphone Bias=3*.9=2.7V,
    SPI_WriteReg(WM8974_INPPGA_GAIN_CTRL, 0x01A);       //45, maximum 18dB input gain
    SPI_WriteReg(WM8974_SPK_MIXER_CTRL,   0x000);   //50, DAC to SPK
    SPI_WriteReg(WM8974_OUTPUT_CTRL,      0x003);       //49, Mono output boost off, Speaker output boost off,thermal shutdown enabled
}

static const unsigned short mSpeakVolume[16]=
{
    0x0A1,0x0A3,0x0A5,0x0A7,0x0A9,0x0AB,0x0AD,0x0AF,
    0x0B1,0x0B3,0x0B5,0x0B7,0x0B9,0x0BB,0x0BD,0x0BF
};

void BSP_CodecSpeakerVolume(int Vol)
{
    SPI_WriteReg(WM8974_SPK_VOLUME_CTRL, mSpeakVolume[Vol&0x0F]);		//54, Speaker value
}

void BSP_CodecOpenSpeaker(void)
{
    SPI_WriteReg(WM8974_POWER_MANAGE1,    0x00F);	//1,Buffer disabled, Auxiliary input buffer disable
    SPI_WriteReg(WM8974_POWER_MANAGE2,    0x000);	//2,ADC off, Input PGA off, Boost stage OFF
    SPI_WriteReg(WM8974_POWER_MANAGE3,    0x065);	//3,DAC on, Mono off and Speak out on
    SPI_WriteReg(WM8974_INPUT_CTRL,       0x000);		//44, Microphone Bias=3*.9=2.7V,
    SPI_WriteReg(WM8974_INPPGA_GAIN_CTRL, 0x000);		//45, maximum 18dB input gain
    SPI_WriteReg(WM8974_OUTPUT_CTRL,      0x002);		//49, Speaker output not boost on,thermal shutdown enabled
    SPI_WriteReg(WM8974_SPK_MIXER_CTRL,   0x001);	//50, Output of DAC to speaker mixer input
    BSP_CodecSpeakerVolume(8);
}

void BSP_CodecOff(void)
{
    SPI_WriteReg(WM8974_POWER_MANAGE1,    0x000);	//1,Buffer disabled, Out4 for VOX
    SPI_WriteReg(WM8974_POWER_MANAGE2,    0x000);	//2,Boost stage ON,ADC Not Work, and boost work
    SPI_WriteReg(WM8974_POWER_MANAGE3,    0x000);	//3,out4 Enabled,  DAC and Speak out not work
    SPI_WriteReg(WM8974_INPUT_CTRL,       0x003);		//44, Microphone Bias=3*.9=2.7V,
    SPI_WriteReg(WM8974_INPPGA_GAIN_CTRL, 0x098);		//45, maximum 18dB input gain
}

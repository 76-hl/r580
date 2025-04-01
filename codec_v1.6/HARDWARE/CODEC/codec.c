/**
   ******************************************************************************
   * @file    codec.c
   * @author
   * @version v1
   * @date
   * @brief   ��Ƶ����
   ******************************************************************************
**/

#include "codec.h"

/**
   ******************************************************************************
   * @brief    SPI3��ʼ��
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

    // 1. ����SPI��GPIOʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // 2. ����SPI���ţ�PC10: SCK, PC12: MOSI��
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_SetBits(GPIOC, GPIO_Pin_10);  // SCK���иߵ�ƽ������SCKΪ�ߵ�ƽ

    // 4. ����CS���ţ�PA15��
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);  // CS���иߵ�ƽ������CSΪ�ߵ�ƽ

    // 5.���õ���˫������ѡ������
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);  // ���øߵ�ƽ
}

/**
   ******************************************************************************
   * @brief    д�Ĵ�������
   *
   * @note     ����ʱҪƬѡ���ű���Ҫ�ӵͱ��
   *           SPI�����ֲ�ͬ�Ľṹ��һ����16bit(Ĭ��)��һ����24bit
   *           16bit��7bit�Ĵ�����ַ + 9bit�Ĵ�������
   *           24bit��8bit�豸��ַ + 7bit�Ĵ�����ַ + 9bit����
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

        MYSPI_SCK(0);//�½��س���
        delay_us(5);

        if(i==15)
        {
            MYSPI_CS(0);//��ǰ����Ƭѡ
        }

        MYSPI_SCK(1);
        delay_us(5);
    }

    MYSPI_CS(1);
}

/**
   ******************************************************************************
   * @brief    NAU9914�Ĵ�������
   *
   * @note     ���������ֲ��������
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
    SPI_WriteReg(0x04,0x118);    //ADC��DAC������֡ʱ����࣬I2S���䣬16λ��BCLK��FS��Ϊ�½��ش���
    SPI_WriteReg(0x05,0x000);    //��Ƶ�ӿ�ѹ���Ĵ�������Ƶ��ѹ����������
    SPI_WriteReg(0x06,0x0AD);    //������ʱ��ģʽ,����PLL����Ƶϵ��Ϊ12
    SPI_WriteReg(0x07,0x00A);    //����Ƶ��8kHz����������ʱ��
    SPI_WriteReg(0x08,0x000);    //��ΪSPI�ӿڣ���Ƭѡ���ţ�����ΪCsb Input��PLL���ʱ�Ӳ���Ƶ
    SPI_WriteReg(0x0A,0x008);    //����Ĭ�ϣ��������Զ�����������������������Ҫ��ʱ���ڽ�������
                                 //oversample����Ϊ128x��Ϊ��������
    SPI_WriteReg(0x0B,0x0FF);    //����Ϊ0dB
    SPI_WriteReg(0x0E,0x1F8);    //����Ĭ�ϣ���������Ϊ128x��Ϊ��������
                                 //����Ӧ��ģʽ����ʱ��ͨ�˲����Ƕ��׵ģ���ֹƵ��Ϊ327Hz������Ƶ�������Ƶ�������������Ƶ��Ϊ8kHz
                                 //HPFEN[8]��1��ʹ�ܸ�ͨ�˲���
    SPI_WriteReg(0x0F,0x0FF);    //����Ϊ0dB
    SPI_WriteReg(0x12,0x078);    //EQ1������Ƶ��Ϊ175 ������Ϊ0dB��DACͨ��
    SPI_WriteReg(0x13,0x000);    //EQ2������Ƶ��Ϊ300 ������Ϊ6dB��DACͨ��
    SPI_WriteReg(0x14,0x000);    //EQ3������Ƶ��Ϊ1.4k������Ϊ9dB��DACͨ��
    SPI_WriteReg(0x15,0x000);    //EQ4������Ƶ��Ϊ4.1k������Ϊ6dB��DACͨ��
    SPI_WriteReg(0x16,0x000);    //EQ5������Ƶ��Ϊ9.0k������Ϊ3dB��DACͨ��
    SPI_WriteReg(0x18,0x000);    //Ĭ�ϣ�������
    SPI_WriteReg(0x19,0x000);    //Ĭ�ϣ�������
    SPI_WriteReg(0x1B,0x000);    //��ʱ������
    SPI_WriteReg(0x1C,0x000);    //��ʱ������
    SPI_WriteReg(0x1D,0x000);    //��ʱ������
    SPI_WriteReg(0x1E,0x000);    //��ʱ������
    SPI_WriteReg(0x20,0x000);    //ALC1,��ʱĬ�ϣ�������
                                 //������ALC��ALC���ADC��������������棬��������PGA
    SPI_WriteReg(0x21,0x000);    //ALC2,��ʱĬ�ϣ�������
    SPI_WriteReg(0x22,0x000);    //ALC3,��ʱĬ�ϣ�������
    SPI_WriteReg(0x23,0x000);
    SPI_WriteReg(0x24,0x00C);    //MCLK���벻����2��Ƶ������N = 8��K = 0
    SPI_WriteReg(0x25,0x00C);
    SPI_WriteReg(0x26,0x0DD);
    SPI_WriteReg(0x27,0x05E);
    SPI_WriteReg(0x28,0x000);    //�������������··��˥��-10dB�� MONO  mixer inputΪ0dB
    SPI_WriteReg(0x2C,0x004);    //MIC+ �� MIC-���ӵ�PGA���˺͸��ˣ����ò������ģʽ����Ч���ƹ�ģ����
                                 //(addr0x3A��MICBIASM[4] = 0)��˷�ƫ�õ�ѹΪ0.9*3.3(VDDA) = 2.97V
    SPI_WriteReg(0x2D,0x0B2);    //�ɱ�����Ŵ���������Ϊ18dB��PGA����������������������������źŲ��ᱻ�Ŵ���ߴ��䵽������·��
                                 //�������㽻���⣬������Ĵ�������ʱ����������
    SPI_WriteReg(0x31,0x002);    //mono�����������������������������ã����ù��ȱ�����ģ������迹~1k��
    SPI_WriteReg(0x2F,0x100);    //PGA�������Ϊ20dB��AUX��MIC�������ӵ�boost
                                 //���MIC+����Ҫ�����������׶Σ���Ҫ����PMICPAG[0] = 0(0x2C)
    SPI_WriteReg(0x31,0x002);    //mono�����������������������������ã����ù��ȱ�����ģ������迹~1k��
    SPI_WriteReg(0x32,0x001);    //DAC �� BYPASS ������������������AUX�����ӵ����������
    SPI_WriteReg(0x38,0x040);    //DAC to MONO Mixer��AUX �� BYPASS�������ӵ�MONO Mixer������MOUT������
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

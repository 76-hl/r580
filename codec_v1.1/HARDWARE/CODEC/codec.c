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
void SPI_WriteReg(uint8_t addr,uint16_t data)
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

/**
   ******************************************************************************
   * @brief    NAU9914�Ĵ�������
   *   
   * @note 	   ���������ֲ��������
   *           
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void NAU8814_Init(void)
{	
	//ʹ��
	
	
	
	//��Ҫ��0x01��ʹ��MICBIASEN��MICBIASEN[4]��1
	//1.Input Signal Control Register �����źſ��ƼĴ���
	SPI_WriteReg(0x2C,0x003);    //MIC+ �� MIC-���ӵ�PGA���˺͸��ˣ����ò������ģʽ����Ч���ƹ�ģ����
	                             //(addr0x3A��MICBIASM[4] = 0)��˷�ƫ�õ�ѹΪ0.9*3.3(VDDA) = 2.97V
	
	//��Ҫ��0x02��ʹ��PGA��PGAEN[2]��1
	//2.PGA Gain Control RegisterPGA PAG������ƼĴ���
	SPI_WriteReg(0x2D,0x028);    //�ɱ�����Ŵ���������Ϊ18dB��PGA����������������������������źŲ��ᱻ�Ŵ���ߴ��䵽������·��
	                             //�������㽻���⣬������Ĵ�������ʱ����������
	                             
	//��Ҫ��0x02��ʹ��BOOST��BSTEN[4]��1
	//3.ADC Boost Control Registers ADC��ѹ���ƼĴ���
	SPI_WriteReg(0x2F,0x100);    //PGA�������Ϊ20dB��AUX��MIC�������ӵ�boost
	                             //���MIC+����Ҫ�����������׶Σ���Ҫ����PMICPAG[0] = 0(0x2C)
	                         
	
	
	//��Ҫ��0x02��ʹ��ADC��ADCEN[0]��1
	//4.ADC Control Register ADC���ƼĴ���
	SPI_WriteReg(0x0E,0x1e8);    //����Ĭ�ϣ���������Ϊ128x��Ϊ��������
	                             //����Ӧ��ģʽ����ʱ��ͨ�˲����Ƕ��׵ģ���ֹƵ��Ϊ327Hz������Ƶ�������Ƶ�������������Ƶ��Ϊ8kHz
	                             //HPFEN[8]��1��ʹ�ܸ�ͨ�˲���
	                             
	//5.NOTCH FILTER REGISTERS ȱ���˲����Ĵ���
	SPI_WriteReg(0x1B,0x000);    //��ʱ������
	SPI_WriteReg(0x1C,0x000);    //��ʱ������
	SPI_WriteReg(0x1D,0x000);    //��ʱ������
	SPI_WriteReg(0x1E,0x000);    //��ʱ������
	
	//6.ADC Gain Control Register ADC������ƼĴ���
	SPI_WriteReg(0x0F,0x0FF);    //����Ϊ0dB
	
	//7.ALC REGISTER �Զ���ƽ���ƼĴ���
	SPI_WriteReg(0x20,0x038);    //ALC1,��ʱĬ�ϣ�������
	                             //������ALC��ALC���ADC��������������棬��������PGA
	SPI_WriteReg(0x21,0x00B);    //ALC2,��ʱĬ�ϣ�������
	SPI_WriteReg(0x22,0x032);    //ALC3,��ʱĬ�ϣ�������
	
	
	
	//8.DAC Control Register DAC���ƼĴ���
	//��Ҫ��0x03��ʹ��DAC��DACEN[0]��1
	SPI_WriteReg(0x0A,0x008);    //����Ĭ�ϣ��������Զ�����������������������Ҫ��ʱ���ڽ�������
	                             //oversample����Ϊ128x��Ϊ��������
								 
	//9.DAC Gain Control Register DAC������ƼĴ���				 
	SPI_WriteReg(0x0B,0x0FF);    //����Ϊ0dB

	//10.DIGITAL TO ANALOG CONVERTER (DAC) LIMITER REGISTERS ��ģת������DAC���������Ĵ���
	SPI_WriteReg(0x18,0x032);    //Ĭ�ϣ�������
	SPI_WriteReg(0x19,0x000);    //Ĭ�ϣ�������
								 
	//11.5-BAND EQUALIZER CONTROL REGISTERS5 5���ξ��������ƼĴ���
	SPI_WriteReg(0x12,0x16C);    //EQ1������Ƶ��Ϊ175 ������Ϊ0dB��DACͨ��
	SPI_WriteReg(0x13,0x026);    //EQ2������Ƶ��Ϊ300 ������Ϊ6dB��DACͨ��
	SPI_WriteReg(0x14,0x063);    //EQ3������Ƶ��Ϊ1.4k������Ϊ9dB��DACͨ��
	SPI_WriteReg(0x15,0x066);    //EQ4������Ƶ��Ϊ4.1k������Ϊ6dB��DACͨ��
	SPI_WriteReg(0x16,0x04C);    //EQ5������Ƶ��Ϊ9.0k������Ϊ3dB��DACͨ��
	
	//12.Attenuation Control Register ˥�����ƼĴ���
	SPI_WriteReg(0x28,0x002);    //�������������··��˥��-10dB�� MONO  mixer inputΪ0dB
	
	
	
	//13.GPIO Control Register GPIO���ƼĴ���
	SPI_WriteReg(0x08,0x000);    //��ΪSPI�ӿڣ���Ƭѡ���ţ�����ΪCsb Input��PLL���ʱ�Ӳ���Ƶ
	
	//14.Audio Sample Rate Control Register ��Ƶ�������ʿ��ƼĴ���
	SPI_WriteReg(0x07,0x00a);    //����Ƶ��8kHz����������ʱ��
	
	//Ҫʹ��PLL
	//15.PLL Control Registers����PLL�Ĵ���
	SPI_WriteReg(0x24,0x008);    //MCLK���벻����2��Ƶ������N = 8��K = 0
	SPI_WriteReg(0x25,0x000);
	SPI_WriteReg(0x26,0x000);
	SPI_WriteReg(0x27,0x000);
	
	//16.Clock Control Register ����ʱ�ӼĴ���
	SPI_WriteReg(0x06,0x1e1);    //������ʱ��ģʽ,����PLL����Ƶϵ��Ϊ12
	
	//17.MISC CONTROL REGISTER ������ƼĴ���
	SPI_WriteReg(0x49,0x000);    //����SPIEN[8],��mode pinȥѡ��I2C����SPIģʽ
	                             //mode pin�Ѿ��Ӹߵ�ƽ��SPIEN[8]��Ϊ0��ѡ��SPI 16bitģʽ
	
	//18.Audio Interface Control ��Ƶ���ƼĴ���
	SPI_WriteReg(0x04,0x190);    //ADC��DAC������֡ʱ����࣬I2S���䣬16λ��BCLK��FS��Ϊ�½��ش���
	
	//19.Audio Interface Companding Control ��Ƶ�ӿ�ѹ������
	SPI_WriteReg(0x05,0x000);    //��Ƶ�ӿ�ѹ���Ĵ�������Ƶ��ѹ����������
	
	
	
	
	//��Ҫ��0x03��ʹ��SPKMIX��SPKMXEN[2]��1
	//20.Speaker Mixer Control Register ��������������ƼĴ���
	SPI_WriteReg(0x32,0x003);    //DAC �� BYPASS ������������������AUX�����ӵ����������
	
	//21.Speaker Gain Control Register ������������ƼĴ���
	SPI_WriteReg(0x36,0x0b9);    //����������Ϊ0dB��ʹ�������������̸�������
	
	//��Ҫ��0x03��ʹ��MOUT��MOUTMXEN[3]��1
	//22.MONO Mixer Control Register MONO��Ƶ�����ƼĴ���
	SPI_WriteReg(0x38,0x001);    //DAC to MONO Mixer��AUX �� BYPASS�������ӵ�MONO Mixer������MOUT������
	
	//23.Output Register ����Ĵ���
	SPI_WriteReg(0x31,0x002);    //mono�����������������������������ã����ù��ȱ�����ģ������迹~1k��
}




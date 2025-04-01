#ifndef __CODEC_H
#define __CODEC_H

#include "stm32f4xx_conf.h"
#include "sys.h"
#include "delay.h"


#define MYSPI_CS(n) (n) ? GPIO_SetBits(GPIOA, GPIO_Pin_15) : GPIO_ResetBits(GPIOA, GPIO_Pin_15)
#define MYSPI_SCK(n) (n) ? GPIO_SetBits(GPIOC, GPIO_Pin_10) : GPIO_ResetBits(GPIOC, GPIO_Pin_10)
#define MYSPI_MOSI(n) (n) ? GPIO_SetBits(GPIOC, GPIO_Pin_12) : GPIO_ResetBits(GPIOC, GPIO_Pin_12)


void SPI3_Init(void);
void SPI_WriteReg(uint8_t addr,uint16_t val);
void NAU8814_Init(void);
void BSP_CodecOpenMicrophone(void);
void BSP_CodecSpeakerVolume(int Vol);
void BSP_CodecOpenSpeaker(void);
void BSP_CodecOff(void);

#define CodecICAddress 0x34

#define WM8974_SOFTWARE_RESET   0   //
#define WM8974_POWER_MANAGE1    1   //
#define WM8974_POWER_MANAGE2    2   //
#define WM8974_POWER_MANAGE3    3   //
#define WM8974_AUDIO_INTERFACE  4   //
#define WM8974_COMPANDING_CTRL  5   //
#define WM8974_CLOCK_GEN_CTRL   6   //
#define WM8974_ADDITIONAL_CTRL  7   //
#define WM8974_GPIO             8   //
#define WM8974_DAC_CONTROL      10  //
#define WM8974_DAC_DIGITAL_VOL  11  //
#define WM8974_ADC_CONTROL      14  //
#define WM8974_ADC_DIGITAL_VOL  15  //
#define WM8974_EQ1              18  //
#define WM8974_EQ2              19  //
#define WM8974_EQ3              20  //
#define WM8974_EQ4              21  //
#define WM8974_EQ5              22  //
#define WM8974_DAC_LIMITER1     24  //
#define WM8974_DAC_LIMITER2     25  //
#define WM8974_NOTCH_FILTER1    27  //
#define WM8974_NOTCH_FILTER2    28  //
#define WM8974_NOTCH_FILTER3    29  //
#define WM8974_NOTCH_FILTER4    30  //
#define WM8974_ALC_CONTROL1     32  //
#define WM8974_ALC_CONTROL2     33  //
#define WM8974_ALC_CONTROL3     34  //
#define WM8974_NOISE_GATE       35  //
#define WM8974_PLL_N            36  //
#define WM8974_PLL_K1           37  //
#define WM8974_PLL_K2           38  //
#define WM8974_PLL_K3           39  //
#define WM8974_ATTENUATION_CTRL 40  //
#define WM8974_INPUT_CTRL       44  //
#define WM8974_INPPGA_GAIN_CTRL 45  //
#define WM8974_ADC_BOOST_CTRL   47  //
#define WM8974_OUTPUT_CTRL      49  //
#define WM8974_SPK_MIXER_CTRL   50  //
#define WM8974_SPK_VOLUME_CTRL  54  //
#define WM8974_MONO_MIXER_CTRL  56  //

#endif

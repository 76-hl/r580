#ifndef MR_NAU881X_REGS_H
#define MR_NAU881X_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define NAU881X_I2C_ADDRESS (0x1A << 1)

#define NAU881X_PART_NAU8810 0
#define NAU881X_PART_NAU8814 1

typedef enum _nau881x_register
{
    NAU881X_REG_SOFTWARE_RESET = 0,
    NAU881X_REG_POWER_MANAGEMENT_1 = 1,
    NAU881X_REG_POWER_MANAGEMENT_2 = 2,
    NAU881X_REG_POWER_MANAGEMENT_3 = 3,
    NAU881X_REG_AUDIO_INTERFACE = 4,
    NAU881X_REG_COMPANDING_CTRL = 5,
    NAU881X_REG_CLOCK_CTRL_1 = 6,
    NAU881X_REG_CLOCK_CTRL_2 = 7,
    NAU8814_REG_GPIO_CTRL = 8,

    NAU881X_REG_DAC_CTRL = 10,
    NAU881X_REG_DAC_VOL = 11,

    NAU881X_REG_ADC_CTRL = 14,
    NAU881X_REG_ADC_VOL = 15,

    NAU881X_REG_EQ_1 = 18,
    NAU881X_REG_EQ_2 = 19,
    NAU881X_REG_EQ_3 = 20,
    NAU881X_REG_EQ_4 = 21,
    NAU881X_REG_EQ_5 = 22,
    
    NAU881X_REG_DAC_LIMITER_1 = 24,
    NAU881X_REG_DAC_LIMITER_2 = 25,

    NAU881X_REG_NOTCH_FILTER_0_H = 27,
    NAU881X_REG_NOTCH_FILTER_0_L = 28,
    NAU881X_REG_NOTCH_FILTER_1_H = 29,
    NAU881X_REG_NOTCH_FILTER_1_L = 30,

    NAU881X_REG_ALC_CTRL_1 = 32,
    NAU881X_REG_ALC_CTRL_2 = 33,
    NAU881X_REG_ALC_CTRL_3 = 34,
    NAU881X_REG_NOISE_GATE = 35,
	
    NAU881X_REG_PLL_N = 36,
    NAU881X_REG_PLL_K1 = 37,
    NAU881X_REG_PLL_K2 = 38,
    NAU881X_REG_PLL_K3 = 39,
	
    NAU881X_REG_ATTN_CTRL = 40,

    NAU881X_REG_INPUT_CTRL = 44,
    NAU881X_REG_PGA_GAIN_CTRL = 45,

    NAU881X_REG_ADC_BOOST_CTRL = 47,

    NAU881X_REG_OUTPUT_CTRL = 49,
    NAU881X_REG_SPK_MIXER_CTRL = 50,

    NAU881X_REG_SPK_VOL_CTRL = 54,

    NAU881X_REG_MONO_MIXER_CTRL = 56,

    NAU881X_REG_POWER_MANAGEMENT_4 = 58,
    NAU881X_REG_PCM_TIMESLOT = 59,
    NAU881X_REG_ADCOUT_DRIVE = 60,

    NAU881X_REG_SILICON_REV = 62,
    NAU881X_REG_2WIRE_ID = 63,
    NAU881X_REG_ADDITIONAL_ID = 64,

    NAU881X_REG_HIGH_VOLTAGE_CTRL = 69,
    NAU881X_REG_ALC_ENHANCEMENTS_1 = 70,
    NAU881X_REG_ALC_ENHANCEMENTS_2 = 71,

    NAU881X_REG_ADDITIONAL_IF_CTRL = 73,

    NAU881X_REG_POWER_TIE_OFF_CTRL = 75,
    NAU881X_REG_AGC_P2P_DET = 76,
    NAU881X_REG_AGC_PEAK_DET = 77,
    NAU881X_REG_CTRL_AND_STATUS = 78,
    NAU881X_REG_OUTPUT_TIE_OFF_CTRL = 79
} nau881x_register_t;

///////////////////////////////////////////////////////////////////////////////////////
typedef enum 
{
	RESETBIT = 0,
	SETBIT   = 1
}nau881x_state;


typedef enum 
{
	SLAVE    = 0,
	MASTER   = 1
}nau881x_mode;


typedef enum _nau881x_samplerate
{
    NAU881X_SAMPLERATE_48KHZ = 0,
    NAU881X_SAMPLERATE_32KHZ = 1,
    NAU881X_SAMPLERATE_24KHZ = 2,
    NAU881X_SAMPLERATE_16KHZ = 3,
	NAU881X_SAMPLERATE_12KHZ = 4,
	NAU881X_SAMPLERATE_8KHZ = 5
} nau881x_samplerate_t;


typedef enum 
{
	EQ1 = 1,
	EQ2 = 2,
	EQ3 = 3,
	EQ4 = 4,
	EQ5 = 5
}nau881x_eqnum;
///////////////////////////////////////////////////////////////////////////////////////

typedef enum _nau881x_input
{
    NAU881X_INPUT_NONE      = 0,
    NAU881X_INPUT_MICP      = 1,
    NAU881X_INPUT_MICN      = 2,
    NAU8814_INPUT_AUX       = 4
} nau881x_input_t;

typedef enum _nau881x_aux_mode
{
    NAU881X_AUXMODE_INVERTING = 0,
    NAU881X_AUXMODE_MIXER = 1
} nau881x_aux_mode_t;

typedef enum _nau881x_adc_oversamplerate
{
    NAU881X_ADC_OVERSAMPLE_64X = 0,
    NAU881X_ADC_OVERSAMPLE_128X = 1
} nau881x_adc_oversamplerate_t;

typedef enum _nau881x_hpf_mode
{
    NAU881X_HPF_MODE_AUDIO = 0,
    NAU881X_HPF_MODE_APP = 1
} nau881x_hpf_mode_t;

typedef enum _nau881x_alc_mode
{
    NAU881X_ALC_MODE_NORMAL = 0,
    NAU881X_ALC_MODE_LIMITER = 1
} nau881x_alc_mode_t;


typedef enum _nau881x_dac_oversamplerate
{
    NAU881X_DAC_OVERSAMPLE_64X = 0,
    NAU881X_DAC_OVERSAMPLE_128X = 1
} nau881x_dac_oversamplerate_t;

typedef enum _nau881x_dac_samplerate
{
    NAU881X_DAC_SAMPLERATE_NO_DEEMPHASIS = 0,
    NAU881X_DAC_SAMPLERATE_32KHZ = 1,
    NAU881X_DAC_SAMPLERATE_44KHZ = 2,
    NAU881X_DAC_SAMPLERATE_48KHZ = 3
} nau881x_dac_samplerate_t;

typedef enum _nau881x_eq_path
{
    NAU881X_EQ_PATH_ADC = 0,
    NAU881X_EQ_PATH_DAC = 1
} nau881x_eq_path_t;

typedef enum _nau881x_eq_bandwidth
{
    NAU881X_EQ_BW_NARROW = 0,
    NAU881X_EQ_BW_WIDE = 1
} nau881x_eq_bandwidth_t;

typedef enum _nau881x_eq1_cutoff_freq
{
    NAU881X_EQ1_CUTOFF_80HZ = 0,
    NAU881X_EQ1_CUTOFF_105HZ = 1,
    NAU881X_EQ1_CUTOFF_135HZ = 2,
    NAU881X_EQ1_CUTOFF_175HZ = 3
} nau881x_eq1_cutoff_freq_t;

typedef enum _nau881x_eq2_center_freq
{
    NAU881X_EQ2_CENTER_230HZ = 0,
    NAU881X_EQ2_CENTER_300HZ = 1,
    NAU881X_EQ2_CENTER_385HZ = 2,
    NAU881X_EQ2_CENTER_500HZ = 3
} nau881x_eq2_center_freq_t;

typedef enum _nau881x_eq3_center_freq
{
    NAU881X_EQ3_CENTER_650HZ = 0,
    NAU881X_EQ3_CENTER_850HZ = 1,
    NAU881X_EQ3_CENTER_1100HZ = 2,
    NAU881X_EQ3_CENTER_1400HZ = 3
} nau881x_eq3_center_freq_t;

typedef enum _nau881x_eq4_center_freq
{
    NAU881X_EQ4_CENTER_1800HZ = 0,
    NAU881X_EQ4_CENTER_2400HZ = 1,
    NAU881X_EQ4_CENTER_3200HZ = 2,
    NAU881X_EQ4_CENTER_4100HZ = 3
} nau881x_eq4_center_freq_t;

typedef enum _nau881x_eq5_cutoff_freq
{
    NAU881X_EQ5_CUTOFF_5300HZ = 0,
    NAU881X_EQ5_CUTOFF_6900HZ = 1,
    NAU881X_EQ5_CUTOFF_9000HZ = 2,
    NAU881X_EQ5_CUTOFF_11700HZ = 3
} nau881x_eq5_cutoff_freq_t;

typedef enum _nau881x_output_source
{
    NAU881X_OUTPUT_FROM_NONE = 0,
    NAU881X_OUTPUT_FROM_DAC = (1 << 0),
    NAU881X_OUTPUT_FROM_BYPASS = (1 << 1),
    NAU8814_OUTPUT_FROM_AUX = (1 << 2)
} nau881x_output_source_t;

typedef enum _nau881x_output
{
    NAU881X_OUTPUT_NONE = 0,
    NAU881X_OUTPUT_SPK = (1 << 0),
    NAU881X_OUTPUT_MOUT = (1 << 1),
    NAU881X_OUTPUT_MOUT_DIFFERENTIAL = (1 << 2)
} nau881x_output_t;

typedef enum _nau8814_gpio_function
{
    NAU8814_GPIO_FUNCTION_CSB_INPUT = 0,
    NAU8814_GPIO_FUNCTION_JACK_DETECT = 1,
    NAU8814_GPIO_FUNCTION_TEMP_OK = 2,
    NAU8814_GPIO_FUNCTION_AMUTE_ACTIVE = 3,
    NAU8814_GPIO_FUNCTION_PLL_FREQUENCY_OUTPUT = 4,
    NAU8814_GPIO_FUNCTION_PLL_LOCK = 5,
    NAU8814_GPIO_FUNCTION_HIGH = 6,
    NAU8814_GPIO_FUNCTION_LOW = 7
} nau8814_gpio_function_t;

// See Digital Audio Interfaces section in the datasheet for more details
typedef enum _nau881x_audio_iface_fmt
{
    NAU881X_AUDIO_IFACE_FMT_RIGHT_JUSTIFIED = 0,
    NAU881X_AUDIO_IFACE_FMT_LEFT_JUSTIFIED = 1,
    NAU881X_AUDIO_IFACE_FMT_I2S = 2,
    NAU881X_AUDIO_IFACE_FMT_PCM_A = 3,
    NAU881X_AUDIO_IFACE_FMT_PCM_B = 0x0100,         
    NAU881X_AUDIO_IFACE_FMT_PCM_TIMESLOT = 0x1011   
}nau881x_audio_iface_fmt_t;

typedef enum _nau881x_audio_iface_wl
{
    NAU881X_AUDIO_IFACE_WL_16BITS = 0,
    NAU881X_AUDIO_IFACE_WL_20BITS = 1,
    NAU881X_AUDIO_IFACE_WL_24BITS = 2,
    NAU881X_AUDIO_IFACE_WL_32BITS = 3,
    NAU881X_AUDIO_IFACE_WL_8BITS = 4
} nau881x_audio_iface_wl_t;

typedef enum _nau881x_bclkdiv
{
    NAU881X_BCLKDIV_1 = 0,
    NAU881X_BCLKDIV_2 = 1,
    NAU881X_BCLKDIV_4 = 2,
    NAU881X_BCLKDIV_8 = 3,
    NAU881X_BCLKDIV_16 = 4,
    NAU881X_BCLKDIV_32 = 5 
} nau881x_bclkdiv_t;

typedef enum _nau881x_mclkdiv
{
    NAU881X_MCLKDIV_1 = 0,
    NAU881X_MCLKDIV_1HALF = 1,
    NAU881X_MCLKDIV_2 = 2,
    NAU881X_MCLKDIV_3 = 3,
    NAU881X_MCLKDIV_4 = 4,
    NAU881X_MCLKDIV_6 = 5,
    NAU881X_MCLKDIV_8 = 6,
    NAU881X_MCLKDIV_12 = 7
} nau881x_mclkdiv_t;

typedef enum _nau881x_clksel
{
    NAU881X_CLKSEL_MCLK = 0,
    NAU881X_CLKSEL_PLL = 1
} nau881x_clksel_t;

typedef enum _nau881x_companding
{
    NAU881X_COMPANDING_OFF = 0,
    NAU881X_COMPANDING_ALAW = 2,
    NAU881X_COMPANDING_ULAW = 3
} nau881x_companding_t;

typedef enum _nau881x_power_management_3
{
    NAU881X_PM3_DAC = (1 << 0),
    NAU881X_PM3_SPKMIX = (1 << 2),
    NAU881X_PM3_MOUTMIX = (1 << 3),
    NAU881X_PM3_SPKP = (1 << 5),
    NAU881X_PM3_SPKN = (1 << 6),
    NAU881X_PM3_MOUT = (1 << 7)
} nau881x_pm3_t;

#ifdef __cplusplus
}
#endif

#endif // MR_NAU881X_REGS_H

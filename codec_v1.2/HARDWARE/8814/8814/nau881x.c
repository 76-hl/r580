#include "nau881x.h"
#include "nau881x_regs.h"


nau881x_status_t NAU881x_SoftwareReset(NAU881x_t* nau881x);
nau881x_status_t NAU881x_Register_Write(NAU881x_t* nau881x, uint8_t register_addr, uint16_t value);
uint16_t         NAU881x_Register_GetValue(NAU881x_t* nau881x, uint8_t register_addr);
NAU881x_t        nau881x;


nau881x_status_t NAU881x_Init(NAU881x_t* nau881x)
{
    // Datasheet page 64
    NAU881x_SoftwareReset(nau881x);

    // Power up sequence
    // Disable output boost stage for SPK and MOUT
    nau881x->_register[NAU881X_REG_OUTPUT_CTRL] &= ~((1 << 2) | (1 << 3));
    NAU881x_Register_Write(nau881x, NAU881X_REG_OUTPUT_CTRL, nau881x->_register[NAU881X_REG_OUTPUT_CTRL]);

    // Set REFIMP to 80 ohm
    nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_1] |= (1 << 0);
    NAU881x_Register_Write(nau881x, NAU881X_REG_POWER_MANAGEMENT_1, nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_1]);

    // Enable internal device bias (ABIASEN) and bias buffer (IOBUFEN)
    nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_1] |= ((1 << 3) | (1 << 2));
    NAU881x_Register_Write(nau881x, NAU881X_REG_POWER_MANAGEMENT_1, nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_1]);

    return NAU881X_STATUS_OK;
}

/* ----- Input path ----- */
nau881x_status_t NAU881x_Get_PGA_Input(NAU881x_t* nau881x, nau881x_input_t* input)
{
    uint16_t val = nau881x->_register[NAU881X_REG_INPUT_CTRL];
    *input = (nau881x_input_t)(val & 0x0003);
    return NAU881X_STATUS_OK;
}

nau881x_status_t NAU881x_Set_PGA_Input(NAU881x_t* nau881x, nau881x_input_t input)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_INPUT_CTRL];
    regval &= ~(0x0007);            // AUX bits in NAU8810 always zero
    regval |= (uint16_t)input;      // TODO: Return invalid for AUX if NAU8810
    return NAU881x_Register_Write(nau881x, NAU881X_REG_INPUT_CTRL, regval);
}

nau881x_status_t NAU881x_Set_PGA_Gain(NAU881x_t* nau881x, uint8_t val)
{
    if (val > 0x3F)
        return NAU881X_STATUS_INVALID;

    uint16_t regval = nau881x->_register[NAU881X_REG_PGA_GAIN_CTRL];
    regval &= ~(0x003F);
    regval |= (val & 0x003F);
    return NAU881x_Register_Write(nau881x, NAU881X_REG_PGA_GAIN_CTRL, regval);
}

nau881x_status_t NAU881x_Set_PGA_Gain_db(NAU881x_t* nau881x, float vol_db)
{
    if (vol_db < -12)
        return NAU881X_STATUS_INVALID;
    else if (vol_db >= 35.25f)
        return NAU881X_STATUS_INVALID;

    // Round to floor by 0.75
    int16_t rounding = vol_db * 100;
    rounding %= 75;
    if (rounding < 0)
        rounding += 75;
    vol_db -= rounding / 100.0;
    // Convert to val for register
    uint8_t val = (vol_db + 12) / 0.75f;
    return NAU881x_Set_PGA_Gain(nau881x, val);
}

uint8_t NAU881x_Get_PGA_Gain(NAU881x_t* nau881x)
{
    return nau881x->_register[NAU881X_REG_PGA_GAIN_CTRL] & 0x003F;
}

nau881x_status_t NAU881x_Set_PGA_Mute(NAU881x_t* nau881x, uint8_t state)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_PGA_GAIN_CTRL];
    regval &= ~(1 << 6);
    regval |= (state ? 1 : 0) << 6;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_PGA_GAIN_CTRL, regval);
}

nau881x_status_t NAU881x_Set_PGA_ZeroCross(NAU881x_t* nau881x, uint8_t state)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_PGA_GAIN_CTRL];
    regval &= ~(1 << 7);
    regval |= (state ? 1 : 0) << 7;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_PGA_GAIN_CTRL, regval);
}

nau881x_status_t NAU881x_Set_PGA_Enable(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_2];
    regval &= ~(1 << 2);
    regval |= (enable ? 1 : 0) << 2;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_POWER_MANAGEMENT_2, regval);
}

nau881x_status_t NAU8814_Set_Aux_Enable(NAU881x_t* nau8814, uint8_t enable)
{
    uint16_t regval = nau8814->_register[NAU881X_REG_POWER_MANAGEMENT_1];
    regval &= ~(1 << 6);
    regval |= (enable ? 1 : 0) << 6;
    return NAU881x_Register_Write(nau8814, NAU881X_REG_POWER_MANAGEMENT_1, regval);
}

nau881x_status_t NAU8814_Set_Aux_Mode(NAU881x_t* nau8814, nau881x_aux_mode_t mode)
{
    uint16_t regval = nau8814->_register[NAU881X_REG_INPUT_CTRL];
    regval &= ~(1 << 3);
    regval |= mode << 3;
    return NAU881x_Register_Write(nau8814, NAU881X_REG_INPUT_CTRL, regval);
}

nau881x_status_t NAU881x_Set_PGA_Boost(NAU881x_t* nau881x, uint8_t state)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_ADC_BOOST_CTRL];
    regval &= ~(1 << 8);
    regval |= (state ? 1 : 0) << 8;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ADC_BOOST_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Boost_Volume(NAU881x_t* nau881x, nau881x_input_t input, uint8_t vol)
{
    if (vol > 0x07)
        return NAU881X_STATUS_INVALID;

    // TODO: Check if NAU8810 without #if macro because NAU8810 does not have AUX
// #if NAU881X_PART == NAU881X_PART_NAU8810
//     if (input != NAU881X_INPUT_MICP)
//         return NAU881X_STATUS_INVALID;
// #endif

    int8_t shift = -1;
    switch (input)
    {
        case NAU8814_INPUT_AUX: shift = 0; break;
        case NAU881X_INPUT_MICP: shift = 4; break;
        default: break;
    }
    if (shift < 0) return NAU881X_STATUS_INVALID;

    uint16_t regval = nau881x->_register[NAU881X_REG_ADC_BOOST_CTRL];
    regval &= ~(0x07 << shift);
    regval |= (vol & 0x07) << shift;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ADC_BOOST_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Boost_Enable(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_2];
    regval &= ~(1 << 4);
    regval |= (enable ? 1 : 0) << 4;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_POWER_MANAGEMENT_2, regval);
}

nau881x_status_t NAU881x_Set_MicBias_Enable(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_1];
    regval &= ~(1 << 4);
    regval |= (enable ? 1 : 0) << 4;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_POWER_MANAGEMENT_1, regval);
}

nau881x_status_t NAU881x_Set_MicBias_Voltage(NAU881x_t* nau881x, uint8_t val)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_INPUT_CTRL];
    regval &= ~(0x03 << 7);
    regval |= val << 7;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_INPUT_CTRL, regval);
}

nau881x_status_t NAU881x_Set_MicBiasMode_Enable(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_4];
    regval &= ~(1 << 4);
    regval |= (enable ? 1 : 0) << 4;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_POWER_MANAGEMENT_4, regval);
}

/* ----- ADC digital filter ----- */
nau881x_status_t NAU881x_Set_ADC_Enable(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_2];
    regval &= ~(1 << 0);
    regval |= (enable ? 1 : 0) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_POWER_MANAGEMENT_2, regval);
}

nau881x_status_t NAU881x_Set_ADC_Polarity(NAU881x_t* nau881x, uint8_t invert)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_ADC_CTRL];
    regval &= ~(1 << 0);
    regval |= (invert ? 1 : 0) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ADC_CTRL, regval);
}

nau881x_status_t NAU881x_Set_ADC_OverSampleRate(NAU881x_t* nau881x, nau881x_adc_oversamplerate_t rate)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_ADC_CTRL];
    regval &= ~(1 << 3);
    regval |= rate << 3;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ADC_CTRL, regval);
}

nau881x_status_t NAU881x_Set_ADC_HighPassFilter(NAU881x_t* nau881x, uint8_t enable, nau881x_hpf_mode_t mode, uint8_t freq_regval)
{
    if (freq_regval > 0x07)
        return NAU881X_STATUS_INVALID;

    uint16_t regval = nau881x->_register[NAU881X_REG_ADC_CTRL];
    regval &= ~(0x1F << 4);
    regval |= (enable ? 1 : 0) << 8;
    regval |= (mode ? 1 : 0) << 7;
    regval |= (freq_regval & 0x07) << 4;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ADC_CTRL, regval);
}

nau881x_status_t NAU881x_Set_ADC_Gain(NAU881x_t* nau881x, uint8_t regval)
{
    if (regval > 0xFF)
        return NAU881X_STATUS_INVALID;

    uint16_t val = 0;
    val |= (regval & 0xFF) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ADC_VOL, val);
}

/* ----- ALC ----- */
nau881x_status_t NAU881x_Set_ALC_Enable(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_ALC_CTRL_1];
    regval &= ~(1 << 8);
    regval |= (enable ? 1 : 0) << 8;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ALC_CTRL_1, regval);
}

nau881x_status_t NAU881x_Set_ALC_Gain(NAU881x_t* nau881x, uint8_t minval, uint8_t maxval)
{
    if ((minval > 0x07) || (maxval > 0x07))
        return NAU881X_STATUS_INVALID;

    uint16_t regval = nau881x->_register[NAU881X_REG_ALC_CTRL_1];
    regval &= ~(0x3F << 0);
    regval |= (minval & 0x07) << 0;
    regval |= (maxval & 0x07) << 3;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ALC_CTRL_1, regval);
}

nau881x_status_t NAU881x_Set_ALC_TargetLevel(NAU881x_t* nau881x, uint8_t val)
{
    if (val > 0x0F)
        return NAU881X_STATUS_INVALID;
    
    uint16_t regval = nau881x->_register[NAU881X_REG_ALC_CTRL_2];
    regval &= ~(0x0F << 0);
    regval |= (val & 0x0F) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ALC_CTRL_2, regval);
}

nau881x_status_t NAU881x_Set_ALC_Hold(NAU881x_t* nau881x, uint8_t val)
{
    if (val > 0x0F)
        return NAU881X_STATUS_INVALID;
    
    uint16_t regval = nau881x->_register[NAU881X_REG_ALC_CTRL_2];
    regval &= ~(0x0F << 4);
    regval |= (val & 0x0F) << 4;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ALC_CTRL_2, regval);
}

nau881x_status_t NAU881x_Set_ALC_Mode(NAU881x_t* nau881x, nau881x_alc_mode_t mode)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_ALC_CTRL_3];
    regval &= ~(1 << 8);
    regval |= (mode & 0x01) << 8;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ALC_CTRL_3, regval);
}

nau881x_status_t NAU881x_Set_ALC_AttackTime(NAU881x_t* nau881x, uint8_t val)
{
    if (val > 0x0F)
        return NAU881X_STATUS_INVALID;
    
    uint16_t regval = nau881x->_register[NAU881X_REG_ALC_CTRL_3];
    regval &= ~(0x0F << 0);
    regval |= (val & 0x0F) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ALC_CTRL_3, regval);
}

nau881x_status_t NAU881x_Set_ALC_DecayTime(NAU881x_t* nau881x, uint8_t val)
{
    if (val > 0x0F)
        return NAU881X_STATUS_INVALID;
    
    uint16_t regval = nau881x->_register[NAU881X_REG_ALC_CTRL_3];
    regval &= ~(0x0F << 4);
    regval |= (val & 0x0F) << 4;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ALC_CTRL_3, regval);
}

nau881x_status_t NAU881x_Set_ALC_ZeroCross(NAU881x_t* nau881x, uint8_t state)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_ALC_CTRL_2];
    regval &= ~(1 << 8);
    regval |= (state ? 1 : 0) << 8;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ALC_CTRL_2, regval);
}

nau881x_status_t NAU881x_Set_ALC_NoiseGate_Threshold(NAU881x_t* nau881x, uint8_t val)
{
    if (val > 7)
        return NAU881X_STATUS_INVALID;

    uint16_t regval = nau881x->_register[NAU881X_REG_NOISE_GATE];
    regval &= ~(0x07 << 0);
    regval |= (val & 0x07) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_NOISE_GATE, regval);
}

nau881x_status_t NAU881x_Set_ALC_NoiseGate_Enable(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_NOISE_GATE];
    regval &= ~(1 << 3);
    regval |= (enable ? 1 : 0) << 3;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_NOISE_GATE, regval);
}

/* ----- DAC digital filter ----- */
nau881x_status_t NAU881x_Set_ADC_DAC_Passthrough(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_COMPANDING_CTRL];
    regval &= ~(1 << 0);
    regval |= (enable ? 1 : 0) << 1;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_COMPANDING_CTRL, regval);
}

nau881x_status_t NAU881x_Set_DAC_Enable(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_3];
    regval &= ~(1 << 0);
    regval |= (enable ? 1 : 0) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_POWER_MANAGEMENT_3, regval);
}

nau881x_status_t NAU881x_Set_DAC_Polarity(NAU881x_t* nau881x, uint8_t invert)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_DAC_CTRL];
    regval &= ~(1 << 0);
    regval |= (invert ? 1 : 0) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_DAC_CTRL, regval);
}

nau881x_status_t NAU881x_Set_DAC_Gain(NAU881x_t* nau881x, uint8_t val)
{
    uint16_t regval = 0;
    regval |= (val & 0xFF) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_DAC_VOL, regval);
}

nau881x_status_t NAU881x_Set_DAC_SoftMute(NAU881x_t* nau881x, uint8_t state)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_DAC_CTRL];
    regval &= ~(1 << 6);
    regval |= (state ? 1 : 0) << 6;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_DAC_CTRL, regval);
}

nau881x_status_t NAU881x_Set_DAC_AutoMute(NAU881x_t* nau881x, uint8_t state)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_DAC_CTRL];
    regval &= ~(1 << 2);
    regval |= (state ? 1 : 0) << 2;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_DAC_CTRL, regval);
}

nau881x_status_t NAU881x_Set_DAC_SampleRate(NAU881x_t* nau881x, nau881x_dac_samplerate_t rate)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_DAC_CTRL];
    regval &= ~(0x03 << 4);
    regval |= rate << 4;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_DAC_CTRL, regval);
}


nau881x_status_t NAU881x_Set_DAC_OverSampleRate(NAU881x_t* nau881x, nau881x_dac_oversamplerate_t rate)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_DAC_CTRL];
    regval &= ~(1 << 3);
    regval |= rate << 3;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_DAC_CTRL, regval);
}


nau881x_status_t NAU881x_Set_DAC_Limiter_Enable(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_DAC_LIMITER_1];
    regval &= ~(1 << 8);
    regval |= (enable ? 1 : 0) << 8;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_DAC_LIMITER_1, regval);
}

nau881x_status_t NAU881x_Set_DAC_Limiter_AttackTime(NAU881x_t* nau881x, uint8_t val)
{
    if (val > 0x0F)
        return NAU881X_STATUS_INVALID;
    
    uint16_t regval = nau881x->_register[NAU881X_REG_DAC_LIMITER_1];
    regval &= ~(0x0F << 0);
    regval |= (val & 0x0F) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_DAC_LIMITER_1, regval);
}

nau881x_status_t NAU881x_Set_DAC_Limiter_DecayTime(NAU881x_t* nau881x, uint8_t val)
{
    if (val > 0x0F)
        return NAU881X_STATUS_INVALID;
    
    uint16_t regval = nau881x->_register[NAU881X_REG_DAC_LIMITER_1];
    regval &= ~(0x0F << 4);
    regval |= (val & 0x0F) << 4;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_DAC_LIMITER_1, regval);
}

nau881x_status_t NAU881x_Set_DAC_Limiter_VolumeBoost(NAU881x_t* nau881x, uint8_t value)
{
    if (value > 12)
        return NAU881X_STATUS_INVALID;
    uint16_t regval = nau881x->_register[NAU881X_REG_DAC_LIMITER_2];
    regval &= ~(0x0F << 0);
    regval |= (value) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_DAC_LIMITER_2, regval);
}

nau881x_status_t NAU881x_Set_DAC_Limiter_Threshold(NAU881x_t* nau881x, int8_t value)
{
    if ((value >= 0) || (value < -6))
        return NAU881X_STATUS_INVALID;
    uint16_t regval = nau881x->_register[NAU881X_REG_DAC_LIMITER_2];
    value = value * (-1) - 1;
    regval &= ~(0x07 << 4);
    regval |= (value) << 4;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_DAC_LIMITER_2, regval);
}

nau881x_status_t NAU881x_Set_Equalizer_Path(NAU881x_t* nau881x, nau881x_eq_path_t path)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_EQ_1];
    regval &= ~(1 << 8);
    regval |= path << 8;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_EQ_1, regval);
}

nau881x_status_t NAU881x_Set_Equalizer_Bandwidth(NAU881x_t* nau881x, uint8_t equalizer_no, nau881x_eq_bandwidth_t bandwidth)
{
    if ((equalizer_no < 2) || (equalizer_no >= 5))
        return NAU881X_STATUS_INVALID;
    
    uint8_t regnum = NAU881X_REG_EQ_1 + equalizer_no - 1;
    uint16_t regval = nau881x->_register[regnum];
    regval &= ~(1 << 8);
    regval |= bandwidth << 8;
    return NAU881x_Register_Write(nau881x, regnum, regval);
}

nau881x_status_t NAU881x_Set_Equalizer_Gain(NAU881x_t* nau881x, uint8_t equalizer_no, int8_t value)
{
    if ((value < -12) || (value > 12))
        return NAU881X_STATUS_INVALID;
    
    uint8_t val = (value * (-1) + 12) & 0x1F;
    uint8_t regnum = NAU881X_REG_EQ_1 + equalizer_no - 1;
    uint16_t regval = nau881x->_register[regnum];
    regval &= ~(0x1F << 0);
    regval |= val << 0;
    return NAU881x_Register_Write(nau881x, regnum, regval);
}

nau881x_status_t NAU881x_Set_Equalizer1_Frequency(NAU881x_t* nau881x, nau881x_eq1_cutoff_freq_t cutoff_freq)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_EQ_1];
    regval &= ~(0x03 << 5);
    regval |= cutoff_freq << 5;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_EQ_1, regval);
}

nau881x_status_t NAU881x_Set_Equalizer2_Frequency(NAU881x_t* nau881x, nau881x_eq2_center_freq_t center_freq)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_EQ_2];
    regval &= ~(0x03 << 5);
    regval |= center_freq << 5;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_EQ_2, regval);
}

nau881x_status_t NAU881x_Set_Equalizer3_Frequency(NAU881x_t* nau881x, nau881x_eq3_center_freq_t center_freq)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_EQ_3];
    regval &= ~(0x03 << 5);
    regval |= center_freq << 5;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_EQ_3, regval);
}

nau881x_status_t NAU881x_Set_Equalizer4_Frequency(NAU881x_t* nau881x, nau881x_eq4_center_freq_t center_freq)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_EQ_4];
    regval &= ~(0x03 << 5);
    regval |= center_freq << 5;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_EQ_4, regval);
}

nau881x_status_t NAU881x_Set_Equalizer5_Frequency(NAU881x_t* nau881x, nau881x_eq5_cutoff_freq_t cutoff_freq)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_EQ_5];
    regval &= ~(0x03 << 5);
    regval |= cutoff_freq << 5;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_EQ_5, regval);
}

/* ----- Analog outputs ----- */
nau881x_status_t NAU881x_Set_Output_Enable(NAU881x_t* nau881x, nau881x_output_t output)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_3];
    regval &= ~(NAU881X_PM3_SPKMIX | NAU881X_PM3_SPKP | NAU881X_PM3_SPKN);
    regval &= ~(NAU881X_PM3_MOUTMIX | NAU881X_PM3_MOUT);

    if (output & NAU881X_OUTPUT_SPK)
        regval |= NAU881X_PM3_SPKMIX | NAU881X_PM3_SPKP | NAU881X_PM3_SPKN;
    if (output & NAU881X_OUTPUT_MOUT)
        regval |= NAU881X_PM3_MOUTMIX | NAU881X_PM3_MOUT;

    if (output & NAU881X_OUTPUT_MOUT_DIFFERENTIAL)
    {
        // TODO: SPKMOUT
        regval |= NAU881X_PM3_MOUTMIX | NAU881X_PM3_MOUT | NAU881X_PM3_SPKN;
    }

    return NAU881x_Register_Write(nau881x, NAU881X_REG_POWER_MANAGEMENT_3, regval);
}

nau881x_status_t NAU881x_Set_Speaker_Source(NAU881x_t* nau881x, nau881x_output_source_t source)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_SPK_MIXER_CTRL];
    regval &= ~(0x03 << 0);
    regval &= ~(1 << 5);
    if (source & NAU8814_OUTPUT_FROM_AUX) 
		source = (nau881x_output_source_t)(((uint32_t)source & (uint32_t)NAU8814_OUTPUT_FROM_AUX) | (1 << 5));
    regval |= source;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_SPK_MIXER_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Speaker_FromBypass_Attenuation(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_ATTN_CTRL];
    regval &= ~(1 << 1);
    regval |= (enable ? 1 : 0) << 1;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ATTN_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Speaker_Boost(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_OUTPUT_CTRL];
    regval &= ~(1 << 2);
    regval |= (enable ? 1 : 0) << 2;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_OUTPUT_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Speaker_ZeroCross(NAU881x_t* nau881x, uint8_t state)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_SPK_VOL_CTRL];
    regval &= ~(1 << 7);
    regval |= (state ? 1 : 0) << 7;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_SPK_VOL_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Speaker_Mute(NAU881x_t* nau881x, uint8_t state)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_SPK_VOL_CTRL];
    regval &= ~(1 << 6);
    regval |= (state ? 1 : 0) << 6;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_SPK_VOL_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Speaker_Volume(NAU881x_t* nau881x, uint8_t val)
{
    if (val > 0x3F)
        return NAU881X_STATUS_INVALID;

    uint16_t regval = nau881x->_register[NAU881X_REG_SPK_VOL_CTRL];
    regval &= ~(0x3F << 0);
    regval |= (val & 0x3F) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_SPK_VOL_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Speaker_Volume_db(NAU881x_t* nau881x, int8_t vol_db)
{
    if ((vol_db < -57) || (vol_db >= 6))
        return NAU881X_STATUS_INVALID;

    return NAU881x_Set_Speaker_Volume(nau881x, NAU881X_SPKVOL_DB_TO_REG_VALUE(vol_db));
}

uint8_t NAU881x_Get_Speaker_Volume(NAU881x_t* nau881x)
{
    return nau881x->_register[NAU881X_REG_SPK_VOL_CTRL] & 0x3F;
}

uint8_t NAU881x_Get_Speaker_Volume_db(NAU881x_t* nau881x)
{
    return NAU881X_SPKVOL_REG_VALUE_TO_DB(NAU881x_Get_Speaker_Volume(nau881x));
}

nau881x_status_t NAU881X_Set_Mono_Source(NAU881x_t* nau881x, nau881x_output_source_t source)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_MONO_MIXER_CTRL];
    regval &= ~(0x07 << 0);
    regval |= source;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_MONO_MIXER_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Mono_FromBypass_Attenuation(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_ATTN_CTRL];
    regval &= ~(1 << 2);
    regval |= (enable ? 1 : 0) << 2;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ATTN_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Mono_Boost(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_OUTPUT_CTRL];
    regval &= ~(1 << 3);
    regval |= (enable ? 1 : 0) << 3;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_OUTPUT_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Mono_Mute(NAU881x_t* nau881x, uint8_t state)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_MONO_MIXER_CTRL];
    regval &= ~(1 << 6);
    regval |= (state ? 1 : 0) << 6;
    NAU881x_Register_Write(nau881x, NAU881X_REG_MONO_MIXER_CTRL, regval);

    regval = nau881x->_register[NAU881X_REG_HIGH_VOLTAGE_CTRL];
    regval &= ~(1 << 4);
    regval |= (state ? 1 : 0) << 4;
    NAU881x_Register_Write(nau881x, NAU881X_REG_HIGH_VOLTAGE_CTRL, regval);

    return NAU881X_STATUS_OK;
}

/* ----- General purpose control ----- */
nau881x_status_t NAU881x_Set_SlowClock_Enable(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_CLOCK_CTRL_2];
    regval &= ~(1 << 0);
    regval |= (enable ? 1 : 0) << 0;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_CLOCK_CTRL_2, regval);
}

nau881x_status_t NAU881x_Set_SampleRate(NAU881x_t* nau881x, nau881x_samplerate_t rate)
{
	if (rate > 6)
        return NAU881X_STATUS_INVALID;
	
	uint16_t regval = nau881x->_register[NAU881X_REG_CLOCK_CTRL_2];
    regval &= ~(0x07 << 1);
    regval |= rate << 1;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_CLOCK_CTRL_2, regval);
}

nau881x_status_t NAU8814_Set_GPIO_Control(NAU881x_t* nau8814, nau8814_gpio_function_t function, uint8_t invert_polarity)
{
    if (function > 7)
        return NAU881X_STATUS_INVALID;

    uint16_t regval = nau8814->_register[NAU8814_REG_GPIO_CTRL];
    
    regval &= (0x07 << 0);
    regval |= function << 0;
    regval |= (invert_polarity ? 1 : 0) << 3;
    return NAU881x_Register_Write(nau8814, NAU8814_REG_GPIO_CTRL, regval);
}

nau881x_status_t NAU8814_Set_ThermalShutdown_Enable(NAU881x_t* nau8814, uint8_t enable)
{
    uint16_t regval = nau8814->_register[NAU881X_REG_OUTPUT_CTRL];
    regval &= ~(1 << 1);
    regval |= (enable ? 1 : 0) << 1;
    return NAU881x_Register_Write(nau8814, NAU881X_REG_OUTPUT_CTRL, regval);
}

/* ----- Clock generation ----- */
nau881x_status_t NAU881x_Set_PLL_Enable(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_1];
    regval &= ~(1 << 5);
    regval |= (enable ? 1 : 0) << 5;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_POWER_MANAGEMENT_1, regval);
}

nau881x_status_t NAU881x_Set_PLL_FrequencyRatio(NAU881x_t* nau881x, uint8_t mclk_div2, uint8_t N, uint32_t K)
{
    if (N < 5 || N > 13)
        return NAU881X_STATUS_INVALID;
    if (K > 0xFFFFFF)
        return NAU881X_STATUS_INVALID;
    
    uint16_t n_regval = nau881x->_register[NAU881X_REG_PLL_N] & (0x1F << 0);
    n_regval |= (mclk_div2 ? 1 : 0) << 4;
    n_regval |= (N & 0x0F);
    NAU881x_Register_Write(nau881x, NAU881X_REG_PLL_K3, K & 0x1FF);
    NAU881x_Register_Write(nau881x, NAU881X_REG_PLL_K2, (K >> 9) & 0x1FF);
    NAU881x_Register_Write(nau881x, NAU881X_REG_PLL_K1, (K >> 18) & 0x3F);
    NAU881x_Register_Write(nau881x, NAU881X_REG_PLL_N, n_regval);
    nau881x->_register[NAU881X_REG_PLL_K3] = K & 0x1FF;
    nau881x->_register[NAU881X_REG_PLL_K2] = (K >> 9) & 0x1FF;
    nau881x->_register[NAU881X_REG_PLL_K1] = (K >> 18) & 0x3F;
    return NAU881X_STATUS_OK;
}

/* ----- Control interface ----- */
nau881x_status_t NAU8814_Set_ControlInterface_SPI24bit(NAU881x_t* nau8814, uint8_t enable)
{
    uint16_t regval = nau8814->_register[NAU881X_REG_ADDITIONAL_IF_CTRL];
    regval &= ~(1 << 8);
    regval |= (enable ? 1 : 0) << 8;
    return NAU881x_Register_Write(nau8814, NAU881X_REG_ADDITIONAL_IF_CTRL, regval);
}

/* ----- Digital audio interface ----- */
nau881x_status_t NAU881x_Set_AudioInterfaceFormat(NAU881x_t* nau881x, nau881x_audio_iface_fmt_t format, nau881x_audio_iface_wl_t word_length)
{
    if (word_length > 4)
        return NAU881X_STATUS_INVALID;
    
    uint16_t regval = nau881x->_register[NAU881X_REG_AUDIO_INTERFACE];
    regval &= ~(0x0F << 3);
    regval |= ((format & 0x03) << 3);
    if (word_length != NAU881X_AUDIO_IFACE_WL_8BITS)
        regval |= ((word_length & 0x03) << 5);
    NAU881x_Register_Write(nau881x, NAU881X_REG_AUDIO_INTERFACE, regval);

    regval = nau881x->_register[NAU881X_REG_ADCOUT_DRIVE];
    regval &= ~((1 << 1) | (1 << 8) | (1 << 6));   // PCM B, PCMTSEN, and PCM8BIT bits
    regval |= ((format & (1 << 2)) ? 1 : 0) << 1;
    regval |= ((format & (1 << 3)) ? 1 : 0) << 8;
    if (word_length == NAU881X_AUDIO_IFACE_WL_8BITS)
        regval |= (1 << 6);
    NAU881x_Register_Write(nau881x, NAU881X_REG_ADCOUT_DRIVE, regval);

    return NAU881X_STATUS_OK;
}

nau881x_status_t NAU881x_Set_PCM_Timeslot(NAU881x_t* nau881x, uint16_t timeslot)
{
    if (timeslot > 0x3FF)
        return NAU881X_STATUS_INVALID;
    
    uint16_t regval = timeslot & 0x1FF;
    NAU881x_Register_Write(nau881x, NAU881X_REG_PCM_TIMESLOT, regval);

    regval = nau881x->_register[NAU881X_REG_ADCOUT_DRIVE];
    regval &= ~(1 << 0);
    regval |= ((timeslot & 0x1FF) ? 1 : 0) << 0;
    NAU881x_Register_Write(nau881x, NAU881X_REG_ADCOUT_DRIVE, regval);

    return NAU881X_STATUS_OK;
}

nau881x_status_t NAU881x_Set_FrameClock_Polarity(NAU881x_t* nau881x, uint8_t invert)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_AUDIO_INTERFACE];
    regval &= ~(1 << 7);
    regval |= (invert ? 1 : 0) << 7;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_AUDIO_INTERFACE, regval);
}

nau881x_status_t NAU881x_Set_BCLK_Polarity(NAU881x_t* nau881x, uint8_t invert)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_AUDIO_INTERFACE];
    regval &= ~(1 << 8);
    regval |= (invert ? 1 : 0) << 8;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_AUDIO_INTERFACE, regval);
}

nau881x_status_t NAU881x_Set_ADC_Data_Phase(NAU881x_t* nau881x, uint8_t in_right_phase_of_frame)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_AUDIO_INTERFACE];
    regval &= ~(1 << 1);
    regval |= (in_right_phase_of_frame ? 1 : 0) << 1;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_AUDIO_INTERFACE, regval);
}

nau881x_status_t NAU881x_Set_DAC_Data_Phase(NAU881x_t* nau881x, uint8_t in_right_phase_of_frame)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_AUDIO_INTERFACE];
    regval &= ~(1 << 2);
    regval |= (in_right_phase_of_frame ? 1 : 0) << 2;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_AUDIO_INTERFACE, regval);
}

nau881x_status_t NAU881x_Set_Clock(NAU881x_t* nau881x, uint8_t is_master, nau881x_bclkdiv_t bclk_divider, nau881x_mclkdiv_t mclk_divider, nau881x_clksel_t clock_source)
{
    if ((uint32_t)bclk_divider >= 6)
        return NAU881X_STATUS_INVALID;
    if ((uint32_t)mclk_divider >= 8)
        return NAU881X_STATUS_INVALID;
    if ((uint32_t)clock_source >= 2)
        return NAU881X_STATUS_INVALID;

    uint16_t regval = 0;
    regval |= (is_master ? 1 : 0) << 0;
    regval |= bclk_divider << 2;
    regval |= mclk_divider << 5;
    regval |= clock_source << 8;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_CLOCK_CTRL_1, regval);
}

nau881x_status_t NAU881x_Set_LOUTR(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_ADCOUT_DRIVE];
    regval &= ~(1 << 2);
    regval |= ((enable ? 1 : 0) << 2);
    return NAU881x_Register_Write(nau881x, NAU881X_REG_ADCOUT_DRIVE, regval);
}

nau881x_status_t NAU881x_Set_ADC_Companding(NAU881x_t* nau881x, nau881x_companding_t companding)
{
    if (companding == 2 || companding > 3)
        return NAU881X_STATUS_INVALID;
    uint16_t regval = nau881x->_register[NAU881X_REG_COMPANDING_CTRL];
    regval &= ~(0x03 << 1);
    regval |= companding << 1;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_COMPANDING_CTRL, regval);
}

nau881x_status_t NAU881x_Set_DAC_Companding(NAU881x_t* nau881x, nau881x_companding_t companding)
{
    if (companding == 2 || companding > 3)
        return NAU881X_STATUS_INVALID;
    uint16_t regval = nau881x->_register[NAU881X_REG_COMPANDING_CTRL];
    regval &= ~(0x03 << 3);
    regval |= companding << 3;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_COMPANDING_CTRL, regval);
}

nau881x_status_t NAU881x_Set_Companding_WordLength_8bit(NAU881x_t* nau881x, uint8_t enable)
{
    uint16_t regval = nau881x->_register[NAU881X_REG_COMPANDING_CTRL];
    regval &= ~(1 << 5);
    regval |= (enable ? 1 : 0) << 5;
    return NAU881x_Register_Write(nau881x, NAU881X_REG_COMPANDING_CTRL, regval);
}

/* ----- Other ----- */
//nau881x_status_t NAU881x_Get_SiliconRevision(NAU881x_t* nau881x, uint8_t* silicon_revision)
//{
//    nau881x->_register[NAU881X_REG_SILICON_REV] = NAU881X_REG_READ(nau881x->comm_handle, NAU881X_REG_SILICON_REV);
//    *silicon_revision = nau881x->_register[NAU881X_REG_SILICON_REV] & 0xFF;
//    return NAU881X_STATUS_OK;
//}

/* ----- Reset ----- */
nau881x_status_t NAU881x_SoftwareReset(NAU881x_t* nau881x)
{
    NAU881x_Register_Write(nau881x, NAU881X_REG_SOFTWARE_RESET, 1);

    // Set register default values based on datasheet
    // Software reset register does not need to be set
    // Power management
    nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_1] = 0x0000;
    nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_2] = 0x0000;
    nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_3] = 0x0000;
    // Audio control
    nau881x->_register[NAU881X_REG_AUDIO_INTERFACE] = 0x0050;
    nau881x->_register[NAU881X_REG_COMPANDING_CTRL] = 0x0000;
    nau881x->_register[NAU881X_REG_CLOCK_CTRL_1] = 0x0140;
    nau881x->_register[NAU881X_REG_CLOCK_CTRL_2] = 0x0000;
    nau881x->_register[NAU8814_REG_GPIO_CTRL] = 0x0000;
    nau881x->_register[NAU881X_REG_DAC_CTRL] = 0x0000;
    nau881x->_register[NAU881X_REG_DAC_VOL] = 0x00FF;
    nau881x->_register[NAU881X_REG_ADC_CTRL] = 0x0100;
    nau881x->_register[NAU881X_REG_ADC_VOL] = 0x00FF;
    // Equalizer
    nau881x->_register[NAU881X_REG_EQ_1] = 0x012C;
    nau881x->_register[NAU881X_REG_EQ_2] = 0x002C;
    nau881x->_register[NAU881X_REG_EQ_3] = 0x002C;
    nau881x->_register[NAU881X_REG_EQ_4] = 0x002C;
    nau881x->_register[NAU881X_REG_EQ_5] = 0x002C;
    // DAC limiter
    nau881x->_register[NAU881X_REG_DAC_LIMITER_1] = 0x0032;
    nau881x->_register[NAU881X_REG_DAC_LIMITER_2] = 0x0000;
    // Notch filter
    nau881x->_register[NAU881X_REG_NOTCH_FILTER_0_H] = 0x0000;
    nau881x->_register[NAU881X_REG_NOTCH_FILTER_0_L] = 0x0000;
    nau881x->_register[NAU881X_REG_NOTCH_FILTER_1_H] = 0x0000;
    nau881x->_register[NAU881X_REG_NOTCH_FILTER_1_L] = 0x0000;
    // ALC control
    nau881x->_register[NAU881X_REG_ALC_CTRL_1] = 0x0038;
    nau881x->_register[NAU881X_REG_ALC_CTRL_2] = 0x000B;
    nau881x->_register[NAU881X_REG_ALC_CTRL_3] = 0x0032;
    nau881x->_register[NAU881X_REG_NOISE_GATE] = 0x0000;
    // PLL control
    nau881x->_register[NAU881X_REG_PLL_N] = 0x0008;
    nau881x->_register[NAU881X_REG_PLL_K1] = 0x000C;
    nau881x->_register[NAU881X_REG_PLL_K2] = 0x0093;
    nau881x->_register[NAU881X_REG_PLL_K3] = 0x00E9;
    // Input, putput & mixer control
    nau881x->_register[NAU881X_REG_ATTN_CTRL] = 0x0000;
    nau881x->_register[NAU881X_REG_INPUT_CTRL] = 0x0003;
    nau881x->_register[NAU881X_REG_PGA_GAIN_CTRL] = 0x0010;
    nau881x->_register[NAU881X_REG_ADC_BOOST_CTRL] = 0x0100;
    nau881x->_register[NAU881X_REG_OUTPUT_CTRL] = 0x0002;
    nau881x->_register[NAU881X_REG_SPK_MIXER_CTRL] = 0x0001;
    nau881x->_register[NAU881X_REG_SPK_VOL_CTRL] = 0x0039;
    nau881x->_register[NAU881X_REG_MONO_MIXER_CTRL] = 0x0001;
    // Low power control
    nau881x->_register[NAU881X_REG_POWER_MANAGEMENT_4] = 0x0000;
    // PCM time slot & ADCOUT impedance option control
    nau881x->_register[NAU881X_REG_PCM_TIMESLOT] = 0x0000;
    nau881x->_register[NAU881X_REG_ADCOUT_DRIVE] = 0x0020;
    // Register ID
    nau881x->_register[NAU881X_REG_SILICON_REV] = 0x00EF;
    nau881x->_register[NAU881X_REG_2WIRE_ID] = 0x001A;
    nau881x->_register[NAU881X_REG_ADDITIONAL_ID] = 0x00CA;
    // reserved reg does not need to be set
    nau881x->_register[NAU881X_REG_HIGH_VOLTAGE_CTRL] = 0x0001;
    nau881x->_register[NAU881X_REG_ALC_ENHANCEMENTS_1] = 0x0000;
    nau881x->_register[NAU881X_REG_ALC_ENHANCEMENTS_2] = 0x0039;
    nau881x->_register[NAU881X_REG_ADDITIONAL_IF_CTRL] = 0x0000;
    nau881x->_register[NAU881X_REG_POWER_TIE_OFF_CTRL] = 0x0000;
    nau881x->_register[NAU881X_REG_AGC_P2P_DET] = 0x0000;
    nau881x->_register[NAU881X_REG_AGC_PEAK_DET] = 0x0000;
    nau881x->_register[NAU881X_REG_CTRL_AND_STATUS] = 0x0000;
    nau881x->_register[NAU881X_REG_OUTPUT_TIE_OFF_CTRL] = 0x0000;

    
    return NAU881X_STATUS_OK;
}

/* ----- Register write function ----- */
nau881x_status_t NAU881x_Register_Write(NAU881x_t* nau881x, uint8_t register_addr, uint16_t value)
{
    NAU881X_REG_WRITE(register_addr, value);
    nau881x->_register[register_addr] = value;
    return NAU881X_STATUS_OK;
}

uint16_t NAU881x_Register_GetValue(NAU881x_t* nau881x, uint8_t register_addr)
{
    // Not actually read the register value from the chip
    return nau881x->_register[register_addr];
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
void Codec_Config(void)
{	
	NAU881x_Init(&nau881x);
	
	NAU881x_Set_PGA_Input(&nau881x,(nau881x_input_t)(NAU881X_INPUT_MICP | NAU881X_INPUT_MICN));//MIC+ 和 MIC-连接到PGA正端和负端，采用差分输入模式，有效抑制共模噪声(0x2C)
	
	//寄存器0x2C的第8位和第9位 和 寄存器0x3A的第4位一起控制Microphone Bias Voltage
	NAU881x_Set_MicBias_Enable(&nau881x,ENABLE);//先使能MICBIAS，MICBIASEN[4]置1(0x01)
	NAU881x_Set_MicBias_Voltage(&nau881x,RESETBIT);//麦克风偏置电压为0.9*3.3(VDDA) = 2.97V(0x2C)	
	NAU881x_Set_MicBiasMode_Enable(&nau881x,DISABLE);//(0x3A)
	
	
	NAU881x_Set_PGA_Enable(&nau881x,ENABLE);//先使能PGA，PGAEN[2]置1(0x02)
	NAU881x_Set_PGA_Gain_db(&nau881x,18);//可编程器放大增益设置为18dB(0x2D)  改
	NAU881x_Set_PGA_Mute(&nau881x,RESETBIT);//PGA正常输出，不静音，若静音了则信号不会被放大或者传输到后续电路中(0x2D)
	NAU881x_Set_PGA_ZeroCross(&nau881x,RESETBIT);//不启用零交叉检测，当增益寄存器更改时，更新增益(0x2D)
	
	
	//AUX和MIC都不连接到boost，因为输入端是差分输入，保持默认值，不进行配置
	//若要配置使用该函数NAU881x_Set_Boost_Volume();，如果MIC+引脚要到输入提升阶段，需要设置PMICPAG[0] = 0 (0x2C)
	NAU881x_Set_Boost_Enable(&nau881x,ENABLE);//先使能BOOST，BSTEN[4]置1(0x02)
	NAU881x_Set_PGA_Boost(&nau881x,SETBIT);//PGA输出增益为20dB(0x2F)
	
	
	NAU881x_Set_ADC_Enable(&nau881x,ENABLE);//先使能ADC，ADCEN[0]置1(0x02)
	NAU881x_Set_ADC_Polarity(&nau881x,RESETBIT);//极性默认(0x0E)
	NAU881x_Set_ADC_OverSampleRate(&nau881x,NAU881X_ADC_OVERSAMPLE_128X);//过采样率为128x，为最佳信噪比(0x0E)
	NAU881x_Set_ADC_HighPassFilter(&nau881x,ENABLE,NAU881X_HPF_MODE_APP,6);//HPFEN[8]置1，使能高通滤波器(0x0E) 改
	                                                                       //采用应用模式，此时高通滤波器是二阶的，截止频率为327Hz，截至频率与采样频率相关联，采样频率为8kHz
	//缺口滤波器寄存器,没有函数
	NAU881x_Set_ADC_Gain(&nau881x,255);//增益为0dB(0x0F)   改
	
	
	NAU881x_Set_ALC_Enable(&nau881x,DISABLE);//ALC不使能，若启用ALC，ALC监控ADC的输出，调整增益，并反馈给PGA(0x20)
	NAU881x_Set_ALC_NoiseGate_Enable(&nau881x,DISABLE);//噪声门不使能(0x23)
	
	
	NAU881x_Set_DAC_Enable(&nau881x,ENABLE);//先使能DAC，DACEN[0]置1(0x03)
	NAU881x_Set_DAC_Polarity(&nau881x,RESETBIT);//极性默认(0x0A)
	NAU881x_Set_DAC_AutoMute(&nau881x,DISABLE);//自动静音不启用，可以在需要的时候在进行配置(0x0A)
	NAU881x_Set_DAC_SoftMute(&nau881x,DISABLE);//软件静音不启用，可以在需要的时候在进行配置(0x0A)
	NAU881x_Set_DAC_OverSampleRate(&nau881x,NAU881X_DAC_OVERSAMPLE_128X);//oversample设置为128x，为最佳信噪比(0x0A)
	NAU881x_Set_DAC_SampleRate(&nau881x,NAU881X_DAC_SAMPLERATE_NO_DEEMPHASIS);//(0x0A)
	NAU881x_Set_DAC_Gain(&nau881x,255);//增益为0dB(0x0B)   改
	NAU881x_Set_DAC_Limiter_Enable(&nau881x,DISABLE);//不使能(0x18,0x19)
	
	
	NAU881x_Set_Equalizer_Path(&nau881x,NAU881X_EQ_PATH_DAC);               //EQ1，DAC通道
	NAU881x_Set_Equalizer_Gain(&nau881x,EQ1,0);                             //EQ1，12dB   改
	NAU881x_Set_Equalizer1_Frequency(&nau881x,NAU881X_EQ1_CUTOFF_175HZ);    //EQ1，频率为175Hz
	
	NAU881x_Set_Equalizer_Bandwidth(&nau881x,EQ2,NAU881X_EQ_BW_NARROW);     //EQ2，窄带宽
	NAU881x_Set_Equalizer_Gain(&nau881x,EQ2,15);                            //EQ2，3dB    改
	NAU881x_Set_Equalizer2_Frequency(&nau881x,NAU881X_EQ2_CENTER_300HZ);    //EQ2，频率为300Hz
	
	NAU881x_Set_Equalizer_Bandwidth(&nau881x,EQ3,NAU881X_EQ_BW_NARROW);     //EQ3，窄带宽
	NAU881x_Set_Equalizer_Gain(&nau881x,EQ3,3);                             //EQ3，9dB    改
	NAU881x_Set_Equalizer3_Frequency(&nau881x,NAU881X_EQ3_CENTER_850HZ);    //EQ3，频率为850Hz
	
	NAU881x_Set_Equalizer_Bandwidth(&nau881x,EQ4,NAU881X_EQ_BW_NARROW);     //EQ4，窄带宽
	NAU881x_Set_Equalizer_Gain(&nau881x,EQ4,6);                             //EQ4，6dB    改
	NAU881x_Set_Equalizer4_Frequency(&nau881x,NAU881X_EQ4_CENTER_1800HZ);   //EQ4，频率为1.8kHz
	
	NAU881x_Set_Equalizer_Bandwidth(&nau881x,EQ5,NAU881X_EQ_BW_NARROW);     //EQ5，窄带宽
	NAU881x_Set_Equalizer_Gain(&nau881x,EQ5,24);                            //EQ5，-12dB   改
	NAU881x_Set_Equalizer5_Frequency(&nau881x,NAU881X_EQ5_CUTOFF_5300HZ);   //EQ5，频率为5.3kHz
	
	
	NAU881x_Set_Speaker_FromBypass_Attenuation(&nau881x,ENABLE);//扬声混合器的旁路路径衰减-10dB(0x28)
	NAU881x_Set_Mono_FromBypass_Attenuation(&nau881x,DISABLE);//MONO  mixer input为0dB(0x28)
	
	
	NAU8814_Set_GPIO_Control(&nau881x,NAU8814_GPIO_FUNCTION_CSB_INPUT,RESETBIT);//作为SPI接口，即片选引脚，设置为Csb Input，极性默认(0x08)
	NAU8814_Set_ControlInterface_SPI24bit(&nau881x,DISABLE);//不使用24bit模式(0x49)，设置SPIEN[8],和mode pin去选择I2C还是SPI模式
	                                                        //mode pin已经接高电平，SPIEN[8]设为0，选择SPI 16bit模式
															
	NAU881x_Set_SampleRate(&nau881x,NAU881X_SAMPLERATE_8KHZ);//采样频率8kHz(0x07)
	NAU881x_Set_SlowClock_Enable(&nau881x,DISABLE);//不使能慢时钟(0x07)
	
	
	NAU881x_Set_PLL_Enable(&nau881x,ENABLE);//先使能PLL，PLLEN[5]置1(0x01)
	NAU881x_Set_PLL_FrequencyRatio(&nau881x,0,8,0);//MCLK进入不进行2分频，并且N = 8，K = 0(0x24,0x25,0x26,0x27)
	
	
	NAU881x_Set_Clock(&nau881x,MASTER,NAU881X_BCLKDIV_1,NAU881X_MCLKDIV_12,NAU881X_CLKSEL_PLL);//设置主时钟模式,采用PLL，MCLK分频系数为12(0x06)
	
	
	NAU881x_Set_AudioInterfaceFormat(&nau881x,NAU881X_AUDIO_IFACE_FMT_I2S,NAU881X_AUDIO_IFACE_WL_16BITS);//I2S传输，16位宽
	NAU881x_Set_FrameClock_Polarity(&nau881x,RESETBIT);//FS为下降沿触发
	NAU881x_Set_BCLK_Polarity(&nau881x,RESETBIT);//BCLK为下降沿触发
	NAU881x_Set_ADC_Data_Phase(&nau881x,RESETBIT);//ADC和DAC数据在帧时钟左侧
	NAU881x_Set_DAC_Data_Phase(&nau881x,RESETBIT);
	
	
	NAU881x_Set_ADC_Companding(&nau881x,NAU881X_COMPANDING_OFF);//ADC音频不压缩
	NAU881x_Set_DAC_Companding(&nau881x,NAU881X_COMPANDING_OFF);//DAC音频不压缩
	NAU881x_Set_Companding_WordLength_8bit(&nau881x,DISABLE);//不使能压缩模式8位字
	NAU881x_Set_ADC_DAC_Passthrough(&nau881x,DISABLE);//不直通
	
	
	NAU881x_Set_Output_Enable(&nau881x,NAU881X_OUTPUT_SPK);//输出接口采用扬声器，需要将SPKMXEN[2]、PSPKEN[5]、NSPKEN[6]置1(0x03)
	NAU881x_Set_Speaker_Source(&nau881x,(nau881x_output_source_t)(NAU881X_OUTPUT_FROM_DAC | NAU881X_OUTPUT_FROM_BYPASS));//DAC 和 BYPASS 到扬声器混合器输出，AUX不连接到扬声器输出(0x32)
	
	
	NAU881x_Set_Speaker_Volume_db(&nau881x,0);//扬声器增益为0dB
	NAU881x_Set_Speaker_Mute(&nau881x,SETBIT);//扬声器不静音
	NAU881x_Set_Speaker_ZeroCross(&nau881x,RESETBIT);//不启用零交差检测
	
	NAU881x_Set_Speaker_Boost(&nau881x,DISABLE);//不启用扬声器增益提升
	NAU8814_Set_ThermalShutdown_Enable(&nau881x,ENABLE);//启用过热保护
}











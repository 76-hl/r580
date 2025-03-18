#ifndef __CODEC_H
#define __CODEC_H

#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "delay.h"


void SPI3_Init(void);
void SPI_WriteReg(uint8_t addr,uint16_t data);
void NAU8814_Init(void);

#endif

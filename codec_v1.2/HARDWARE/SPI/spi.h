#ifndef __CODEC_H
#define __CODEC_H

#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "delay.h"

#include "nau881x.h"
#include "nau881x_regs.h"

void SPI3_Init(void);
void MySPI_Write(uint8_t addr,uint16_t data);

#endif

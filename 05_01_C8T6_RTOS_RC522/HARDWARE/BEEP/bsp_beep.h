#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H

#include "stm32f10x.h"
//#include "sys.h"

void beep_init(void);

//#define BEEP PBout(12)//PA12(·äÃùÆ÷)
//BEEPµÆ×´Ì¬
#define BEEP_STATU(i) i?GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_SET):GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_RESET);

#endif

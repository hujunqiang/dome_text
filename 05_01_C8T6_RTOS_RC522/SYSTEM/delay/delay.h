#ifndef  _DELAY_H
#define  _DELAY_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"


void Delay_Init(void);

void delay_ms(u32 nms);
void delay_s(uint32_t delay);



void delay_xus(u32 nus);
void delay_xms(u32 nms);
void SysTick_Handler(void);


#endif






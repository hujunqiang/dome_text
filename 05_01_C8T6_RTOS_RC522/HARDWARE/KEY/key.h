#ifndef __KEY_H
#define __KEY_H
#include"stm32f10x.h"
#include "stdio.h"
#include "usart1.h"

#define KEY1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)    //定义KEY1为读取按键1
#define KEY2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)    //定义KEY2为读取按键2
#define KEY3 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)    //定义KEY3为读取按键3
#define KEY4 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)    //定义KEY3为读取按键3

int read_key1(void);
int read_key2(void);
int read_key3(void);
int read_key4(void);


void KEY_Init(void);
u8 KEY_Scan(u8 mode);
u8 Key_GetNum(void);

#endif

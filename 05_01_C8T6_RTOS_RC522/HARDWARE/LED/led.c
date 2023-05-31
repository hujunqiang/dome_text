#include "led.h"

// 初始化PB5和PE5为输出口.并使能这两个口的时钟
// LED IO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能PB端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; // PB 5 6 12 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;					// 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					// IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);								// 根据设定参数初始化PB 5 6 12
	GPIO_ResetBits(GPIOB, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);			// PB 5 6 12	输出高
}

void LED1_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}

void LED1_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
}

void LED1_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_5);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	}
}

void LED2_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_6);
}

void LED2_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_6);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	}
}

void LED3_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);
}

void LED3_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
}

void LED3_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_7) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_7);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_7);
	}
}

#if 0

// 初始化PB5和PE5为输出口.并使能这两个口的时钟
// LED IO初始化
void LED_Init(void)
{ 

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE); // 使能PC端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		  // LED0-->PC.13 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);			  // 根据设定参数初始化GPIOC.13
	GPIO_SetBits(GPIOC, GPIO_Pin_13);				  // PC13	输出高

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // LED1、2-->PA0、1端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   // IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   // 根据设定参数初始化GPIOA.0
	GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1);		   // PC13	输出高
}

#endif







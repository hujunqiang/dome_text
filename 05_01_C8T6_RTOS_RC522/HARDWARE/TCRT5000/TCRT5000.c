#include "tcrt5000.h"

void tcrt5000_1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure_A;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIO外设
	GPIO_InitStructure_A.GPIO_Pin = GPIO_Pin_8;			  // 定义PA8gpio口
	GPIO_InitStructure_A.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure_A.GPIO_Mode = GPIO_Mode_IPD; // ipd下拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure_A);		// 初始化GPIO
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

void tcrt5000_2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure_A;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIO外设
	GPIO_InitStructure_A.GPIO_Pin = GPIO_Pin_11;		  // 定义PA11gpio口
	GPIO_InitStructure_A.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure_A.GPIO_Mode = GPIO_Mode_IPD; // ipd下拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure_A);		// 初始化GPIO
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}

void tcrt5000_3_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure_A;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIO外设
	GPIO_InitStructure_A.GPIO_Pin = GPIO_Pin_12;		  // 定义PA12gpio口
	GPIO_InitStructure_A.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure_A.GPIO_Mode = GPIO_Mode_IPD; // ipd下拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure_A);		// 初始化GPIO
	GPIO_ResetBits(GPIOA, GPIO_Pin_12);
}

void tcrt5000_4_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure_A;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIO外设
	// stm32f103中PB3、PB4、PA15为JTAG引脚，若要作为普通IO口来使用的话，要禁用JTAG功能。
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitStructure_A.GPIO_Pin = GPIO_Pin_15; // 定义PA15gpio口
	GPIO_InitStructure_A.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure_A.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ipd下拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure_A);		// 初始化GPIO
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
}

// int tcrt5000_1(void)
//{
//	int left_1=1;
//	left_1= GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8);//读取A8引脚的值
//	printf("hongwai ----- %d",left_1);//串口打印
//	return left_1;
// }

// int tcrt5000_2(void)
//{
//	int left_2=1;
//	left_2= GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11);//读取A11引脚的值
//	// printf("hongwai ----- %d",left);//串口打印
//	return left_2;
// }

bool tcrt5000_1(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
}
bool tcrt5000_2(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11);
}
bool tcrt5000_3(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12);
}
bool tcrt5000_4(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15);
}

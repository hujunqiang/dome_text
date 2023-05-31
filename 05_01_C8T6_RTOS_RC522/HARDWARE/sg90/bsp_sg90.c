#include "bsp_sg90.h"

// PWM配置 舵机驱动（查看STM32手册）
void SG90_1_TIM2_pwm_init(int arr, int psc)
{

	// 声明GPIO+TIM+PWM结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	// 使能PA引脚时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	// 使能PB引脚时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// 使能TIM2时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	// 开复用时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	// 配置PA1引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // 引脚编号
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   // 输出速率
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   // 复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // 引脚编号
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// TIM2配置  TIM2 -> APB1为72Mhz
	TIM_TimeBaseStructure.TIM_Period = arr;						// 计数周期(当前10次/ms),舵机的控制一般需要一个20ms的时基脉冲
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					// 预分频值(72Mhz/7200 = 10000HZ/s 则为10次/ms)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				// 再次分频值 1
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	// PWM配置 TIM2_CH3
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  // PWM模式2  cnt > ccr 有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出比较
	TIM_OCInitStructure.TIM_Pulse = 0;							  // 设置CCR寄存器的初值 可不做设置
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  // 输出极性高 输出高电平
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

	// 使能预装载寄存器
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// 使能TIM2
	TIM_Cmd(TIM2, ENABLE);
	// 使能TIM3
	TIM_Cmd(TIM3, ENABLE);
}

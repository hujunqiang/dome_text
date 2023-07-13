#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x.h"
 
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 100
// 256 字节 或者 1k 
//volatile关键字来修饰rxBuffer数组，表示该数组是易变的，可能会被中断服务函数修改。
volatile uint8_t rxBuffer[BUFFER_SIZE]; // 接收缓冲区
volatile uint8_t rxIndex = 0;           // 接收缓冲区索引
volatile uint8_t *rxPtr = NULL;

//写一个指针，指向这个数组  把串口接收到的数据存到指针指向的数组里面，指针每次加一，指向下一个数组元素

//在响应时，可能接受多个数据 ，中断响应使能这个串口，

//下降沿触发，使能串口1
void EXTI2_IRQnHandler(void) // 外部中断 2 中断服务函数
{
    //先清除中断标志位
    EXTI_ClearITPendingBit(EXTI_Line2);
    USART_Cmd(USART1, ENABLE); // 使能串口1
}

//在串口中断服务函数里面，把接收到的数据存到指针指向的数组里面，指针每次加一，指向下一个数组元素
//只是把指针指向接受数组的起始位置（把数组的下表设置为零）  在串口的中断函数里面自增
void UART1_IRQHandler(void) // 串口1中断服务函数
{
    //先清除中断标志位
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        *rxPtr = USART_ReceiveData(USART1); // 将接收到的数据存到指针指向的数组里面
        rxPtr++; // 指针每次加一，指向下一个数组元素
        if (rxPtr >= rxBuffer + BUFFER_SIZE) // 判断指针是否越界
            rxPtr = rxBuffer; // 如果越界，指针指向数组的首地址
    }
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // GPIO配置结构体

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; // TX  RX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 速度50MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  // 初始化GPIOA
}

void USART1_Init(void)
{
    USART_InitTypeDef USART_InitStructure; // 串口配置结构体
    NVIC_InitTypeDef NVIC_InitStructure;   // 中断配置结构体

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // 使能USART1时钟

    USART_DeInit(USART1); // 复位串口1

    USART_InitStructure.USART_BaudRate = 19200;                                     // 波特率19200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 8位数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 停止位1
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 收发模式
    USART_Init(USART1, &USART_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ; //抢占优先级 3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //子优先级 3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ 通道使能
	NVIC_Init(&NVIC_InitStructure); //中断优先级初始化
	//开启中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启中断

}
void EXTIX_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // 开启 AFIO 时钟

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2); 
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;// 中断线 2
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;// 中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;// 使能中断线

    EXTI_Init(&EXTI_InitStructure);                              // 初始化中断线参数
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;             // 使能外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; // 抢占优先级 2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;        // 子优先级 2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              // 使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);// 初始化 NVIC 参数
}


 int main(void)
 {	
	delay_init();	    //延时函数初始化	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
    GPIO_Configuration();
    USART1_Init();
    EXTIX_Init();
	rxPtr = (uint8_t *)malloc(sizeof(uint8_t)*BUFFER_SIZE);
	rxPtr = rxBuffer; // 声明并初始化指针变量，指向数组的首地址
	while(1)
	{

	}
 }


 /**


// void UART1_IRQHandler(void) // 串口1中断服务函数
// {
//     if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//     {
//         rxBuffer[rxIndex] = USART_ReceiveData(USART1);
//         rxIndex++;
//         if (rxIndex >= BUFFER_SIZE)
//             rxIndex = 0;
//     }
// }


**/


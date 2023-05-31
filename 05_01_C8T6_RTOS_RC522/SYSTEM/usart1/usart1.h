#ifndef _USART1_H
#define _USART1_H

#include "stm32f10x.h"

// C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#define EN_USART1_RX  1   // 使能（1）/禁止（0）串口1接收
#define USART_REC_LEN 200 // 定义最大接收字节数 200

#define USART_DEBUG USART1 // 调试打印所使用的串口组

#define USART1_TX GPIO_Pin_9  // 串口1发送口
#define USART1_RX GPIO_Pin_10 // 串口1接收口

#define USART3_TX GPIO_Pin_10 // 串口1发送口
#define USART3_RX GPIO_Pin_11 // 串口1接收口

#define USART_4G USART2 // 与4G模块通信的串口组



// 如果想串口中断接收，请不要注释以下宏定义
// 串口1初始化
void USART1_Init(int baud);
// 串口2初始化
void uart2_init(u32 bound);
// 串口3初始化
void uart3_init(u32 bound);
// 串口2发送字符串
void Usart_SendString(USART_TypeDef *USARTx, u8 *str, unsigned short len);
// 串口1打印
void UsartPrintf(USART_TypeDef *USARTx, char *fmt, ...);



#endif

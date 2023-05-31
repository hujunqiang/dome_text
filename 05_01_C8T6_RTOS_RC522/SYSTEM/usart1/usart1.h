#ifndef _USART1_H
#define _USART1_H

#include "stm32f10x.h"

// C��
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#define EN_USART1_RX  1   // ʹ�ܣ�1��/��ֹ��0������1����
#define USART_REC_LEN 200 // �����������ֽ��� 200

#define USART_DEBUG USART1 // ���Դ�ӡ��ʹ�õĴ�����

#define USART1_TX GPIO_Pin_9  // ����1���Ϳ�
#define USART1_RX GPIO_Pin_10 // ����1���տ�

#define USART3_TX GPIO_Pin_10 // ����1���Ϳ�
#define USART3_RX GPIO_Pin_11 // ����1���տ�

#define USART_4G USART2 // ��4Gģ��ͨ�ŵĴ�����



// ����봮���жϽ��գ��벻Ҫע�����º궨��
// ����1��ʼ��
void USART1_Init(int baud);
// ����2��ʼ��
void uart2_init(u32 bound);
// ����3��ʼ��
void uart3_init(u32 bound);
// ����2�����ַ���
void Usart_SendString(USART_TypeDef *USARTx, u8 *str, unsigned short len);
// ����1��ӡ
void UsartPrintf(USART_TypeDef *USARTx, char *fmt, ...);



#endif

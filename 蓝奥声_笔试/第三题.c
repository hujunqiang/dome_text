#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x.h"
 
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 100
// 256 �ֽ� ���� 1k 
//volatile�ؼ���������rxBuffer���飬��ʾ���������ױ�ģ����ܻᱻ�жϷ������޸ġ�
volatile uint8_t rxBuffer[BUFFER_SIZE]; // ���ջ�����
volatile uint8_t rxIndex = 0;           // ���ջ���������
volatile uint8_t *rxPtr = NULL;

//дһ��ָ�룬ָ���������  �Ѵ��ڽ��յ������ݴ浽ָ��ָ����������棬ָ��ÿ�μ�һ��ָ����һ������Ԫ��

//����Ӧʱ�����ܽ��ܶ������ ���ж���Ӧʹ��������ڣ�

//�½��ش�����ʹ�ܴ���1
void EXTI2_IRQnHandler(void) // �ⲿ�ж� 2 �жϷ�����
{
    //������жϱ�־λ
    EXTI_ClearITPendingBit(EXTI_Line2);
    USART_Cmd(USART1, ENABLE); // ʹ�ܴ���1
}

//�ڴ����жϷ��������棬�ѽ��յ������ݴ浽ָ��ָ����������棬ָ��ÿ�μ�һ��ָ����һ������Ԫ��
//ֻ�ǰ�ָ��ָ������������ʼλ�ã���������±�����Ϊ�㣩  �ڴ��ڵ��жϺ�����������
void UART1_IRQHandler(void) // ����1�жϷ�����
{
    //������жϱ�־λ
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        *rxPtr = USART_ReceiveData(USART1); // �����յ������ݴ浽ָ��ָ�����������
        rxPtr++; // ָ��ÿ�μ�һ��ָ����һ������Ԫ��
        if (rxPtr >= rxBuffer + BUFFER_SIZE) // �ж�ָ���Ƿ�Խ��
            rxPtr = rxBuffer; // ���Խ�磬ָ��ָ��������׵�ַ
    }
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // GPIO���ýṹ��

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ʹ��GPIOAʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; // TX  RX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // �ٶ�50MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  // ��ʼ��GPIOA
}

void USART1_Init(void)
{
    USART_InitTypeDef USART_InitStructure; // �������ýṹ��
    NVIC_InitTypeDef NVIC_InitStructure;   // �ж����ýṹ��

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // ʹ��USART1ʱ��

    USART_DeInit(USART1); // ��λ����1

    USART_InitStructure.USART_BaudRate = 19200;                                     // ������19200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 8λ����λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // ֹͣλ1
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // ����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ��������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // �շ�ģʽ
    USART_Init(USART1, &USART_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ; //��ռ���ȼ� 3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ� 3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure); //�ж����ȼ���ʼ��
	//�����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //�����ж�

}
void EXTIX_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // ���� AFIO ʱ��

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2); 
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;// �ж��� 2
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;// �ж�ģʽ
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // �½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;// ʹ���ж���

    EXTI_Init(&EXTI_InitStructure);                              // ��ʼ���ж��߲���
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;             // ʹ���ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; // ��ռ���ȼ� 2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;        // �����ȼ� 2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              // ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);// ��ʼ�� NVIC ����
}


 int main(void)
 {	
	delay_init();	    //��ʱ������ʼ��	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
    GPIO_Configuration();
    USART1_Init();
    EXTIX_Init();
	rxPtr = (uint8_t *)malloc(sizeof(uint8_t)*BUFFER_SIZE);
	rxPtr = rxBuffer; // ��������ʼ��ָ�������ָ��������׵�ַ
	while(1)
	{

	}
 }


 /**


// void UART1_IRQHandler(void) // ����1�жϷ�����
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


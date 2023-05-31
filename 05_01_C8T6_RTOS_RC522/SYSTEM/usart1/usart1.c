#include "usart1.h"

volatile unsigned char  usart1_recvbuf[128]; //串口1的接收数组
volatile unsigned int   usart1_recvcnt = 0;  //串口1的接收数据的个数


u8 USART_RX_BUF[USART_REC_LEN]; // 接收缓冲,最大USART_REC_LEN个字节.
u16 USART_RX_STA = 0; // 接收状态标记
u16 U3_RX_Cnt = 0;
u8 U3_RX_BUF[1024] = {0}; // 串口接收缓存区
u8 U3_RX_Cmd = 0;
u8 U3_RX_Error = 0;

//printf可以向标准输出输出格式化字符串  ---> printf函数内部会调用一个函数fputc,所以用户只需要修改fputc函数
int fputc(int ch, FILE *f) 
{
	USART_SendData(USART1,(uint8_t)ch); 						//通过串口1发送一个字节数据
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET); //等待数据发送完成
	
	return ch;
}

//USART1串口初始化
void USART1_Init(int baud)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);		//打开GPIOA时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);		//打开USART1时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);		//打开复用时钟
			
	//设置PA9和PA10引脚 PA9->TX PA10->RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//配置USART1
	USART_InitStructure.USART_BaudRate 		= baud;
	USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits 		= USART_StopBits_1;
	USART_InitStructure.USART_Parity 		= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode 			= USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	
	//配置嵌套中断
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//配置中断触发方式为接收到数据触发
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//使能USART1
	USART_Cmd(USART1,ENABLE);
}

//USART1的中断服务函数
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		usart1_recvbuf[usart1_recvcnt++] = USART_ReceiveData(USART1);
	}  
}

/*
 ************************************************************
 *	函数名称：	uart2_init
 *
 *	函数功能：	串口2初始化
 *
 *	入口参数：	bound：设定的波特率
 *
 *	返回参数：	无
 *
 *	说明：		TX-PA2		RX-PA3
 ************************************************************
 */
void uart2_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	// 1.使能IO和串口时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // 使能串口2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 使能GPIOA时钟
	// 2.初始化PA2 TXD2--4G模块
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 推挽复用输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// PA3 RXD2--4G
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// 3.初始化串口
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 1个停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长8位
	USART_Init(USART2, &USART_InitStructure);
	// 4.使能串口及其中断
	USART_Cmd(USART2, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	// 5.初始化中断
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 响应优先级
	NVIC_Init(&NVIC_InitStructure);
}

/*
************************************************************
*	函数名称：	uart3_init
*
*	函数功能：	串口3初始化
*
*	入口参数：	bound：设定的波特率
*
*	返回参数：	无
*
*	说明：		TX-PB10		RX-PB11
* BY8001-16P MP3模块串口通信初始化
************************************************************
*/
void uart3_init(u32 bound)
{
	// GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // 使能USART3，GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// USART3_TX   GPIOB.10
	GPIO_InitStructure.GPIO_Pin = USART3_TX; // PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);			// 初始化GPIOA.9

	// USART3_RX	  GPIOB.11初始化
	GPIO_InitStructure.GPIO_Pin = USART3_RX;			  // PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);				  // 初始化GPIOA.10

	// Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // 子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器

	// USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;										// 串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式

	USART_Init(USART3, &USART_InitStructure);	   // 初始化串口3
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 开启串口接受中断
	USART_Cmd(USART3, ENABLE);					   // 使能串口3
}

// 串口3中断服务程序
void USART3_IRQHandler(void)
{
	uint8_t d;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 接收中断
	{
		// 接收数据
		d = USART_ReceiveData(USART3); // 接收数据
		if (U3_RX_Cnt > 1024)
			U3_RX_Cnt = 0;
		U3_RX_BUF[U3_RX_Cnt++] = d; // 记录接收到的值
		if (U3_RX_BUF[U3_RX_Cnt - 2] == 'O' && d == 'K')
		{
			U3_RX_Cmd = 1; // 接收到命令
			U3_RX_Cnt = 0;
		}
		USART_ClearITPendingBit(USART3, USART_IT_RXNE); // 清空标志位
	}
}

/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, u8 *str, unsigned short len)
{
	unsigned short count = 0;
	for (; count < len; count++)
	{
		USART_SendData(USARTx, *str++); // 发送数据
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
			; // 等待发送完成
	}
}

/*
************************************************************
*	函数名称：	UsartPrintf
*
*	函数功能：	格式化打印
*
*	入口参数：	USARTx：串口组
*				fmt：不定长参
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt, ...)
{
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;

	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap); // 格式化
	va_end(ap);

	while (*pStr != 0)
	{

		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
			;
		USART_SendData(USARTx, *pStr++);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
			;
	}
}


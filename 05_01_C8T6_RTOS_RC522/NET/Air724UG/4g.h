#ifndef __4G_H
#define __4G_H
//#include "sys.h"
#include "stdio.h"
#include "stm32f10x.h"

#define REV_OK 0   // 接收完成标志
#define REV_WAIT 1 // 接收未完成标志

// 外部引用串口接收数据完成的变量
extern u8  U2_RX_BUF[1024]; // 接收缓冲区
extern u16 U2_RX_Cnt;       // 接收计数
extern u8  U2_RX_Cmd;       // 接收命令到标志位 U2_RX_Cmd = 1
extern u8  U2_RX_Error;     // 接收到错误




void AIR724_Init(void);

void AIR724_Clear(void);

void AIR724_SendData(unsigned char *data, unsigned short len);

unsigned char *AIR724_GetIPD(unsigned short timeOut);

_Bool AIR724_SendCmd(char *cmd, char *res);

_Bool AIR724_WaitRecive(void);

// int AIR724_SendCmd1(char *cmd);

#endif

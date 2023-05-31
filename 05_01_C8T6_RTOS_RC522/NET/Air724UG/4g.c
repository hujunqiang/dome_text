#include "4g.h"
/**************************************************************
 *	文件名： 	4g.c
 *
 *	日期： 		2020-07-30
 *
 *	版本： 		V1.0
 *
 *	说明： 		AIR724驱动
 *
 *	修改记录：
 **************************************************************/

// 单片机头文件
#include "stm32f10x.h"
#include "oled.h"
// 网络设备驱动
#include "4g.h"

// 硬件驱动
#include "delay.h"
#include "usart1.h"

// C库
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


unsigned char air724_buf[256];
unsigned short air724_cnt = 0, air724_cntPre = 0;
u16 U2_RX_Cnt = 0;
u8 U2_RX_BUF[1024] = {0}; // 串口接收缓存区
u8 U2_RX_Cmd = 0;
u8 U2_RX_Error = 0;
extern u8 *dataPtr;



#define AIR724_MQTT_INFO "AT+MIPSTART=124.221.158.21,1883\r\n"

//==========================================================
//	函数名称：	AIR724_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：
//==========================================================
void AIR724_Clear(void)
{
	memset(air724_buf, 0, sizeof(air724_buf));
	air724_cnt = 0;
}

//==========================================================
//	函数名称：	AIR724_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool AIR724_WaitRecive(void)
{

	if (air724_cnt == 0) // 如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;

	if (air724_cnt == air724_cntPre) // 如果上一次的值和这次相同，则说明接收完毕
	{
		air724_cnt = 0; // 清0接收计数

		return REV_OK; // 返回接收完成标志
	}

	air724_cntPre = air724_cnt; // 置为相同

	return REV_WAIT; // 返回接收未完成标志
}

//==========================================================
//	函数名称：	AIR724_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：
//==========================================================
_Bool AIR724_SendCmd(char *cmd, char *res)
{
	unsigned char timeOut = 200;
	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	while (timeOut--)
	{
		if (AIR724_WaitRecive() == REV_OK) // 如果收到数据
		{
			if (strstr((const char *)air724_buf, res) != NULL) // 如果检索到关键词
			{
				AIR724_Clear(); // 清空缓存
				return 0;
			}
		}
//		delay_ms(10);
        delay_xms(10);
	}
	return 1;
}


//==========================================================
//	函数名称：	AIR724_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：
//==========================================================
void AIR724_SendData(unsigned char *data, unsigned short len)
{
	char cmdBuf[32];
	AIR724_Clear();								 // 清空接收缓存
	sprintf(cmdBuf, "AT+MIPSEND=0,%d\r\n", len); // 发送命令
	if (!AIR724_SendCmd(cmdBuf, ">"))			 // 收到‘>’时可以发送数据
	{
		Usart_SendString(USART2, data, len); // 发送设备连接请求数据
	}
}

//==========================================================
//	函数名称：	AIR724_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	timeOut等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//==========================================================
unsigned char *AIR724_GetIPD(unsigned short timeOut)
{
	char *ptrIPD;
	do{
		if (AIR724_WaitRecive() == REV_OK) // 如果接收完成
		{
			ptrIPD = strstr((char *)air724_buf, "+MSUB"); // 搜索“MIPURC”头
			if (ptrIPD == NULL)							  // 如果没找到，可能是IPDATA头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				// UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, '\n'); // 找到'\n'
				if (ptrIPD != NULL){
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
			}
		}
//		delay_ms(5); // 延时等待
        delay_xms(5); // 延时等待
		timeOut--;
	} while (timeOut > 0);
	return NULL; // 超时还未找到，返回空指针
}

//==========================================================
//	函数名称：	AIR724_Init
//
//	函数功能：	初始化AIR724
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：
//==========================================================
void AIR724_Init(void)
{
	AIR724_Clear();

//	UsartPrintf(USART_DEBUG, "01. AT\r\n");
	AIR724_SendCmd("AT\r\n", " ");

//    UsartPrintf(USART_DEBUG, "02. AT+MDISCONNECT\r\n");
	AIR724_SendCmd("AT+MDISCONNECT\r\n", "OK");

//	UsartPrintf(USART_DEBUG, "03. AT+MCONFIG=test,"
//							 ","
//							 "\r\n"); // cilentid,user,passwd
	while (AIR724_SendCmd("AT+MCONFIG=test,'',''\r\n", "OK"))
		delay_xms(50);

//	UsartPrintf(USART_DEBUG, "04. AT+MIPSTART\r\n");
	while (AIR724_SendCmd(AIR724_MQTT_INFO, "CONNECT OK"))
		delay_xms(50);

//	UsartPrintf(USART_DEBUG, "05. AT+MCONNECT\r\n");
	while (AIR724_SendCmd("AT+MCONNECT=1,60\r\n", "OK")) // CONNACK OK
		delay_xms(50);

//	UsartPrintf(USART_DEBUG, "06. AT+MSUB=/myrfid/sub\r\n");
	while (AIR724_SendCmd("AT+MSUB=/myrfid/sub,0\r\n", "SUBACK")) // 打印从订阅的topic收到的数据
		delay_xms(50);

//	UsartPrintf(USART_DEBUG, "07. AT+MQTTMSGGET\r\n");
	while (AIR724_SendCmd("AT+MQTTMSGGET\r\n", "OK")) // 打印从订阅的topic收到的数据
		delay_xms(50);

//	UsartPrintf(USART_DEBUG, "08. AIR724 Init OK\r\n");

}

//==========================================================
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	串口2收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：
//==========================================================
void USART2_IRQHandler(void)
{
	//+MSUB: "/soul/sub/",26 byte,{"target":"fan","value":1}
	uint8_t d;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 接收中断
	{
		// 接收数据
		d = USART_ReceiveData(USART2); // 接收数据
		if (U2_RX_Cnt > 1024)
			U2_RX_Cnt = 0;
		U2_RX_BUF[U2_RX_Cnt++] = d; // 记录接收到的值
		if (U2_RX_BUF[U2_RX_Cnt - 4] == 'b' && U2_RX_BUF[U2_RX_Cnt - 3] == 'y' && U2_RX_BUF[U2_RX_Cnt - 2] == 't' && d == 'e')
		{
			U2_RX_Cmd = 1; // 接收到命令
			U2_RX_Cnt = 0;
		}
		if (air724_cnt >= sizeof(air724_buf))
			air724_cnt = 0; // 防止串口被刷爆
		air724_buf[air724_cnt++] = USART2->DR;
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); // 清空标志位
	}
}


/*     2023-05-02
void AIR724_Init(void){
	AIR724_Clear();
	UsartPrintf(USART_DEBUG, "01. AT\r\n");
	AIR724_SendCmd("AT\r\n", " ");
    UsartPrintf(USART_DEBUG, "02. AT+MDISCONNECT\r\n");
	AIR724_SendCmd("AT+MDISCONNECT\r\n", "OK");
	UsartPrintf(USART_DEBUG, "03. AT+MCONFIG=test,"",""\r\n"); 
	while (AIR724_SendCmd("AT+MCONFIG=test,'',''\r\n", "OK"))delay_xms(50);
	UsartPrintf(USART_DEBUG, "04. AT+MIPSTART\r\n");
	while (AIR724_SendCmd(AIR724_MQTT_INFO, "CONNECT OK"))delay_xms(50);
	UsartPrintf(USART_DEBUG, "05. AT+MCONNECT\r\n");
	while (AIR724_SendCmd("AT+MCONNECT=1,60\r\n", "OK")) delay_xms(50);
	UsartPrintf(USART_DEBUG, "06. AT+MSUB=/myrfid/sub\r\n");
	while (AIR724_SendCmd("AT+MSUB=/myrfid/sub,0\r\n", "SUBACK")) delay_xms(50);
	UsartPrintf(USART_DEBUG, "07. AT+MQTTMSGGET\r\n");
	while (AIR724_SendCmd("AT+MQTTMSGGET\r\n", "OK"))delay_xms(50);
	UsartPrintf(USART_DEBUG, "08. AIR724 Init OK\r\n");
}


DROP TABLE IF EXISTS `mqtt_rfid`;
CREATE TABLE `mqtt_rfid`  (
  `ID` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL,
  `In_Time` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL,
  `Out_Time` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL,
  `Out_Area` text CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL,
  `id_t` int NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id_t`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 57 
CHARACTER SET = utf8mb4 
COLLATE = utf8mb4_0900_ai_ci 
ROW_FORMAT = Dynamic;







*/


// void AIR724_Init(void)
// {
// 	AIR724_Clear();
// 	UsartPrintf(USART_DEBUG, "01. AT\r\n");
// 	AIR724_SendCmd("AT\r\n", " ");
//     UsartPrintf(USART_DEBUG, "02. AT+MDISCONNECT\r\n");
// 	AIR724_SendCmd("AT+MDISCONNECT\r\n", "OK");
// 	UsartPrintf(USART_DEBUG, "03. AT+MCONFIG=test,"
// 							 ","
// 							 "\r\n"); // cilentid,user,passwd
// 	while (AIR724_SendCmd("AT+MCONFIG=test,'',''\r\n", "OK"))
// 		delay_xms(50);
// 	UsartPrintf(USART_DEBUG, "04. AT+MIPSTART\r\n");
// 	while (AIR724_SendCmd(AIR724_MQTT_INFO, "CONNECT OK"))
// 		delay_xms(50);
// 	UsartPrintf(USART_DEBUG, "05. AT+MCONNECT\r\n");
// 	while (AIR724_SendCmd("AT+MCONNECT=1,60\r\n", "OK")) // CONNACK OK
// 		delay_xms(50);
// 	UsartPrintf(USART_DEBUG, "06. AT+MSUB=/myrfid/sub\r\n");
// 	while (AIR724_SendCmd("AT+MSUB=/myrfid/sub,0\r\n", "SUBACK")) // 打印从订阅的topic收到的数据
// 		delay_xms(50);
// 	UsartPrintf(USART_DEBUG, "07. AT+MQTTMSGGET\r\n");
// 	while (AIR724_SendCmd("AT+MQTTMSGGET\r\n", "OK")) // 打印从订阅的topic收到的数据
// 		delay_xms(50);
// 	UsartPrintf(USART_DEBUG, "08. AIR724 Init OK\r\n");
// }




/*         203-05-02
 _Bool AIR724_WaitRecive(void){
	if (air724_cnt == 0) return REV_WAIT;
	if (air724_cnt == air724_cntPre) {
		air724_cnt = 0; 
		return REV_OK; 
	}
	air724_cntPre = air724_cnt; 
	return REV_WAIT; 
}
void AIR724_Clear(void){
	memset(air724_buf, 0, sizeof(air724_buf));
	air724_cnt = 0;
}
_Bool AIR724_SendCmd(char *cmd, char *res){
	unsigned char timeOut = 200;
	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	while (timeOut--){
		if (AIR724_WaitRecive() == REV_OK){ // 如果收到数据
			if (strstr((const char *)air724_buf, res) != NULL){ // 如果检索到关键词
				AIR724_Clear(); // 清空缓存
				return 0;
			}
		}delay_xms(10);
	}return 1;
}
*/


/*      05-20

void AIR724_Init(void)
{
	AIR724_Clear();

//	UsartPrintf(USART_DEBUG, "01. AT\r\n");
	AIR724_SendCmd("AT\r\n", " ");

//    UsartPrintf(USART_DEBUG, "02. AT+MDISCONNECT\r\n");
	AIR724_SendCmd("AT+MDISCONNECT\r\n", "OK");

//	UsartPrintf(USART_DEBUG, "03. AT+MCONFIG=test,"
//							 ","
//							 "\r\n"); // cilentid,user,passwd
	while (AIR724_SendCmd("AT+MCONFIG=test,'',''\r\n", "OK"))
		delay_xms(50);

//	UsartPrintf(USART_DEBUG, "04. AT+MIPSTART\r\n");
	while (AIR724_SendCmd(AIR724_MQTT_INFO, "CONNECT OK"))
		delay_xms(50);

//	UsartPrintf(USART_DEBUG, "05. AT+MCONNECT\r\n");
	while (AIR724_SendCmd("AT+MCONNECT=1,60\r\n", "OK")) // CONNACK OK
		delay_xms(50);

//	UsartPrintf(USART_DEBUG, "06. AT+MSUB=/myrfid/sub\r\n");
	while (AIR724_SendCmd("AT+MSUB=/myrfid/sub,0\r\n", "SUBACK")) // 打印从订阅的topic收到的数据
		delay_xms(50);

//	UsartPrintf(USART_DEBUG, "07. AT+MQTTMSGGET\r\n");
	while (AIR724_SendCmd("AT+MQTTMSGGET\r\n", "OK")) // 打印从订阅的topic收到的数据
		delay_xms(50);

//	UsartPrintf(USART_DEBUG, "08. AIR724 Init OK\r\n");

}
*/



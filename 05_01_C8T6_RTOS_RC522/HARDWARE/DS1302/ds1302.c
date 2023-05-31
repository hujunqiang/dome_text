#include "ds1302.h"
_calendar_obj calendar;
// 月份数据表
u8 const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};			   // 月修正数据表
const u8 mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // 平年的月份日期表
static void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;			  // RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 先占优先级1位,从优先级3位
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // 使能该通道中断
	NVIC_Init(&NVIC_InitStructure);							  // 根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

// 判断是否是闰年函数
// 月份   1  2  3  4  5  6  7  8  9  10 11 12
// 闰年   31 29 31 30 31 30 31 31 30 31 30 31
// 非闰年 31 28 31 30 31 30 31 31 30 31 30 31
// 输入:年份
// 输出:该年份是不是闰年.1,是.0,不是
u8 Is_Leap_Year(u16 year)
{
	if (year % 4 == 0) // 必须能被4整除
	{
		if (year % 100 == 0)
		{
			if (year % 400 == 0)
				return 1; // 如果以00结尾,还要能被400整除
			else
				return 0;
		}
		else
			return 1;
	}
	else
		return 0;
}

// 获得现在是星期几
// 功能描述:输入公历日期得到星期(只允许1901-2099年)
// 输入参数：公历年月日
// 返回值：星期号
u8 RTC_Get_Week(u16 year, u8 month, u8 day){
	u16 temp2;
	u8 yearH, yearL;
	yearH = year / 100;
	yearL = year % 100;
	// 如果为21世纪,年份数加100
	if (yearH > 19) yearL += 100;
	// 所过闰年数只算1900年之后的
	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7;
	temp2 = temp2 + day + table_week[month - 1];
	if (yearL % 4 == 0 && month < 3)    temp2--;
	return (temp2 % 7);
}

// 设置时钟
// 把输入的时钟转换为秒钟
// 以 1970 年 1 月 1 日为基准
// 1970~2099 年为合法年份
// 返回值:0,成功;其他:错误代码.
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec){
	u16 t;
	u32 seccount = 0;
	// 非合法年份则退出
	if (syear < 1970 || syear > 2099) return 1;
	for (t = 1970; t < syear; t++){ // 把所有年份的秒钟相加
		if (Is_Leap_Year(t)) seccount += 31622400; // 闰年的秒钟数
		else                 seccount += 31536000; // 平年的秒钟数
	}smon -= 1;
	for (t = 0; t < smon; t++){ // 把前面月份的秒钟数相加
		seccount += (u32)mon_table[t] * 86400; // 月份秒钟数相加
		if (Is_Leap_Year(syear) && t == 1)
			seccount += 86400; // 闰年 2 月份增加一天的秒钟数
	}
	seccount += (u32)(sday - 1) * 86400;									 // 把前面日期的秒钟数相加
	seccount += (u32)hour * 3600;											 // 小时秒钟数
	seccount += (u32)min * 60;												 // 分钟秒钟数
	seccount += sec;														 // 最后的秒钟加上去
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); // 使能 PWR 和 BKP 外设时钟
	PWR_BackupAccessCmd(ENABLE);											 // 使能 RTC 和后备寄存器访问
	RTC_SetCounter(seccount);												 // 设置 RTC 计数器的值
	RTC_WaitForLastTask();													 // 等待最近一次对 RTC 寄存器的写操作完成
	return 0;
}

// 得到当前的时间，结果保存在 calendar 结构体里面
// 返回值:0,成功;其他:错误代码.
u8 RTC_Get(void){
	static u16 daycnt = 0;
	u32 timecount ,temp,temp1= 0;
	timecount = RTC->CNTH; // 得到计数器中的值(秒钟数)
	timecount <<= 16;
	timecount += RTC->CNTL;
	temp = timecount / 86400; // 得到天数(秒钟数对应的)
	if (daycnt != temp)	{	  // 超过一天了
		daycnt = temp;
		temp1 = 1970; // 从 1970 年开始
		while (temp >= 365){
			if (Is_Leap_Year(temp1)){ // 是闰年
				if (temp >= 366) temp -= 366; // 闰年的秒钟数
				else          break;
			}else   temp -= 365; // 平年
			temp1++;
		}calendar.w_year = temp1; // 得到年份
		temp1 = 0;
		while (temp >= 28){ // 超过了一个月
			if (Is_Leap_Year(calendar.w_year) && temp1 == 1){ // 当年是不是闰年/2 月份
				if (temp >= 29)temp -= 29; // 闰年的秒钟数
				else           break;
			}else{if (temp >= mon_table[temp1]) temp -= mon_table[temp1]; // 平年
				  else         break;
			}temp1++;
		}
		calendar.w_month = temp1 + 1; // 得到月份
		calendar.w_date = temp + 1;	  // 得到日期
	}
	temp = timecount % 86400;														  // 得到秒钟数
	calendar.hour = temp / 3600;													  // 小时
	calendar.min = (temp % 3600) / 60;												  // 分钟
	calendar.sec = (temp % 3600) % 60;												  // 秒钟
	calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date); // 获取星期
	return 0;
}

uint16_t RTC_Init(void)
{
	// 用于检查是不是第一次配置时钟
	uint16_t temp = 0;
	// 使能电源时钟和备份区域时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	// 使能后备寄存器访问
	PWR_BackupAccessCmd(ENABLE);
	// 从指定的后备寄存器中读出数据，读出了与写入的指定数据不相乎
	// 即不是第一次配置时钟
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x6666)
	{
		BKP_DeInit();			   // 复位备份区域
		RCC_LSEConfig(RCC_LSE_ON); // 设置外部低速晶振(LSE)
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && temp < 200)
		{

			temp++;
            delay_xms(10);
		}
		if (temp >= 200)
		{
			return 1; // 初始化时钟失败,晶振有问题
		}
		else
		{
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); // 设置 RTC 时钟
			RCC_RTCCLKCmd(ENABLE);					  // 使能 RTC 时钟
			RTC_WaitForLastTask();					  // 等待最近一次对 RTC 寄存器的写操作完成
			RTC_WaitForSynchro();					  // 等待 RTC 寄存器同步
			RTC_ITConfig(RTC_IT_SEC, ENABLE);		  // 使能 RTC 秒中断
			RTC_WaitForLastTask();					  // 等待最近一次对 RTC 寄存器的写操作完成
			RTC_EnterConfigMode();					  // 允许配置
			RTC_SetPrescaler(32767);				  // 设置 RTC 预分频的值
			RTC_WaitForLastTask();					  // 等待最近一次对 RTC 寄存器的写操作完成
			RTC_Set(2023, 05, 25, 00, 53, 50);		  // 设置时间
			RTC_ExitConfigMode();					  // 退出配置模式
			BKP_WriteBackupRegister(BKP_DR1, 0X6666); // 向指定的后备寄存器中
													  // 写入用户程序数据 0x5050
		}
	}
	else // 若为是，则继续计时
	{
		RTC_WaitForSynchro();			  // 等待最近一次对 RTC 寄存器的写操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE); // 使能 RTC 秒中断
		RTC_WaitForLastTask();			  // 等待最近一次对 RTC 寄存器的写操作完成
	}
	RTC_NVIC_Config(); // RCT 中断分组设置
	RTC_Get();		   // 更新时间
	return 0;		   // ok
}

// RTC时钟中断
// 每秒触发一次
void RTC_IRQHandler(void)
{
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) // 秒钟中断
	{
		RTC_Get(); // 更新时间
//	printf("RTC Time:%d-%d-%d %d:%d:%d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//输出闹铃时间
	}
	RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW); // 清闹钟中断
	RTC_WaitForLastTask();
}

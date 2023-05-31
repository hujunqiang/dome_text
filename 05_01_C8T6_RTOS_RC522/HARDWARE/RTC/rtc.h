#ifndef _RTC_H_
#define _RTC_H_
#include "stm32f10x.h"
//#include "sys.h"

typedef struct
{
	volatile u8 hour;
	volatile u8 min;
	volatile u8 sec;
	volatile u16 w_year;
	volatile u8 w_month;
	volatile u8 w_date;
	volatile u8 week;
}RTC_VARIABLE;

extern RTC_VARIABLE rtc;//日历结构体

extern u8 const mon_table[12];//月份日期数据表
void Dis_Time(u8 x,u8 y,u8 size);//在制定位置开始显示
void Dis_Week(u8 x,u8 y,u8 size,u8 lang);//在知道位置显示星期
u8 RTC_Init(void);//初始化RTC，返回0，失败；1，成功；
u8 Is_Leap_Tear(u16 year);//平年，闰年判断
u8 RTC_Alarm_Set(u16 syear,u16 smon,u8 sday,u8 hour,u16 min,u16 sec);
u8 RTC_Get(void);//更新时间
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u16 smon,u8 sday,u8 hour,u16 min,u16 sec);

#endif

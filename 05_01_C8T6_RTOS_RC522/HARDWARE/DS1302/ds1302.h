#ifndef __DS1302_H
#define __DS1302_H
//#include "sys.h"
#include "delay.h"
#include "usart1.h"
#include "stdio.h"
#include "stm32f10x.h"
typedef struct {
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;
	vu8 hour;
	vu8 min;
	vu8 sec;
}_calendar_obj;


u8 Is_Leap_Year(u16 year);
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
u8 RTC_Get(void);
uint16_t RTC_Init(void);




#endif



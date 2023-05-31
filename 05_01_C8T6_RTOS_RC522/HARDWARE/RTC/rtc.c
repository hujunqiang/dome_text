#include "rtc.h"
#include "delay.h"
#include  "led.h"
#include "usart1.h"
//#include "oled.h"
#include <stdio.h>

/*
功能：设置RTC，进行1秒钟中断，串口打印当时时间
编程：
   1、开启时钟，BKP后备域（backup domain）,PWR
	 2、RTC CORE BKP 解锁
	 3、RTC(后备电源情况)设置一次进行工作
	 进行判断RTC是否已经设置过，若没有：
	 复位备份区域
	 LSE：32768hz开启外部低速时钟
	 等待时钟稳定（）
	 {
	  while(GETFLAG && 时间)；
   }
	 if（判断时间是不是超过超时时间）
	 1、如果标准位是已经完成就执行下面的程序，
	 2、如果的时间超时，就返回1
	 配置RTC输入时钟
	 使能RTC时钟
	 判断和等待最近一次对RTC寄存器的写操作完成
	 使能RTC秒中断
	 等待最近一次对RTC寄存器的写操作完成
	 
	 进入配置模式
	 设置分频
	 等待完成
	 设置初始时间
	 等待完成
	 退出配置模式
	 NVIC RTC中断
	 向指定的后备寄存器1中写入用户程序数据：0x6666
	4.更新中断开启
*/

RTC_VARIABLE rtc;//时钟结构体

static void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel=RTC_IRQn;//RTC中断
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//使能该中断；
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//响应优先级
	NVIC_Init(&NVIC_InitStruct);
}

/*
实时时钟配置
初始化RTC时钟，同时检测时钟是否工作正常
BKP->DR1用于保存是否第一次配置的设置
返回0：正常
其他：错误代码
*/

u8 RTC_Init()
{
	//判断是不是第一次配置时钟
	u8 temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP|RCC_APB1Periph_PWR,ENABLE);//使能PWR和BKP外设时钟
	PWR_BackupAccessCmd(ENABLE);
	if(BKP_ReadBackupRegister(BKP_DR1)!=0x6666)
	{
		BKP_DeInit();//复位备份区域
		RCC_LSEConfig(RCC_LSE_ON);//设置外部低速晶振（LSE），使用外部低速晶振
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET&&temp<200)//初始化外部时钟
		{
			temp++;
			delay_ms(10);
		}
		if(temp>=200)
		{
			return 1;//初始化时钟失败，晶振有问题
		}
		else
		{
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//设置RTC时钟（RTCCLK），选择LSE作为RTC时钟
			RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
			RTC_WaitForLastTask();//等待最近一次对RTC寄存器的写操作完成
			RTC_WaitForSynchro();//等待RTC寄存器同步
			RTC_ITConfig(RTC_IT_SEC,ENABLE);//使能RTC秒中断
			RTC_WaitForLastTask();//等待最近一次对RTC寄存器的写操作完成
			RTC_EnterConfigMode();//允许配置
			RTC_SetPrescaler(32768-1);//设置RTC预分频的值，实现1s
			RTC_WaitForLastTask();//等待最近一次对RTC寄存器的写操作完成
			RTC_Set(2022,1,28,23,59,50);//设置时间
			RTC_ExitConfigMode();//退出配置模式
			BKP_WriteBackupRegister(BKP_DR1,0x6666);//向指定的后备寄存器中写入用户数据
		}
	}
	else//系统继续计时
	{
		RTC_WaitForSynchro();//等待寄存器同步完成
		RTC_ITConfig(RTC_IT_SEC,ENABLE);//使能RTC秒中断
		RTC_WaitForLastTask();//等待最近一次对RTC寄存器的写操作完成
	}
	RTC_NVIC_Config();//RTC中断分组设置
	RTC_Get();//更新时间
	return 0; //完成
}
/*
RTC时钟中断
每一秒触发一次

*/
void RTC_IRQHandler()
{
	//LED=0;
	if(RTC_GetITStatus(RTC_IT_SEC)!=RESET)//秒中断
	{
		RTC_Get();
//		OLED_ShowNum(7,6,rtc.w_year,4,16,1);      //年数字
//			OLED_ShowNum(56,6,rtc.w_month,2,16,1);      //月数字
//			OLED_ShowNum(88,6,rtc.w_date,2,16,1);       //日数字
//			OLED_ShowNum(7,27,rtc.hour,2,16,1);      //小时数字
//			OLED_ShowNum(32,27,rtc.min,2,16,1);     //分钟数字
//			OLED_ShowNum(56,27,rtc.sec,2,16,1);     //秒数字
//			OLED_ShowChinese(104,27,148+rtc.week,16,1);     //
//			OLED_Refresh();
		printf("Time:%d-%d-%d %d:%d:%d %d\r\n",rtc.w_year,rtc.w_month,rtc.w_date,rtc.hour,rtc.min,rtc.sec,rtc.week);
	}
	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
	RTC_WaitForLastTask();
}
/*
判断是不是润年函数（被4可以整除，100不可以，400可以整除的才是闰年）
	月份 1  2  3  4  5  6  7  8  9  10  11  12
	闰年 31 29 31 30 31 30 31 31 30  31  30  31
非闰年 31 28 31 30 31 30 31 31 30  31  30  31
输入：年份
输出：该年是不是闰年，1是，0不是
*/
u8 Is_Leap_Tear(u16 year)
{
	if(year%4==0)
	{
		if(year%100==0)
		{
			if(year%400==0)return 1;
			else return 0;
		}else return 1;
	}else return 0;
}

/*
设置时钟
把输入的时钟转换为秒钟
以1970年1月1日为基准
1970-2099年为合法年份
返回值：0成功；其他：错误代码
月份数据表
*/
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5};//月修正数据表
//平均的月份日期表
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u8 RTC_Set(u16 syear,u16 smon,u8 sday,u8 hour,u16 min,u16 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;
	for(t=1970;t<syear;t++)//把所有年份的秒钟相加
	{
		if(Is_Leap_Tear(t))seccount+=31622400;//闰年的秒钟数
		else seccount+=31536000;              //平年秒钟数
	}
	smon-=1;
	for(t=0;t<smon;t++)
	{
		seccount+=(u32)mon_table[t]*86400;//月份秒钟数相加
		if(Is_Leap_Tear(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数
	}
	seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟数相加
	seccount+=(u32)hour*3600;//小时秒钟数
	seccount+=(u32)min*60;//分钟秒钟数
	seccount+=(u32)sec;//最后的秒钟加上去
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);//使能pwr和bkp外设时钟
	PWR_BackupAccessCmd(ENABLE);//使能RTC和后备寄存器访问
	RTC_SetCounter(seccount);    //设置RTC计数器的值
	
	RTC_WaitForLastTask();//等待最近一次对RTC寄存器的写操作完成
 return 0;
}
//初始化闹钟
//以1970年1月1日为基准
//1970-2099年为合法年份
//syear,smon,sday,hour,min,sec;闹钟的年月日时分秒
//返回值：0成功；其他：错误代码
u8 RTC_Alarm_Set(u16 syear,u16 smon,u8 sday,u8 hour,u16 min,u16 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;
	for(t=1970;t<syear;t++)//把所有年份的秒钟相加
	{
		if(Is_Leap_Tear(t))seccount+=31622400;//闰年的秒钟数
		else seccount+=31536000;              //平年秒钟数
	}
	smon-=1;
	for(t=0;t<smon;t++)
	{
		seccount+=(u32)mon_table[t]*86400;//月份秒钟数相加
		if(Is_Leap_Tear(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数
	}
	seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟数相加
	seccount+=(u32)hour*3600;//小时秒钟数
	seccount+=(u32)min*60;//分钟秒钟数
	seccount+=(u32)sec;//最后的秒钟加上去
	//设置时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);//使能pwr和bkp外设时钟
	PWR_BackupAccessCmd(ENABLE);//使能RTC和后备寄存器访问
	RTC_SetCounter(seccount);    //设置RTC计数器的值
	
	RTC_WaitForLastTask();//等待最近一次对RTC寄存器的写操作完成
 return 0;
}

//得到当前值
//返回值：0，成功；其他:错误代码
u8 RTC_Get()
{
	static u16 daycnt=0;
	u32 timecount=0;
	u32 temp=0;
	u16 temp1=0;
	timecount=RTC_GetCounter();
	temp=timecount/86400;//得到天数（秒钟数对应的）
	if(daycnt !=temp)
	{
		daycnt=temp;
		temp1=1970;
		while(temp>=365)
		{
			if(Is_Leap_Tear(temp1))//是闰年
			{
				if(temp>=366)temp-=366;//闰年的秒钟数
				else{temp++;break;}
			}
			else temp-=365;//平年
			temp1++;
		}
		rtc.w_year=temp1;//得到年份
		temp1=0;
		while(temp>=28)//超过一个月
		{
			if(Is_Leap_Tear(rtc.w_year)&&temp1==1)//年是不是闰年/2月份
			{
				if(temp>=29)temp-=29;//闰年的秒钟数
				else break;
			}
			else
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++;
		}
		rtc.w_month=temp1+1;//得到月份
		rtc.w_date=temp+1;//得到日期
	}
	temp=timecount%86400;//得到秒钟数
	rtc.hour=temp/3600;//小时
	rtc.min=(temp%3600)/60;//分钟
	rtc.sec=(temp%3600)%60;//秒钟
	if(rtc.w_month==1||rtc.w_month==2)
	{
			rtc.week=(rtc.w_date+1+2*(rtc.w_month+12)+(3*(rtc.w_month+13)/5)+rtc.w_year-1+((rtc.w_year-1)/4)-((rtc.w_year-1)/100)+((rtc.w_year-1)/400)) % 7;
	}
	else
	{
			rtc.week=(rtc.w_date+1+2*rtc.w_month+(3*(rtc.w_month+1)/5)+rtc.w_year+(rtc.w_year/4)-(rtc.w_year/100)+(rtc.w_year/400)) % 7;
	}
	//rtc.week=RTC_Get_Week(rtc.w_year,rtc.w_month,rtc.w_date);//获得星期
	return 0;
}
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{
	u16 temp2;
	u8 yearH,yearL;
	yearH=year/100;yearL=year%100;
	//如果为21世纪，年份数加100
	if(yearH>19)yearL+=100;
	//所过闰年数只算1900年之后的
	temp2=yearL+yearH/4;
	temp2=temp2%7;
	temp2=temp2+day+table_week[month-1];
	if(yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}


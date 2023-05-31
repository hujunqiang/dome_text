#include "rtc.h"
#include "delay.h"
#include  "led.h"
#include "usart1.h"
//#include "oled.h"
#include <stdio.h>

/*
���ܣ�����RTC������1�����жϣ����ڴ�ӡ��ʱʱ��
��̣�
   1������ʱ�ӣ�BKP����backup domain��,PWR
	 2��RTC CORE BKP ����
	 3��RTC(�󱸵�Դ���)����һ�ν��й���
	 �����ж�RTC�Ƿ��Ѿ����ù�����û�У�
	 ��λ��������
	 LSE��32768hz�����ⲿ����ʱ��
	 �ȴ�ʱ���ȶ�����
	 {
	  while(GETFLAG && ʱ��)��
   }
	 if���ж�ʱ���ǲ��ǳ�����ʱʱ�䣩
	 1�������׼λ���Ѿ���ɾ�ִ������ĳ���
	 2�������ʱ�䳬ʱ���ͷ���1
	 ����RTC����ʱ��
	 ʹ��RTCʱ��
	 �жϺ͵ȴ����һ�ζ�RTC�Ĵ�����д�������
	 ʹ��RTC���ж�
	 �ȴ����һ�ζ�RTC�Ĵ�����д�������
	 
	 ��������ģʽ
	 ���÷�Ƶ
	 �ȴ����
	 ���ó�ʼʱ��
	 �ȴ����
	 �˳�����ģʽ
	 NVIC RTC�ж�
	 ��ָ���ĺ󱸼Ĵ���1��д���û��������ݣ�0x6666
	4.�����жϿ���
*/

RTC_VARIABLE rtc;//ʱ�ӽṹ��

static void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel=RTC_IRQn;//RTC�ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//ʹ�ܸ��жϣ�
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);
}

/*
ʵʱʱ������
��ʼ��RTCʱ�ӣ�ͬʱ���ʱ���Ƿ�������
BKP->DR1���ڱ����Ƿ��һ�����õ�����
����0������
�������������
*/

u8 RTC_Init()
{
	//�ж��ǲ��ǵ�һ������ʱ��
	u8 temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP|RCC_APB1Periph_PWR,ENABLE);//ʹ��PWR��BKP����ʱ��
	PWR_BackupAccessCmd(ENABLE);
	if(BKP_ReadBackupRegister(BKP_DR1)!=0x6666)
	{
		BKP_DeInit();//��λ��������
		RCC_LSEConfig(RCC_LSE_ON);//�����ⲿ���پ���LSE����ʹ���ⲿ���پ���
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET&&temp<200)//��ʼ���ⲿʱ��
		{
			temp++;
			delay_ms(10);
		}
		if(temp>=200)
		{
			return 1;//��ʼ��ʱ��ʧ�ܣ�����������
		}
		else
		{
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//����RTCʱ�ӣ�RTCCLK����ѡ��LSE��ΪRTCʱ��
			RCC_RTCCLKCmd(ENABLE);//ʹ��RTCʱ��
			RTC_WaitForLastTask();//�ȴ����һ�ζ�RTC�Ĵ�����д�������
			RTC_WaitForSynchro();//�ȴ�RTC�Ĵ���ͬ��
			RTC_ITConfig(RTC_IT_SEC,ENABLE);//ʹ��RTC���ж�
			RTC_WaitForLastTask();//�ȴ����һ�ζ�RTC�Ĵ�����д�������
			RTC_EnterConfigMode();//��������
			RTC_SetPrescaler(32768-1);//����RTCԤ��Ƶ��ֵ��ʵ��1s
			RTC_WaitForLastTask();//�ȴ����һ�ζ�RTC�Ĵ�����д�������
			RTC_Set(2022,1,28,23,59,50);//����ʱ��
			RTC_ExitConfigMode();//�˳�����ģʽ
			BKP_WriteBackupRegister(BKP_DR1,0x6666);//��ָ���ĺ󱸼Ĵ�����д���û�����
		}
	}
	else//ϵͳ������ʱ
	{
		RTC_WaitForSynchro();//�ȴ��Ĵ���ͬ�����
		RTC_ITConfig(RTC_IT_SEC,ENABLE);//ʹ��RTC���ж�
		RTC_WaitForLastTask();//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	}
	RTC_NVIC_Config();//RTC�жϷ�������
	RTC_Get();//����ʱ��
	return 0; //���
}
/*
RTCʱ���ж�
ÿһ�봥��һ��

*/
void RTC_IRQHandler()
{
	//LED=0;
	if(RTC_GetITStatus(RTC_IT_SEC)!=RESET)//���ж�
	{
		RTC_Get();
//		OLED_ShowNum(7,6,rtc.w_year,4,16,1);      //������
//			OLED_ShowNum(56,6,rtc.w_month,2,16,1);      //������
//			OLED_ShowNum(88,6,rtc.w_date,2,16,1);       //������
//			OLED_ShowNum(7,27,rtc.hour,2,16,1);      //Сʱ����
//			OLED_ShowNum(32,27,rtc.min,2,16,1);     //��������
//			OLED_ShowNum(56,27,rtc.sec,2,16,1);     //������
//			OLED_ShowChinese(104,27,148+rtc.week,16,1);     //
//			OLED_Refresh();
		printf("Time:%d-%d-%d %d:%d:%d %d\r\n",rtc.w_year,rtc.w_month,rtc.w_date,rtc.hour,rtc.min,rtc.sec,rtc.week);
	}
	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
	RTC_WaitForLastTask();
}
/*
�ж��ǲ������꺯������4����������100�����ԣ�400���������Ĳ������꣩
	�·� 1  2  3  4  5  6  7  8  9  10  11  12
	���� 31 29 31 30 31 30 31 31 30  31  30  31
������ 31 28 31 30 31 30 31 31 30  31  30  31
���룺���
����������ǲ������꣬1�ǣ�0����
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
����ʱ��
�������ʱ��ת��Ϊ����
��1970��1��1��Ϊ��׼
1970-2099��Ϊ�Ϸ����
����ֵ��0�ɹ����������������
�·����ݱ�
*/
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5};//���������ݱ�
//ƽ�����·����ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u8 RTC_Set(u16 syear,u16 smon,u8 sday,u8 hour,u16 min,u16 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;
	for(t=1970;t<syear;t++)//��������ݵ��������
	{
		if(Is_Leap_Tear(t))seccount+=31622400;//�����������
		else seccount+=31536000;              //ƽ��������
	}
	smon-=1;
	for(t=0;t<smon;t++)
	{
		seccount+=(u32)mon_table[t]*86400;//�·����������
		if(Is_Leap_Tear(syear)&&t==1)seccount+=86400;//����2�·�����һ���������
	}
	seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ����������
	seccount+=(u32)hour*3600;//Сʱ������
	seccount+=(u32)min*60;//����������
	seccount+=(u32)sec;//�������Ӽ���ȥ
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);//ʹ��pwr��bkp����ʱ��
	PWR_BackupAccessCmd(ENABLE);//ʹ��RTC�ͺ󱸼Ĵ�������
	RTC_SetCounter(seccount);    //����RTC��������ֵ
	
	RTC_WaitForLastTask();//�ȴ����һ�ζ�RTC�Ĵ�����д�������
 return 0;
}
//��ʼ������
//��1970��1��1��Ϊ��׼
//1970-2099��Ϊ�Ϸ����
//syear,smon,sday,hour,min,sec;���ӵ�������ʱ����
//����ֵ��0�ɹ����������������
u8 RTC_Alarm_Set(u16 syear,u16 smon,u8 sday,u8 hour,u16 min,u16 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;
	for(t=1970;t<syear;t++)//��������ݵ��������
	{
		if(Is_Leap_Tear(t))seccount+=31622400;//�����������
		else seccount+=31536000;              //ƽ��������
	}
	smon-=1;
	for(t=0;t<smon;t++)
	{
		seccount+=(u32)mon_table[t]*86400;//�·����������
		if(Is_Leap_Tear(syear)&&t==1)seccount+=86400;//����2�·�����һ���������
	}
	seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ����������
	seccount+=(u32)hour*3600;//Сʱ������
	seccount+=(u32)min*60;//����������
	seccount+=(u32)sec;//�������Ӽ���ȥ
	//����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);//ʹ��pwr��bkp����ʱ��
	PWR_BackupAccessCmd(ENABLE);//ʹ��RTC�ͺ󱸼Ĵ�������
	RTC_SetCounter(seccount);    //����RTC��������ֵ
	
	RTC_WaitForLastTask();//�ȴ����һ�ζ�RTC�Ĵ�����д�������
 return 0;
}

//�õ���ǰֵ
//����ֵ��0���ɹ�������:�������
u8 RTC_Get()
{
	static u16 daycnt=0;
	u32 timecount=0;
	u32 temp=0;
	u16 temp1=0;
	timecount=RTC_GetCounter();
	temp=timecount/86400;//�õ���������������Ӧ�ģ�
	if(daycnt !=temp)
	{
		daycnt=temp;
		temp1=1970;
		while(temp>=365)
		{
			if(Is_Leap_Tear(temp1))//������
			{
				if(temp>=366)temp-=366;//�����������
				else{temp++;break;}
			}
			else temp-=365;//ƽ��
			temp1++;
		}
		rtc.w_year=temp1;//�õ����
		temp1=0;
		while(temp>=28)//����һ����
		{
			if(Is_Leap_Tear(rtc.w_year)&&temp1==1)//���ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;//�����������
				else break;
			}
			else
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;
		}
		rtc.w_month=temp1+1;//�õ��·�
		rtc.w_date=temp+1;//�õ�����
	}
	temp=timecount%86400;//�õ�������
	rtc.hour=temp/3600;//Сʱ
	rtc.min=(temp%3600)/60;//����
	rtc.sec=(temp%3600)%60;//����
	if(rtc.w_month==1||rtc.w_month==2)
	{
			rtc.week=(rtc.w_date+1+2*(rtc.w_month+12)+(3*(rtc.w_month+13)/5)+rtc.w_year-1+((rtc.w_year-1)/4)-((rtc.w_year-1)/100)+((rtc.w_year-1)/400)) % 7;
	}
	else
	{
			rtc.week=(rtc.w_date+1+2*rtc.w_month+(3*(rtc.w_month+1)/5)+rtc.w_year+(rtc.w_year/4)-(rtc.w_year/100)+(rtc.w_year/400)) % 7;
	}
	//rtc.week=RTC_Get_Week(rtc.w_year,rtc.w_month,rtc.w_date);//�������
	return 0;
}
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{
	u16 temp2;
	u8 yearH,yearL;
	yearH=year/100;yearL=year%100;
	//���Ϊ21���ͣ��������100
	if(yearH>19)yearL+=100;
	//����������ֻ��1900��֮���
	temp2=yearL+yearH/4;
	temp2=temp2%7;
	temp2=temp2+day+table_week[month-1];
	if(yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}


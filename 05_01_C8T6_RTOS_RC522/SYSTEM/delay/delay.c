#include "delay.h"

u32 fac_ms,fac_us;

extern void xPortSysTickHandler(void);

//初始化延时函数
void Delay_Init(void)
{
	u32 reload;
	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);	//选择外部时钟 当前外部时钟为8Mhz
	fac_us = SystemCoreClock/1000000;	
	reload = SystemCoreClock/1000000;
	reload *= 1000000/configTICK_RATE_HZ;				//根据configTICK_RATE_HZ设定溢出时间
														//reload为24位寄存器,最大值:16777216
														//在72M下,约合0.233s左右
	
	fac_ms = 1000/configTICK_RATE_HZ;					//代表RTOS可以延时的最少单位	
	
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;			//开启systick中断
	SysTick->LOAD=reload;								//重装初始值
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;				//开启systick
}

//延时nus
//nus:要延时的us数.	
//nus:0~204522252(最大值即2^32/fac_us@fac_us=168)	    								   
void delay_xus(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};										    
}

//延时秒
void delay_s(uint32_t delay)
{
	while(delay--)
	{
//		delay_ms(500);
//		delay_ms(500);
        delay_xms(500);
		delay_xms(500);
	}
//	vTaskDelay(1000 * delay);
}

//延时nms
//nms:要延时的ms数
//nms:0~65535
void delay_ms(u32 nms)
{	
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
	{		
		if(nms>=fac_ms)						//延时的时间大于OS的最少时间周期 
		{ 
   			vTaskDelay(nms/fac_ms);	 		//FreeRTOS延时
		}
		nms%=fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_xus((u32)(nms*1000));				//普通方式延时
}

//延时nms,不会引起任务调度
//nms:要延时的ms数
void delay_xms(u32 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_xus(1000);
}

//systick中断服务函数
void SysTick_Handler(void)
{	    
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
     {
        xPortSysTickHandler();
     }
}





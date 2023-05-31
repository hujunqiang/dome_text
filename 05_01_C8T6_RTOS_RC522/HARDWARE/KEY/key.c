#include "key.h"
#include "stm32f10x.h"
#include "delay.h"

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;// 上拉输入   GPIO_Mode_IN_FLOATING
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 |  GPIO_Pin_7; //| GPIO_Pin_4 | GPIO_Pin_12
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


int read_key1(void){
    if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == 0){
        delay_ms(20);
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == 0){
            while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == 0){
                delay_ms(20);
            }
            return 1;
        }
    }
    return 0;
}
int read_key2(void){
    if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == 0){
        delay_ms(20);
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == 0){
            while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == 0){
                delay_ms(20);
            }
            return 1;
        }
    }
    return 0;
}
int read_key3(void){
    if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == 0){
        delay_ms(20);
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == 0){
            while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == 0){
                delay_ms(20);
            }
            return 1;
        }
    }
    return 0;
}

int read_key4(void){
    if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) == 0){
        delay_ms(20);
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) == 0){
            while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) == 0){
                delay_ms(20);
            }
            return 1;
        }
    }
    return 0;
}



#if 0

u8 Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
	{
//		delay_ms(20);
        delay_xms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
			;
//		delay_ms(20);
        delay_xms(20);
		KeyNum = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)
	{
//		delay_ms(20);
    delay_xms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)
			;
//		delay_ms(20);
        delay_xms(20);
		KeyNum = 2;
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
	{
//		delay_ms(20);
        delay_xms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
			;
//		delay_ms(20);
        delay_xms(20);
		KeyNum = 3;
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0)
	{
//		delay_ms(20);
        delay_xms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0)
			;
//		delay_ms(20);
        delay_xms(20);
		KeyNum = 4;
	}

	return KeyNum;
}





// 按键检测函数
// mode 0不支持连按 1支持连按
u8 KEY_Scan(u8 mode)
{
	static u8 key_up = 1; // 按键松开标志
	if (mode)
		key_up = 1;
	if (key_up && (KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0))
	{
		delay_ms(10); // 消抖
		key_up = 0;	  // 按键已松开
		if (KEY1 == 0)
			return KEY1_PRESS;
		else if (KEY2 == 0)
			return KEY2_PRESS;
		else if (KEY3 == 0)
			return KEY3_PRESS;
		else if (KEY4 == 0)
			return KEY4_PRESS;
	}
	else if (KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1)
		key_up = 1;
	return 0;
}

#endif

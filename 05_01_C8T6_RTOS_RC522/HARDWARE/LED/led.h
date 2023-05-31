#ifndef __LED_H
#define __LED_H	 
//#include "sys.h"
#include "stm32f10x.h"

//LEDµ∆◊¥Ã¨
#define LED1_STATU(i) i?GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET):GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);
#define LED2_STATU(i) i?GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_SET):GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_RESET);
#define LED3_STATU(i) i?GPIO_WriteBit(GPIOB,GPIO_Pin_7,Bit_SET):GPIO_WriteBit(GPIOB,GPIO_Pin_7,Bit_RESET);

//#define LED1 PBout(5)	// PB5£¨LED1=0¡¡	
//#define LED2 PBout(6)	// PB6£¨LED2=0¡¡
//#define LED3 PBout(7)	// PB7£¨LED3=0¡¡

void LED_Init(void);//≥ı ºªØ

void LED_Init(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED1_Turn(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED2_Turn(void);
void LED3_ON(void);
void LED3_OFF(void);
void LED3_Turn(void);



#endif

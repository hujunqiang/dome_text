
#ifndef __u8g2init_h
#define __u8g2init_h

#define SCL_Pin GPIO_Pin_10
#define SDA_Pin GPIO_Pin_11
#define IIC_GPIO_Port GPIOB
#include "u8g2.h"


void IIC_Init(void);
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void u8g2Init(u8g2_t *u8g2);
void draw(u8g2_t *u8g2);

#endif





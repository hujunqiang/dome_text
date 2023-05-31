#ifndef __TCRT5000_H
#define __TCRT5000_H
#include <stdbool.h>
#include "stm32f10x.h"
#include "stdio.h"

bool tcrt5000_1(void);
bool tcrt5000_2(void);
bool tcrt5000_3(void);
bool tcrt5000_4(void);
 
//int tcrt5000_1(void);
//int tcrt5000_2(void);

void tcrt5000_1_init(void);
void tcrt5000_2_init(void);
void tcrt5000_3_init(void);
void tcrt5000_4_init(void);

#endif

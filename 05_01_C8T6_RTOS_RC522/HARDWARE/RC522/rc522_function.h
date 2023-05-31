#ifndef __RC522_FUNCTION_H
#define	__RC522_FUNCTION_H

#include <stm32f10x.h>  
#include <stdio.h>
#include "stm32f10x_it.h"
#include "rc522_config.h"
#include "stm32f10x.h"
#include "delay.h"
#include "usart1.h"

#define          macDummy_Data              0x00

void             PcdReset              ( void );                        //复位
void             M500PcdConfigISOType  ( u8 type  );                    //工作方式
char             PcdRequest            ( u8 req_code, u8 * pTagType );  //寻卡
char             PcdAnticoll           ( u8 * pSnr);                    //读卡号
char *           IC_test               ( void );         
char *           ReaderCard            ( void );

void RC522_Handle(void);

void write_card_a(void);
void write_card_b(void);
void write_card_c(void);

void read_card(void);



#endif /* __RC522_FUNCTION_H */

#include "stm32f10x.h"
#include "FreeRTOS.h" //FreeRTOS  使用其API时必须先调用这个库文件
#include "task.h"     //FreeRTOS  任务头文件
#include "delay.h"
#include "usart1.h"
#include "semphr.h"
// C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
////////////////////////////////////////////////////////
#include "key.h"
#include "led.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include "bsp_sg90.h"
#include "ds1302.h"
#include "tcrt5000.h"
#include "bsp_beep.h"
#include "oled.h"
#include "u8g2_init.h"
#include "u8g2.h"

////////////////////////////////////////////////////////
#include "onenet.h"
#include "4g.h"
////////////////////////////////////////////////////////

extern _calendar_obj calendar;
extern u8 write[];
extern u8  read[];
u8 gewei[7], shiwei[7];
char* A_Area_flag = "0";
char* B_Area_flag = "0";
char* C_Area_flag = "0";
char  LED_1_flag  =  0;
char  LED_2_flag  =  0;
char  LED_3_flag  =  0;
char  beep_flag   =  0;
SemaphoreHandle_t xSemaphore;
///////////////////////////////////////////////////////////////////////////////////////////////
#define servo_1(i)     i ? TIM_SetCompare1(TIM2, 190) : TIM_SetCompare1(TIM2, 180)
#define servo_2(i)     i ? TIM_SetCompare2(TIM2, 190) : TIM_SetCompare2(TIM2, 175)
#define servo_3(i)     i ? TIM_SetCompare3(TIM3, 190) : TIM_SetCompare3(TIM3, 180)
#define servo_4(i)     i ? TIM_SetCompare4(TIM3, 190) : TIM_SetCompare4(TIM3, 180)
///////////////////////////////////////////////////////////////////////////////////////////////
#define DEBUG 1
char PUB_BUF[256]; // 上传数据的buf
// 全局结构体
u8g2_t u8g2;
typedef struct MY_DATA{
    char *rfid_id;
    char Intime[24];
    char Outtime[24];
    char Area[2];
} my_data_t;
my_data_t data;
// 菜单节点结构体
typedef struct Menu_node{
    char label[20]; // 标签
    int menu_num;
    int num_options; // 子菜单选项数量
    void (*function)(u8g2_t *, struct Menu_node *);
    struct Menu_node **options; // 子菜单选项
} menu_node_t;

// 创建菜单节点
menu_node_t *create_menu(char *label, int menu_num, int num_options,
                         void *function, menu_node_t **options)
{
    menu_node_t *menu = (menu_node_t *)malloc(sizeof(menu_node_t));
    sprintf(menu->label, "%s", label);
    menu->num_options = num_options;
    menu->menu_num = menu_num;
    menu->function = function;
    menu->options = options;
    return menu;
}
extern enum {
    main_menu = 0,
    select_menu,
    set_write_menu,
    set_read_menu,
    set_sorting_menu,
} Menu_NUM;
void void_function(){}
//=============================================================================================================
void writeArea_fun(u8g2_t *u8g2, char numArea){
    u8g2_ClearBuffer(u8g2);
    u8g2_DrawStr(u8g2, 8, 16, "ID Name: ");//Phone
    char *str=data.rfid_id;
    u8g2_DrawStr(u8g2, 8, 32, str);
    if (numArea == 'A'){
        u8g2_DrawStr(u8g2, 8, 64, "Area:     A");
    } else if (numArea == 'B'){
        u8g2_DrawStr(u8g2, 8, 64, "Area:     B");
    }u8g2_SendBuffer(u8g2);
}
void LED_1_fun(void){
	if(LED_1_flag == 0){
		LED1_OFF();
	}else if(LED_1_flag == 1){
		LED1_ON();
	}	
}
void LED_2_fun(void){
	if(LED_2_flag == 0){
		LED2_OFF();
	}else if(LED_2_flag == 1){
		LED2_ON();
	}	
}
void LED_3_fun(void){
	if(LED_3_flag == 0){
		LED3_OFF();
	}else if(LED_3_flag == 1){
		LED3_ON();
	}	
}
void BEEP_fun(void){
	BEEP_STATU(1);
	delay_ms(250);
	BEEP_STATU(0);
}
void mission_1(u8g2_t *u8g2){    
	data.rfid_id = ReaderCard(); // 读  ID卡号
    if (strcmp(data.rfid_id, "76:FE:D3:30") == 0){
        u8g2_ClearBuffer(u8g2);
        u8g2_DrawStr(u8g2, 8, 16, "Read OK");
        u8g2_SendBuffer(u8g2);
		if (!KEY2){
            writeArea_fun(u8g2,'A');
			A_Area_flag = "A"; // A 区
		}if (!KEY3){
            writeArea_fun(u8g2, 'B');
			A_Area_flag = "B"; // B 区
        }    
    }if (strcmp(data.rfid_id, "C6:B7:CD:30") == 0){
        u8g2_ClearBuffer(u8g2);
        u8g2_DrawStr(u8g2, 8, 16, "Read OK");
        u8g2_SendBuffer(u8g2);
		if (!KEY2){
            writeArea_fun(u8g2,'A');
			B_Area_flag = "A"; // A 区
		}if (!KEY3){
            writeArea_fun(u8g2, 'B');
			B_Area_flag = "B"; // B 区
        } 
    }if (strcmp(data.rfid_id, "66:32:D5:30") == 0){
        u8g2_ClearBuffer(u8g2);
        u8g2_DrawStr(u8g2, 8, 16, "Read OK");
        u8g2_SendBuffer(u8g2);
		if (!KEY2){
            writeArea_fun(u8g2,'A');
			C_Area_flag = "A"; // A 区
		}if (!KEY3){
            writeArea_fun(u8g2, 'B');
			C_Area_flag = "B"; // B 区
        } 
    }memset(data.rfid_id, 0, sizeof((char *)data.rfid_id));	
}
void mission_2(u8g2_t *u8g2){
     data.rfid_id = ReaderCard(); // 读卡 ID卡号
    if (strcmp(data.rfid_id, "76:FE:D3:30") == 0){
        u8g2_ClearBuffer(u8g2);
        u8g2_DrawStr(u8g2, 8, 16, "start write");
        u8g2_SendBuffer(u8g2);
        vTaskDelay(120);
        if (KEY2){
            u8g2_ClearBuffer(u8g2);
            u8g2_DrawStr(u8g2, 8, 16, "write OK");
            u8g2_DrawStr(u8g2, 8, 32, "76:FE:D3:30");
            if (strcmp(A_Area_flag, "A")){
                u8g2_DrawStr(u8g2, 8, 64, "Area:     B");}
			else if (strcmp(A_Area_flag, "B")){
                u8g2_DrawStr(u8g2, 8, 64, "Area:     A");
            } else{
                u8g2_DrawStr(u8g2, 8, 64, "errer");
            }u8g2_SendBuffer(u8g2);
        }
    }if (strcmp(data.rfid_id, "C6:B7:CD:30") == 0){
        u8g2_ClearBuffer(u8g2);
        u8g2_DrawStr(u8g2, 8, 16, "start write");
        u8g2_SendBuffer(u8g2);
        vTaskDelay(120);
        if (KEY3){
            u8g2_ClearBuffer(u8g2);
            u8g2_DrawStr(u8g2, 8, 16, "write OK");
            u8g2_DrawStr(u8g2, 8, 32, "C6:B7:CD:30");
            if (strcmp(B_Area_flag, "A")){
                u8g2_DrawStr(u8g2, 8, 64, "Area:     B");
            } else if (strcmp(B_Area_flag, "B")){
                u8g2_DrawStr(u8g2, 8, 64, "Area:     A");
            } else{
                u8g2_DrawStr(u8g2, 8, 64, "errer");
            }u8g2_SendBuffer(u8g2);
        }
    }if (strcmp(data.rfid_id, "66:32:D5:30") == 0){
        u8g2_ClearBuffer(u8g2);
        u8g2_DrawStr(u8g2, 8, 16, "start write");
        u8g2_SendBuffer(u8g2);
        vTaskDelay(120);
        if (KEY2){
            u8g2_ClearBuffer(u8g2);
            u8g2_DrawStr(u8g2, 8, 16, "write OK");
            u8g2_DrawStr(u8g2, 8, 32, "66:32:D5:30");
            if (strcmp(C_Area_flag, "A")){
                u8g2_DrawStr(u8g2, 8, 64, "Area:     B");
            } else if (strcmp(C_Area_flag, "B")){
                u8g2_DrawStr(u8g2, 8, 64, "Area:     A");
            } else{
                u8g2_DrawStr(u8g2, 8, 64, "errer");
            }u8g2_SendBuffer(u8g2);
        }
    }memset(data.rfid_id, 0, sizeof((char *)data.rfid_id));
}

void mission_3(u8g2_t *u8g2){ 
#if DEBUG	
		char buf[64] = {0};
		char ret_buf[128] = {0};
		OLED_ShowString(24, 2, (u8 *)"Reading...", 16);//EADING...
		data.rfid_id = ReaderCard(); // 读卡
		if (strcmp(data.rfid_id, "76:FE:D3:30") == 0){
			LED_1_flag = 0;
			OLED_Clear();
			OLED_ShowString(24, 2, (u8 *)"Card A", 16);
			LED_1_fun();
			// 发送 入库时间
			sprintf(data.Intime, "%d-%02d-%02d %02d:%02d:%02d", calendar.w_year,
					calendar.w_month,
					calendar.w_date,
					calendar.hour,
					calendar.min,
					calendar.sec);					
			// 1号 舵机 转动 
			servo_1(1);
			OLED_Clear();
			OLED_ShowString(16, 2, (u8 *)"Transforming...", 16);
			if((strcmp(A_Area_flag, "A"))){
					while (tcrt5000_1());
			}else if((strcmp(A_Area_flag, "B"))){
					while (tcrt5000_2());
			}						
			OLED_Clear();
			OLED_ShowString(24, 2, (u8 *)"A_Card Out", 16);
			sprintf(data.Outtime, "%d-%02d-%02d %02d:%02d:%02d", calendar.w_year,
					calendar.w_month,
					calendar.w_date,
					calendar.hour,
					calendar.min,
					calendar.sec);
			if((strcmp(A_Area_flag, "B"))){
				servo_2(1);
				delay_s(1);
				servo_2(0);
			}else if((strcmp(A_Area_flag, "A"))){
				servo_3(1);
				delay_s(1);
				servo_3(0);
			}			
			sprintf(buf, "%s,%s,%s", data.rfid_id, data.Intime, data.Outtime);

			servo_1(0);
			LED_1_flag = 1;
			BEEP_fun();
			sprintf(ret_buf, "AT+MPUB=/myrfid/pub,0,0,\"\\7B\\22"
							 "ID\\22:\\22%s\\22,\\22"
							 "In_Time\\22:\\22%s\\22,\\22"
							 "Out_Time\\22:\\22%s\\22,\\22"
							 "Out_Area\\22:\\22%s\\22\\7D\"\r\n",
					data.rfid_id,
					data.Intime,
					data.Outtime,
					A_Area_flag);					
					LED_1_fun();					
				printf("\r\nbuf---%s\r\n", ret_buf);
			if (AIR724_SendCmd(ret_buf, "success")){
				memset(ret_buf, 0, sizeof(ret_buf));
			}   memset(data.rfid_id, 0, sizeof((char *)data.rfid_id));AIR724_Clear();
		}
		
		
		else if (strcmp(data.rfid_id, "C6:B7:CD:30") == 0){
            LED_2_flag = 0;
			OLED_Clear();
			OLED_ShowString(24, 2, (u8 *)"Card B", 16);
            LED_2_fun();
			// 发送 入库时间
			sprintf(data.Intime, "%d-%02d-%02d %02d:%02d:%02d", calendar.w_year,
					calendar.w_month,
					calendar.w_date,
					calendar.hour,
					calendar.min,
					calendar.sec);
			// 1号 舵机 转动 
			servo_1(1);
			OLED_Clear();
			OLED_ShowString(16, 2, (u8 *)"Transforming...", 16);
			if((strcmp(B_Area_flag, "A"))){
					while (tcrt5000_1());
			}else if((strcmp(B_Area_flag, "B"))){
					while (tcrt5000_2());
			}
			OLED_Clear();
			OLED_ShowString(24, 2, (u8 *)"B_Card Out", 16);
			sprintf(data.Outtime, "%d-%02d-%02d %02d:%02d:%02d", calendar.w_year,
					calendar.w_month,
					calendar.w_date,
					calendar.hour,
					calendar.min,
					calendar.sec);
			if((strcmp(B_Area_flag, "B"))){
				servo_2(1);
				delay_s(1);
				servo_2(0);
			}else if((strcmp(B_Area_flag, "A"))){
				servo_3(1);
				delay_s(1);
				servo_3(0);
			}			
			sprintf(buf, "%s,%s,%s", data.rfid_id, data.Intime, data.Outtime);
			servo_1(0);
            LED_2_flag = 1;
			BEEP_fun();
			sprintf(ret_buf, "AT+MPUB=/myrfid/pub,0,0,\"\\7B\\22"
							 "ID\\22:\\22%s\\22,\\22"
							 "In_Time\\22:\\22%s\\22,\\22"
							 "Out_Time\\22:\\22%s\\22,\\22"
							 "Out_Area\\22:\\22%s\\22\\7D\"\r\n",
					data.rfid_id,
					data.Intime,
					data.Outtime,
					B_Area_flag);					
                    LED_2_fun();					
				printf("\r\nbuf---%s\r\n", ret_buf);
			if (AIR724_SendCmd(ret_buf, "success")){
				memset(ret_buf, 0, sizeof(ret_buf));
			}   memset(data.rfid_id, 0, sizeof((char *)data.rfid_id));AIR724_Clear();
		}
		
		
		else if (strcmp(data.rfid_id, "66:32:D5:30") == 0){
            LED_3_flag = 0;
			OLED_Clear();
			OLED_ShowString(24, 2, (u8 *)"Card C", 16);
            LED_3_fun();
			// 发送 入库时间
			sprintf(data.Intime, "%d-%02d-%02d %02d:%02d:%02d", calendar.w_year,
					calendar.w_month,
					calendar.w_date,
					calendar.hour,
					calendar.min,
					calendar.sec);
			// 1号 舵机 转动 
			servo_1(1);
			OLED_Clear();
			OLED_ShowString(16, 2, (u8 *)"Transforming...", 16);
			if((strcmp(C_Area_flag, "A"))){
					while (tcrt5000_1());
			}else if((strcmp(C_Area_flag, "B"))){
					while (tcrt5000_2());
			}
			OLED_Clear();
			OLED_ShowString(24, 2, (u8 *)"C_Card Out", 16);
			sprintf(data.Outtime, "%d-%02d-%02d %02d:%02d:%02d", calendar.w_year,
					calendar.w_month,
					calendar.w_date,
					calendar.hour,
					calendar.min,
					calendar.sec);
			if((strcmp(C_Area_flag, "B"))){
				servo_2(1);
				delay_s(1);
				servo_2(0);
			}else if((strcmp(C_Area_flag, "A"))){
				servo_3(1);
				delay_s(1);
				servo_3(0);
			}			
			sprintf(buf, "%s,%s,%s", data.rfid_id, data.Intime, data.Outtime);
			servo_1(0);
            LED_3_flag = 1;
			BEEP_fun();
			sprintf(ret_buf, "AT+MPUB=/myrfid/pub,0,0,\"\\7B\\22"
							 "ID\\22:\\22%s\\22,\\22"
							 "In_Time\\22:\\22%s\\22,\\22"
							 "Out_Time\\22:\\22%s\\22,\\22"
							 "Out_Area\\22:\\22%s\\22\\7D\"\r\n",
					data.rfid_id,
					data.Intime,
					data.Outtime,
					C_Area_flag);
                    LED_3_fun();
				printf("\r\nbuf---%s\r\n", ret_buf);
			if (AIR724_SendCmd(ret_buf, "success")){
				memset(ret_buf, 0, sizeof(ret_buf));
			}   memset(data.rfid_id, 0, sizeof((char *)data.rfid_id));
			AIR724_Clear();
		}AIR724_Clear();
#endif
}



void wirte_function(u8g2_t *u8g2, menu_node_t *menu){
    u8g2_ClearBuffer(u8g2);
    int wirte_flag = 1;
    while (1){
        if (wirte_flag == 0){
            mission_1(u8g2);
        }
        if (!KEY1){
            u8g2_ClearBuffer(u8g2);
            return;
        }if (!KEY2){}
        if (!KEY3){
            wirte_flag = 0;
        }u8g2_SendBuffer(u8g2);
        vTaskDelay(10);
    }
}

void read_function(u8g2_t *u8g2, menu_node_t *menu){
    u8g2_ClearBuffer(u8g2);
    int read_flag = 1;
    while (1){
        if (read_flag == 0){
            mission_2(u8g2);
        }if (!KEY1){
            u8g2_ClearBuffer(u8g2);
            return;
        }if (!KEY2){}
        if (!KEY3){
            read_flag = 0;
        }u8g2_SendBuffer(u8g2);
        vTaskDelay(10);
    }
}

void sorting_function(u8g2_t *u8g2, menu_node_t *menu){
    u8g2_ClearBuffer(u8g2);
    int sorting_flag = 1;
    while (1){
        if (sorting_flag == 0){
            mission_3(u8g2);
        }if (!KEY1){
            u8g2_ClearBuffer(u8g2);
            return;
        }if (!KEY3){
            sorting_flag = 0;
        }if (!KEY2){}
        u8g2_SendBuffer(u8g2);
        vTaskDelay(10);
    }
}
//===================================================================================================================================

void display_menu(u8g2_t *u8g2, menu_node_t *menu){
    int i = 0, choice = 1;
    char showStr[50];
    u8g2_ClearBuffer(u8g2);
    while (1){
        for (i = 0; i < menu->num_options; i++){
            sprintf(showStr, "%d.%s", i + 1, menu->options[i]->label);
            u8g2_DrawStr(u8g2, 12, 12 + (i * 12), showStr);
        }
        u8g2_DrawStr(u8g2, 12, (menu->num_options + 1) * 12, "Back");
        u8g2_DrawStr(u8g2, 0, choice * 12, "->");
        u8g2_SendBuffer(u8g2);
        if (!KEY1){//KEY1    read_key1()
            u8g2_ClearBuffer(u8g2);
            choice++;
            if (choice > menu->num_options + 1){
                choice = 1;
            }
        }if (!KEY2){//!KEY2    read_key2()
            u8g2_ClearBuffer(u8g2);
            choice--;
            if (choice < 1){
                choice = menu->num_options + 1;
            }
        }if (!KEY3){//!KEY3    read_key3()
            u8g2_ClearBuffer(u8g2);
            if (choice > 0 && choice <= menu->num_options){
                if (menu->options[choice - 1]->num_options > 0){
                    display_menu(u8g2, menu->options[choice - 1]);
                }else{
                    menu->options[choice - 1]->function(
                        u8g2, menu->options[choice - 1]);
                }
            }else if (choice == i + 1){
                choice = 1;
                break;
            }
        }vTaskDelay(10);
    }
}

// 任务1
// pvParameters:任务函数所需要的参数，与主函数中的对应
// 注：pvParameters需要进行强制转换才可以使用
void vTaskCode(void *pvParameters){
    menu_node_t *root = create_menu(
        "Root", select_menu, 3, void_function,
        (menu_node_t *[]){
            create_menu("Write Card", set_write_menu,   0, wirte_function,
                        NULL),
            create_menu("Read  Card", set_read_menu,    0, read_function,
                        NULL),
            create_menu("Start Sort", set_sorting_menu, 0, sorting_function,
                        NULL)}
								   );                      
    while(1){
        display_menu(&u8g2, root);
		vTaskDelay(500);			   //延时500ms 使该任务进入阻塞态
    }
}

//任务2
//pvParameters:任务函数所需要的参数，与主函数中的对应
//注：pvParameters需要进行强制转换才可以使用
void vTaskCode2( void * pvParameters ){
    while(1){
	if (U2_RX_Cmd == 1){ // 接收到数据
		U2_RX_Cmd = 0; // 重新置为0
			 if (strstr((const char *)U2_RX_BUF, (const char *)"\"target\":\"Servo1\",\"value\":1")){servo_1(1);}
		else if (strstr((const char *)U2_RX_BUF, (const char *)"\"target\":\"Servo1\",\"value\":0")){servo_1(0);}
		else if (strstr((const char *)U2_RX_BUF, (const char *)"\"target\":\"Servo2\",\"value\":1")){servo_2(1);}
		else if (strstr((const char *)U2_RX_BUF, (const char *)"\"target\":\"Servo2\",\"value\":0")){servo_2(0);}
		else if (strstr((const char *)U2_RX_BUF, (const char *)"\"target\":\"Servo3\",\"value\":1")){servo_3(1);}
		else if (strstr((const char *)U2_RX_BUF, (const char *)"\"target\":\"Servo3\",\"value\":0")){servo_3(0);}
		else if (strstr((const char *)U2_RX_BUF, (const char *)"\"target\":\"BEEP\",\"value\":1")){BEEP_STATU(1);}
		else if (strstr((const char *)U2_RX_BUF, (const char *)"\"target\":\"BEEP\",\"value\":0")){BEEP_STATU(0);}
			memset(U2_RX_BUF, 0, sizeof(U2_RX_BUF)); // 清空串口2缓冲区
		}vTaskDelay(1000);			   //延时1000ms 使该任务进入阻塞态
    }
}



int main(void){
	TaskHandle_t xHandle = NULL;				//存储任务句柄，方便给别的任务使用或者删除
	TaskHandle_t xHandle2 = NULL;				//存储任务句柄，方便给别的任务使用或者删除

	//注:在进行任务创建时，需进行硬件的初始化
    Delay_Init();
	RTC_Init();
	USART1_Init(115200);							// 串口1初始化，打印debug串口
	uart2_init(115200);								// 串口2初始化，stm32-4G通信串口
	OLED_Init();
    IIC_Init();      // u8g2 iic初始化
    u8g2Init(&u8g2); // u8g2初始化
	beep_init();
	KEY_Init();
	LED_Init();
	IIC_Init();						 // u8g2 iic初始化
	AIR724_Init();					 // 初始化AIR724
	RC522_Init();					 // RC522模块所需外设的初始化配置
	SG90_1_TIM2_pwm_init(199, 7199); // 舵机初始化
	tcrt5000_1_init();
	tcrt5000_2_init();
	tcrt5000_3_init();
	tcrt5000_4_init();
    u8g2_SetFontMode(&u8g2, 1);
    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);  //u8g2_font_unifont_t_symbols   u8g2_font_6x13_mf
    /*创建一个新任务*/
    BaseType_t xReturned = xTaskCreate(
                    vTaskCode,       //函数指针，指向需要执行的任务函数
                    "vTaskCode",     //当前任务的名称
                    512*3,      	 //栈内存大小，以字为单位，32位下-> 1word = 4byte 当前大小为50*4=200byte
                    NULL,    	     //任务函数需要使用的参数 不传参数为NULL
                    0,				 //任务的优先级 数字越小，任务优先级越低
                    &xHandle );      //该任务的句柄，用于提供给其他任务使用或者删除
	/*创建一个新任务*/
    BaseType_t xReturned2 = xTaskCreate(
                    vTaskCode2,      //函数指针，指向需要执行的任务函数
                    "vTaskCode2",    //当前任务的名称
                    128,      		 //栈内存大小，以字为单位，32位下-> 1word = 4byte 当前大小为50*4=200byte
                    NULL,    	     //任务函数需要使用的参数 不传参数为NULL
                    1,				 //任务的优先级 数字越小，任务优先级越低
                    &xHandle2 );     //该任务的句柄，用于提供给其他任务使用或者删除

	//错误判断
    if(xReturned == pdPASS && xReturned == pdPASS){
//		printf("The task was created successfully\r\n");  //创建任务成功
		//开启任务调度器，程序进入到该函数并且不会退出,即进入死循环
		vTaskStartScheduler();		
    }if(xReturned != pdPASS){
//		printf("Task 1 failed to create\r\n");//任务1创建失败
	}else if(xReturned2 != pdPASS){
//		printf("Task 2 failed to create\r\n");//任务2创建失败
	}//开启任务调度器，程序进入到该函数并且不会退出,即进入死循环
	//vTaskStartScheduler();
}
























//	BaseType_t xReturned3 = xTaskCreate(
//                    vTaskCode3,      //函数指针，指向需要执行的任务函数
//                    "vTaskCode3",    //当前任务的名称
//                    64*8,      		 //栈内存大小，以字为单位，32位下-> 1word = 4byte 当前大小为50*4=200byte
//                    NULL,    	     //任务函数需要使用的参数 不传参数为NULL
//                    0,				 //任务的优先级 数字越小，任务优先级越低
//                    &xHandle3 );     //该任务的句柄，用于提供给其他任务使用或者删除






/*               2023-05-02
u8g2_t u8g2;
typedef struct Menu_node{
    char label[20];
    int menu_num;
    int num_options;
    void (*function)(u8g2_t *, struct Menu_node *);
    struct Menu_node **options; 
} menu_node_t;
menu_node_t *create_menu(char *label, int menu_num, int num_options,
                         void *function, menu_node_t **options){
    menu_node_t *menu = (menu_node_t *)malloc(sizeof(menu_node_t));
    sprintf(menu->label, "%s", label);
    menu->num_options = num_options;
    menu->menu_num = menu_num;
    menu->function = function;
    menu->options = options;
    return menu;
}
void display_menu(u8g2_t *u8g2, menu_node_t *menu){
    int i = 0, choice = 1;
    char showStr[50];
    u8g2_ClearBuffer(u8g2);
    while (1){for (i = 0; i < menu->num_options; i++){
            sprintf(showStr, "%d.%s", i + 1, menu->options[i]->label);
            u8g2_DrawStr(u8g2, 12, 12 + (i * 12), showStr);
        }u8g2_DrawStr(u8g2, 12, (menu->num_options + 1) * 12, "Back");
        u8g2_DrawStr(u8g2, 0, choice * 12, "->");
        u8g2_SendBuffer(u8g2);
        if (!KEY1){
            u8g2_ClearBuffer(u8g2);
            choice++;
            if (choice > menu->num_options + 1){choice = 1;}
        }if (!KEY2){
            u8g2_ClearBuffer(u8g2);
            choice--;
            if (choice < 1){
                choice = menu->num_options + 1;}
        }if (!KEY3){
            u8g2_ClearBuffer(u8g2);
            if (choice > 0 && choice <= menu->num_options){
                if (menu->options[choice - 1]->num_options > 0){
                    display_menu(u8g2, menu->options[choice - 1]);
                }
                else{menu->options[choice - 1]->function(
                        u8g2, menu->options[choice - 1]);}
            } else if (choice == i + 1){
                choice = 1;
            break;}
        }vTaskDelay(10);
    }
}
extern enum {
    main_menu = 0,
    select_menu,
    set_write_menu,
    set_read_menu,
    set_sorting_menu,
} Menu_NUM;
*/



















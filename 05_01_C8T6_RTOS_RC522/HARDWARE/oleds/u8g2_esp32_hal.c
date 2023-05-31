#include "u8g2_esp32_hal.h"
#include "bsp_key.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "u8g2.h"

const static char *TAG = "u8g2_hal";

u8g2_t u8g2; // a structure which will contain all the data for one display
static u8g2_hal_i2c_cfg_t i2c_cfg =
    U8G2_HAL_I2C_CFG_DEFAULT();     // I2C 引脚配置信息
static i2c_cmd_handle_t handle_i2c; // I2C cmd handle

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern enum {
    main_menu = 0,
    select_menu,
    set_max_menu,
    set_min_menu,
    set_motor_menu,
    set_auto_menu,
    set_t_max_menu,
    set_t_min_menu,
    set_h_max_menu,
    set_h_min_menu,
    set_c_max_menu,
    set_c_min_menu,
} Menu_NUM;

// 创建菜单节点
menu_node_t *create_menu(char *label, int menu_num, int num_options,
                         void *function, menu_node_t **options) {
    menu_node_t *menu = (menu_node_t *)malloc(sizeof(menu_node_t));
    sprintf(menu->label, "%s", label);
    menu->num_options = num_options;
    menu->menu_num = menu_num;
    menu->function = function;
    menu->options = options;
    return menu;
}

void set_sensor_value_function(u8g2_t *u8g2, menu_node_t *menu,
                               sensor_data_t *sensor) {
    char valueStr[10];
    char flagStr[10];
    char titleStr[15];
    int *showValue = NULL;
    u8g2_ClearBuffer(u8g2);
    sensor->menu_index = menu->menu_num;

    switch (sensor->menu_index) {
    case set_t_max_menu:
        strcpy(flagStr, "MAX");
        strcpy(titleStr, "TEMPERATURE");
        showValue = &(sensor->data_val->t_max);
        break;
    case set_t_min_menu:
        strcpy(flagStr, "MIN");
        strcpy(titleStr, "TEMPERATURE");
        showValue = &(sensor->data_val->t_min);
        break;
    case set_h_max_menu:
        strcpy(flagStr, "MAX");
        strcpy(titleStr, "HUMIDITY");
        showValue = &(sensor->data_val->h_max);
        break;
    case set_h_min_menu:
        strcpy(flagStr, "MIN");
        strcpy(titleStr, "HUMIDITY");
        showValue = &(sensor->data_val->h_min);
        break;
    case set_c_max_menu:
        strcpy(flagStr, "MAX");
        strcpy(titleStr, "CO");
        showValue = &(sensor->data_val->c_max);
        break;
    case set_c_min_menu:
        strcpy(flagStr, "MIN");
        strcpy(titleStr, "CO");
        showValue = &(sensor->data_val->c_min);
        break;

    default:
        break;
    }
    while (1) {
        u8g2_DrawStr(u8g2, 0, 12, titleStr);
        u8g2_DrawStr(u8g2, 128 - 24, 12, flagStr);
        u8g2_SetFont(u8g2, u8g2_font_pxplusibmvga8_t_all);
        sprintf(valueStr, "%d", *showValue);
        u8g2_DrawStr(u8g2, 58, 32, valueStr);
        u8g2_SetFont(u8g2, u8g2_font_wqy12_t_myfont);
        u8g2_DrawStr(u8g2, 0, 64, "-");
        u8g2_DrawStr(u8g2, 128 - 10, 64, "+");
        u8g2_SendBuffer(u8g2);
        if (read_key3()) {
            u8g2_ClearBuffer(u8g2);
            return;
        }
        if (read_key1()) {
            (*showValue)--;
            u8g2_ClearBuffer(u8g2);
        }
        if (read_key2()) {
            (*showValue)++;
            u8g2_ClearBuffer(u8g2);
        }
        vTaskDelay(10);
    }
}

void void_function(void) {}

void motor_control_function(u8g2_t *u8g2, menu_node_t *menu,
                            sensor_data_t *sensor) {
    char flagStr[10];
    char titleStr[15];
    u8g2_ClearBuffer(u8g2);
    sensor->menu_index = menu->menu_num;
    strcpy(flagStr, "CONTROL");
    strcpy(titleStr, "MOTOR");

    u8g2_SetFont(u8g2, u8g2_font_pxplusibmvga8_t_all);
    if (sensor->windowsFlag) {
        u8g2_DrawStr(u8g2, 58, 32, "ON");
    } else {
        u8g2_DrawStr(u8g2, 54, 32, "OFF");
    }
    u8g2_SetFont(u8g2, u8g2_font_wqy12_t_myfont);

    while (1) {
        u8g2_DrawStr(u8g2, 0, 12, titleStr);
        u8g2_DrawStr(u8g2, 128 - 52, 12, flagStr);
        u8g2_DrawStr(u8g2, 0, 64, "-");
        u8g2_DrawStr(u8g2, 128 - 10, 64, "+");
        if (read_key3()) {
            u8g2_ClearBuffer(u8g2);
            return;
        }
        if (read_key1()) {
            u8g2_ClearBuffer(u8g2);
            u8g2_SetFont(u8g2, u8g2_font_pxplusibmvga8_t_all);
            if (sensor->auto_flag == AUTO_MODE) {
                u8g2_DrawStr(u8g2, 44, 32, "ERROR");
            } else {
                u8g2_DrawStr(u8g2, 54, 32, "OFF");
                servo_off();
                sensor->windowsFlag = 0;
            }

            u8g2_SetFont(u8g2, u8g2_font_wqy12_t_myfont);
        }
        if (read_key2()) {
            u8g2_ClearBuffer(u8g2);
            u8g2_SetFont(u8g2, u8g2_font_pxplusibmvga8_t_all);
            if (sensor->auto_flag == AUTO_MODE) {
                u8g2_DrawStr(u8g2, 44, 32, "ERROR");
            } else {
                servo_on();
                sensor->windowsFlag = 1;
                u8g2_DrawStr(u8g2, 58, 32, "ON");
            }
            u8g2_SetFont(u8g2, u8g2_font_wqy12_t_myfont);
        }
        u8g2_SendBuffer(u8g2);
        vTaskDelay(10);
    }
}

void auto_control_function(u8g2_t *u8g2, menu_node_t *menu,
                           sensor_data_t *sensor) {
    char flagStr[10];
    char titleStr[15];
    u8g2_ClearBuffer(u8g2);
    sensor->menu_index = menu->menu_num;
    strcpy(flagStr, "CONTROL");
    strcpy(titleStr, "AUTO");

    u8g2_SetFont(u8g2, u8g2_font_pxplusibmvga8_t_all);
    if (sensor->auto_flag == 0) {
        u8g2_DrawStr(u8g2, 50, 32, "AUTO");
    } else {
        u8g2_DrawStr(u8g2, 42, 32, "MANUAL");
    }
    u8g2_SetFont(u8g2, u8g2_font_wqy12_t_myfont);

    while (1) {
        u8g2_DrawStr(u8g2, 0, 12, titleStr);
        u8g2_DrawStr(u8g2, 128 - 52, 12, flagStr);
        u8g2_DrawStr(u8g2, 0, 64, "-");
        u8g2_DrawStr(u8g2, 128 - 10, 64, "+");
        if (read_key3()) {
            u8g2_ClearBuffer(u8g2);
            return;
        }
        if (read_key1()) {
            u8g2_ClearBuffer(u8g2);
            sensor->auto_flag = AUTO_MODE; // 自动控制
            u8g2_SetFont(u8g2, u8g2_font_pxplusibmvga8_t_all);
            u8g2_DrawStr(u8g2, 50, 32, "AUTO");
            u8g2_SetFont(u8g2, u8g2_font_wqy12_t_myfont);
        }
        if (read_key2()) {
            u8g2_ClearBuffer(u8g2);
            sensor->auto_flag = MANUAL_MODE; // 手动控制
            u8g2_SetFont(u8g2, u8g2_font_pxplusibmvga8_t_all);
            u8g2_DrawStr(u8g2, 42, 32, "MANUAL");
            u8g2_SetFont(u8g2, u8g2_font_wqy12_t_myfont);
        }
        u8g2_SendBuffer(u8g2);
        vTaskDelay(10);
    }
}
// 负责调用 I2C 发送数据
static uint8_t u8g2_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                                void *arg_ptr) {
    ESP_LOGD(TAG, "i2c_byte_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p",
             msg, arg_int, arg_ptr);

    switch (msg) {
    case U8X8_MSG_BYTE_SET_DC:
        break;

    case U8X8_MSG_BYTE_INIT: {
        if (i2c_cfg.sda == GPIO_NUM_UNDEFINED ||
            i2c_cfg.scl == GPIO_NUM_UNDEFINED) {
            /**
             * 没有提供 i2c 参数
             * 需要先调用 u8g2_hal_i2c_init(i2c_cfg) 以配置 i2c 参数
             */
            ESP_LOGE(TAG, "No i2c configuration provided.");
            break;
        }

        i2c_config_t conf = {0};
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = i2c_cfg.sda;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_io_num = i2c_cfg.scl;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = I2C_FREQ_HZ;
        ESP_LOGI(TAG, "sda_io_num %d", conf.sda_io_num);
        ESP_LOGI(TAG, "scl_io_num %d", conf.scl_io_num);
        ESP_LOGI(TAG, "clk_speed %ld", conf.master.clk_speed);
        ESP_ERROR_CHECK(i2c_param_config(I2C_NUM, &conf));
        ESP_LOGI(TAG, "i2c_driver_install %d", I2C_NUM);
        ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM, conf.mode, 0, 0, 0));
        break;
    }

    case U8X8_MSG_BYTE_SEND: {
        // arg_ptr 是要发送的数据（uint8_t[]）
        // arg_int 是数据的长度
        uint8_t *data_ptr = (uint8_t *)arg_ptr;
        while (arg_int > 0) {
            ESP_ERROR_CHECK(i2c_master_write_byte(handle_i2c, *data_ptr, 1));
            data_ptr++;
            arg_int--;
        }
        break;
    }

    case U8X8_MSG_BYTE_START_TRANSFER: {
        uint8_t i2c_address = u8x8_GetI2CAddress(u8x8);
        handle_i2c = i2c_cmd_link_create();
        ESP_LOGD(TAG, "Start I2C transfer to %02X.", i2c_address >> 1);
        ESP_ERROR_CHECK(i2c_master_start(handle_i2c));
        ESP_ERROR_CHECK(i2c_master_write_byte(
            handle_i2c, i2c_address | I2C_MASTER_WRITE, 1));
        break;
    }

    case U8X8_MSG_BYTE_END_TRANSFER: {
        ESP_LOGD(TAG, "End I2C transfer.");
        ESP_ERROR_CHECK(i2c_master_stop(handle_i2c));
        ESP_ERROR_CHECK(i2c_master_cmd_begin(
            I2C_NUM, handle_i2c, I2C_TIMEOUT_MS / portTICK_PERIOD_MS));
        i2c_cmd_link_delete(handle_i2c);
        break;
    }
    }
    return 0;
}

u8g2_t *u8g2_hal_i2c_init(u8g2_hal_i2c_cfg_t cfg) {
    i2c_cfg = cfg;

    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &u8g2, U8G2_R0, u8g2_i2c_byte_cb,
        u8x8_dummy_cb // 需要软件操纵 gpio
                      // 模拟通信协议才需要实现这个函数，而我这里用硬件的
    );

    u8g2_SetI2CAddress(&u8g2, 0x78); // 设置屏幕 I2C 地址
    u8g2_InitDisplay(
        &u8g2); // 发送初始化命令序列给屏幕，执行完毕后屏幕会进入睡眠模式
    u8g2_SetPowerSave(&u8g2, 0); // 唤醒屏幕
    u8g2_ClearDisplay(&u8g2);    // 清屏

    return &u8g2;
}
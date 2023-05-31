#if !defined(__U8G2_HAL)
#define __U8G2_HAL

#include "hal/gpio_types.h"
#include "main.h"
#include "u8g2.h"

// SDA - GPIO21
#define PIN_SDA 21
// SCL - GPIO22
#define PIN_SCL 22

#define I2C_NUM I2C_NUM_1 //  指定要使用的 I2C 端口（esp32 有多个 I2C）
#define I2C_FREQ_HZ 1000000
#define I2C_TIMEOUT_MS 1000

#define GPIO_NUM_UNDEFINED (-1)

typedef struct {
    gpio_num_t sda;
    gpio_num_t scl;
} u8g2_hal_i2c_cfg_t;

#define U8G2_HAL_I2C_CFG_DEFAULT()                                             \
    { GPIO_NUM_UNDEFINED, GPIO_NUM_UNDEFINED }

extern u8g2_t u8g2;

/**
 * @brief 配置 I2C 引脚，初始化屏幕后返回 u8g2_t*
 * @param cfg I2C 配置结构体
 * @returns u8g2 实例
 */
u8g2_t *u8g2_hal_i2c_init(u8g2_hal_i2c_cfg_t cfg);
menu_node_t *create_menu(char *label, int menu_num, int num_options,
                         void *function, menu_node_t **options);
void void_function(void);
void set_sensor_value_function(u8g2_t *u8g2, menu_node_t *menu,
                               sensor_data_t *sensor);
void motor_control_function(u8g2_t *u8g2, menu_node_t *menu,
                            sensor_data_t *sensor);
void auto_control_function(u8g2_t *u8g2, menu_node_t *menu,
                           sensor_data_t *sensor);
#endif // __U8G2_HAL
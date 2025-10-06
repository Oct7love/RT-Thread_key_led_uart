    #ifndef __RT_OLED_H__
    #define __RT_OLED_H__

    #include <rtthread.h>
    #include <rtdevice.h>
    #include <drivers/i2c.h>
    #include "stm32f4xx_hal.h"

    #define OLED_ADDR 0x78

    /* OLED 设备结构体 */
    struct rt_oled_device
    {
        struct rt_i2c_bus_device *i2c_bus;  // I2C 总线设备
        rt_uint8_t addr;                     // OLED 设备地址
    };

    #define OLED_WIDTH 128
    #define OLED_HEIGHT 32

    /* 函数声明 */
    rt_err_t rt_oled_init(void);
    void rt_oled_write_cmd(rt_uint8_t cmd);
    void rt_oled_write_data(rt_uint8_t data);
    void rt_oled_show_pic(rt_uint8_t x0, rt_uint8_t y0, rt_uint8_t x1, rt_uint8_t y1, rt_uint8_t BMP[]);
    void rt_oled_show_hanzi(rt_uint8_t x, rt_uint8_t y, rt_uint8_t no);
    void rt_oled_show_hzbig(rt_uint8_t x, rt_uint8_t y, rt_uint8_t n);
    void rt_oled_show_float(rt_uint8_t x, rt_uint8_t y, float num, rt_uint8_t accuracy, rt_uint8_t fontsize);
    void rt_oled_show_num(rt_uint8_t x, rt_uint8_t y, rt_uint32_t num, rt_uint8_t length, rt_uint8_t fontsize);
    void rt_oled_show_str(rt_uint8_t x, rt_uint8_t y, char *ch, rt_uint8_t fontsize);
    void rt_oled_show_char(rt_uint8_t x, rt_uint8_t y, rt_uint8_t ch, rt_uint8_t fontsize);
    void rt_oled_allfill(void);
    void rt_oled_set_position(rt_uint8_t x, rt_uint8_t y);
    void rt_oled_clear(void);
    void rt_oled_display_on(void);
    void rt_oled_display_off(void);

    #endif  /*__RT_OLED_H__*/

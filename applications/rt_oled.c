/*
 * RT-Thread OLED (SSD1306) 驱动库
 * 移植自 STM32 HAL 库版本
 */

#include "rt_oled.h"
#include "oledfont.h"
#include "stm32f4xx_hal.h"

/* 外部 I2C 句柄，需要在 main.c 或其他地方定义 */
extern I2C_HandleTypeDef hi2c1;

/**
 * OLED 初始化控制字
 */
static rt_uint8_t initcmd1[] = {
    0xAE,       // display off
    0xD5, 0x80, // Set Display Clock Divide Ratio/Oscillator Frequency
    0xA8, 0x1F, // set multiplex Ratio
    0xD3, 0x00, // display offset
    0x40,       // set display start line
    0x8d, 0x14, // set charge pump
    0xa1,       // set segment remap
    0xc8,       // set com output scan direction
    0xda, 0x00, // set com pins hardware configuration
    0x81, 0x80, // set contrast control
    0xd9, 0x1f, // set pre-charge period
    0xdb, 0x40, // set vcom deselect level
    0xa4,       // Set Entire Display On/Off
    0xaf,       // set display on
};

/**
 * @brief OLED 写命令
 * @param cmd 命令字节
 */
void rt_oled_write_cmd(rt_uint8_t cmd)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0x100);
}

/**
 * @brief OLED 写数据
 * @param data 数据字节
 */
void rt_oled_write_data(rt_uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, &data, 1, 0x100);
}

/**
 * @brief 图像显示函数
 * @param x0  图像显示起始位置 x 轴
 * @param y0  图像显示起始位置 y 轴
 * @param x1  图像显示结束位置 x 轴 1 - 127
 * @param y1  图像显示结束位置 y 轴 1 - 4
 * @param BMP 图像显示指针地址
 */
void rt_oled_show_pic(rt_uint8_t x0, rt_uint8_t y0, rt_uint8_t x1, rt_uint8_t y1, rt_uint8_t BMP[])
{
    rt_uint16_t i = 0;
    rt_uint8_t x, y;
    for (y = y0; y < y1; y++)
    {
        rt_oled_set_position(x0, y);
        for (x = x0; x < x1; x++)
        {
            rt_oled_write_data(BMP[i++]);
        }
    }
}

/**
 * @brief 显示 16*16 像素汉字
 * @param x  位置 x 轴  0 - 127
 * @param y  位置 y 轴  0 - 3
 * @param no hzk[] 数组中汉字的顺序
 */
void rt_oled_show_hanzi(rt_uint8_t x, rt_uint8_t y, rt_uint8_t no)
{
    rt_uint8_t t, adder = 0;
    rt_oled_set_position(x, y);
    for (t = 0; t < 16; t++)
    {
        rt_oled_write_data(Hzk[2 * no][t]);
        adder += 1;
    }
    rt_oled_set_position(x, y + 1);
    for (t = 0; t < 16; t++)
    {
        rt_oled_write_data(Hzk[2 * no + 1][t]);
        adder += 1;
    }
}

/**
 * @brief 显示 32*32 像素汉字，全屏显示
 * @param x  位置 x 轴  0 - 127
 * @param y  位置 y 轴  0
 * @param n  Hzb[] 数组中汉字的顺序
 */
void rt_oled_show_hzbig(rt_uint8_t x, rt_uint8_t y, rt_uint8_t n)
{
    rt_uint8_t t, adder = 0;
    rt_oled_set_position(x, y);
    for (t = 0; t < 32; t++)
    {
        rt_oled_write_data(Hzb[4 * n][t]);
        adder += 1;
    }
    rt_oled_set_position(x, y + 1);
    for (t = 0; t < 32; t++)
    {
        rt_oled_write_data(Hzb[4 * n + 1][t]);
        adder += 1;
    }

    rt_oled_set_position(x, y + 2);
    for (t = 0; t < 32; t++)
    {
        rt_oled_write_data(Hzb[4 * n + 2][t]);
        adder += 1;
    }
    rt_oled_set_position(x, y + 3);
    for (t = 0; t < 32; t++)
    {
        rt_oled_write_data(Hzb[4 * n + 3][t]);
        adder += 1;
    }
}

/**
 * @brief 显示浮点数
 * @param x  位置 x 轴  0 - 127
 * @param y  位置 y 轴  0
 * @param num  要显示的浮点数
 * @param accuracy 保留小数位数
 * @param fontsize 字体大小 8/16
 */
void rt_oled_show_float(rt_uint8_t x, rt_uint8_t y, float num, rt_uint8_t accuracy, rt_uint8_t fontsize)
{
    rt_uint8_t i = 0;
    rt_uint8_t j = 0;
    rt_uint8_t t = 0;
    rt_uint8_t temp = 0;
    rt_uint16_t numel = 0;
    rt_uint32_t integer = 0;
    float decimals = 0;

    // 是否为负数？
    if (num < 0)
    {
        rt_oled_show_char(x, y, '-', fontsize);
        num = 0 - num;
        i++;
    }

    integer = (rt_uint32_t)num;
    decimals = num - integer;

    // 整数部分
    if (integer)
    {
        numel = integer;

        while (numel)
        {
            numel /= 10;
            j++;
        }
        i += (j - 1);
        for (temp = 0; temp < j; temp++)
        {
            rt_oled_show_char(x + 8 * (i - temp), y, integer % 10 + '0', fontsize);
            integer /= 10;
        }
    }
    else
    {
        rt_oled_show_char(x + 8 * i, y, temp + '0', fontsize);
    }
    i++;
    // 小数部分
    if (accuracy)
    {
        rt_oled_show_char(x + 8 * i, y, '.', fontsize);

        i++;
        for (t = 0; t < accuracy; t++)
        {
            decimals *= 10;
            temp = (rt_uint8_t)decimals;
            rt_oled_show_char(x + 8 * (i + t), y, temp + '0', fontsize);
            decimals -= temp;
        }
    }
}

/**
 * @brief OLED 幂函数
 * @param a 底数
 * @param n 指数
 * @return 结果
 */
static rt_uint32_t rt_oled_pow(rt_uint8_t a, rt_uint8_t n)
{
    rt_uint32_t result = 1;
    while (n--)
    {
        result *= a;
    }
    return result;
}

/**
 * @brief 显示 uint32 整数
 * @param x  位置 x 轴  0 - 127
 * @param y  位置 y 轴  0 - 3
 * @param num  显示的整数
 * @param length 整数位数
 * @param fontsize 字体大小
 */
void rt_oled_show_num(rt_uint8_t x, rt_uint8_t y, rt_uint32_t num, rt_uint8_t length, rt_uint8_t fontsize)
{
    rt_uint8_t t, temp;
    rt_uint8_t enshow = 0;
    rt_uint8_t char_width = (fontsize == 16) ? 8 : 6;  // 16号字体宽度8，8号字体宽度6
    
    for (t = 0; t < length; t++)
    {
        temp = (num / rt_oled_pow(10, length - t - 1)) % 10;
        if (enshow == 0 && t < (length - 1))
        {
            if (temp == 0)
            {
                rt_oled_show_char(x + char_width * t, y, ' ', fontsize);
                continue;
            }
            else
                enshow = 1;
        }
        rt_oled_show_char(x + char_width * t, y, temp + '0', fontsize);
    }
}

/**
 * @brief 显示 ASCII 字符串
 * @param x  字符串在 X 轴的起始位置  范围：0 - 127
 * @param y  字符串在 Y 轴的起始位置  范围：0 - 3 
 * @param ch  字符串指针
 * @param fontsize 字体大小 8/16
 */
void rt_oled_show_str(rt_uint8_t x, rt_uint8_t y, char *ch, rt_uint8_t fontsize)
{
    rt_uint8_t j = 0;
    while (ch[j] != '\0')
    {
        rt_oled_show_char(x, y, ch[j], fontsize);
        x += 8;
        if (x > 120)
        {
            x = 0;
            y += 2;
        }
        j++;
    }
}

/**
 * @brief 显示 ASCII 字符
 * @param x  字符在 X 轴的位置  范围：0 - 127
 * @param y  字符在 Y 轴的位置  范围：0 - 3 
 * @param ch  字符
 * @param fontsize 字体大小 8/16
 */
void rt_oled_show_char(rt_uint8_t x, rt_uint8_t y, rt_uint8_t ch, rt_uint8_t fontsize)
{
    rt_uint8_t c = 0, i = 0;
    c = ch - ' ';

    if (x > 127) // 超出右边界
    {
        x = 0;
        y++;
    }

    if (fontsize == 16)
    {
        rt_oled_set_position(x, y);
        for (i = 0; i < 8; i++)
        {
            rt_oled_write_data(F8X16[c * 16 + i]);
        }
        rt_oled_set_position(x, y + 1);
        for (i = 0; i < 8; i++)
        {
            rt_oled_write_data(F8X16[c * 16 + i + 8]);
        }
    }
    else
    {
        rt_oled_set_position(x, y);
        for (i = 0; i < 6; i++)
        {
            rt_oled_write_data(F6X8[c][i]);
        }
    }
}

/**
 * OLED 填充函数，使用后 0.91 英寸 OLED 屏幕变为全白
 */
void rt_oled_allfill(void)
{
    rt_uint8_t i, j;
    for (i = 0; i < 4; i++)
    {
        rt_oled_write_cmd(0xb0 + i);
        rt_oled_write_cmd(0x00);
        rt_oled_write_cmd(0x10);
        for (j = 0; j < 128; j++)
        {
            rt_oled_write_data(0xFF);
        }
    }
}

/**
 * @brief 设置坐标
 * @param x X 位置，范围 0 - 127
 * @param y Y 位置，范围 0 - 3
 */
void rt_oled_set_position(rt_uint8_t x, rt_uint8_t y)
{
    rt_oled_write_cmd(0xb0 + y);
    rt_oled_write_cmd(((x & 0xf0) >> 4) | 0x10);
    rt_oled_write_cmd((x & 0x0f) | 0x00);
}

/**
 * 清屏函数
 * 用 0 填充每行每列
 */
void rt_oled_clear(void)
{
    rt_uint8_t i, n;
    for (i = 0; i < 4; i++)
    {
        rt_oled_write_cmd(0xb0 + i);
        rt_oled_write_cmd(0x00);
        rt_oled_write_cmd(0x10);
        for (n = 0; n < 128; n++)
        {
            rt_oled_write_data(0);
        }
    }
}

/**
 * 开启和关闭屏幕显示
 */
void rt_oled_display_on(void)
{
    rt_oled_write_cmd(0x8D);
    rt_oled_write_cmd(0x14);
    rt_oled_write_cmd(0xAF);
}

void rt_oled_display_off(void)
{
    rt_oled_write_cmd(0x8D);
    rt_oled_write_cmd(0x10);
    rt_oled_write_cmd(0xAF);
}

/**
 * @brief 初始化 OLED
 * @return RT_EOK 成功，其他值失败
 */
rt_err_t rt_oled_init(void)
{
    rt_uint8_t i;
    
    rt_thread_mdelay(100);
    
    /* 发送初始化命令 */
    for (i = 0; i < sizeof(initcmd1); i++)
    {
        rt_oled_write_cmd(initcmd1[i]);
    }

    rt_oled_clear();
    rt_oled_set_position(0, 0);
    
    rt_kprintf("OLED initialized successfully\n");
    return RT_EOK;
}

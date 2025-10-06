/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-25     RT-Thread    OLED application layer
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "oled_app.h"
#include "rt_oled.h"

/* 外部声明 CubeMX 生成的 I2C 句柄 */
extern I2C_HandleTypeDef hi2c1;

/* OLED 显示线程 */
static void oled_proc(void *parameter)
{
    rt_uint32_t count = 0;
    
    while (1)
    {
        rt_oled_clear();
        
        /* 为0.91寸小屏幕优化的显示 */
        rt_oled_show_str(0, 0, "RT-Thread", 8);
        
        /* 显示运行时间（秒） */
        rt_uint32_t seconds = rt_tick_get() / RT_TICK_PER_SECOND;
        rt_oled_show_str(0, 1, "Time:", 8);
        rt_oled_show_num(30, 1, seconds, 4, 8);
        
        /* 显示计数器 */
        rt_oled_show_str(0, 2, "Cnt:", 8);
        rt_oled_show_num(24, 2, count++, 4, 8);
        
        rt_thread_mdelay(1000);
    }
}

/* OLED 初始化函数 */
void oled_init(void)
{
    rt_thread_t tid;
    
    /* 初始化 OLED（使用 CubeMX 配置的 I2C） */
    if (rt_oled_init() != 0)
    {
        return;
    }
    
    /* 创建 OLED 显示线程 */
    tid = rt_thread_create("oled", oled_proc, RT_NULL, 1024, 20, 10);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
}

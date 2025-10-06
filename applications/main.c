/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-22     RT-Thread    first version
 */

#include <bsp_system.h>

int main(void)
{
    led_init();
    key_init();
    oled_init();
    uart_init();
    return 0;
}

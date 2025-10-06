/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-22     13615       the first version
 */
#ifndef APPLICATIONS_LED_APP_H_
#define APPLICATIONS_LED_APP_H_

#include <bsp_system.h>

void led_init(void);

extern uint8_t ucLed[6];            // 声明 LED 数组
extern  rt_mutex_t ucled_mutex;      // 声明互斥锁

#endif /* APPLICATIONS_LED_APP_H_ */

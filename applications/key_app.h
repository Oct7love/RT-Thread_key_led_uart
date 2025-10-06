/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-22     13615       the first version
 */
#ifndef APPLICATIONS_KEY_APP_H_
#define APPLICATIONS_KEY_APP_H_

#include <bsp_system.h>
typedef struct
{
    uint8_t key_id;
    uint8_t action;
} key_msg_t;
extern rt_mq_t key_mq;
void key_init(void);

#endif /* APPLICATIONS_KEY_APP_H_ */

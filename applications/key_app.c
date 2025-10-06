/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-22     13615       the first version
 */
#include <key_app.h>

 /*初始化六个按键引脚*/
#define KEY1_PIN GET_PIN(E,15)
#define KEY2_PIN GET_PIN(E,13)
#define KEY3_PIN GET_PIN(E,11)
#define KEY4_PIN GET_PIN(E,9)
#define KEY5_PIN GET_PIN(E,7)
#define KEY6_PIN GET_PIN(B,0)

extern rt_mq_t key_mq;   // 消息队列来自其他模块共享


uint8_t key_read(void)
{
    uint8_t temp = 0;

    if(rt_pin_read(KEY1_PIN) == PIN_LOW) temp = 1;
    if(rt_pin_read(KEY2_PIN) == PIN_LOW) temp = 2;
    if(rt_pin_read(KEY3_PIN) == PIN_LOW) temp = 3;
    if(rt_pin_read(KEY4_PIN) == PIN_LOW) temp = 4;
    if(rt_pin_read(KEY5_PIN) == PIN_LOW) temp = 5;
    if(rt_pin_read(KEY6_PIN) == PIN_LOW) temp = 6;

    return temp;
}


uint8_t key_val = 0;
uint8_t key_down = 0;
uint8_t key_up = 0;
uint8_t key_old = 0;

void key_proc()
{
        key_msg_t msg;
        while(1)
    {
        key_val  = key_read();
        key_down = key_val & (key_old ^ key_val);
        key_up   = ~key_val & (key_old ^ key_val);
        key_old  = key_val;

        if(key_down >= 1 && key_down <= 6)
       {
            msg.key_id = key_down;
            msg.action = 1;   // 按下
            rt_mq_send(key_mq, &msg, sizeof(msg));
       }

        if(key_up >= 1 && key_up <= 6)
       {
            msg.key_id = key_up;
            msg.action = 0;   // 松开
            rt_mq_send(key_mq, &msg, sizeof(msg));
               }

        rt_thread_delay(10);
    }

}


/*初始化引脚 & 创建线程*/
void key_init(void)
{


    rt_pin_mode(KEY1_PIN,PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY2_PIN,PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY3_PIN,PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY4_PIN,PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY5_PIN,PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY6_PIN,PIN_MODE_INPUT_PULLUP);

    rt_thread_t key_thread = rt_thread_create("key_proc",
                                                key_proc,
                                                RT_NULL,
                                                1024,
                                                10,
                                                10);

    if(key_thread != RT_NULL)
    {
          rt_thread_startup(key_thread);
    }

}

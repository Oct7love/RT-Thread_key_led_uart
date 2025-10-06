/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-22     13615       the first version
 */
#include <led_app.h>

#define LED_PIN_BASE GET_PIN(D,8) //led基地址，第一个led

uint8_t ucLed[6] = {1,1,1,1,1,1}; //初始led数组
 rt_mutex_t ucled_mutex = RT_NULL;
 rt_mq_t key_mq = RT_NULL;     // 消息队列实例

void led_disp(uint8_t *ucLed)
{
    uint8_t temp = 0x00;
    static uint8_t  temp_old = 0xff;

    rt_mutex_take(ucled_mutex, RT_WAITING_FOREVER); // 加锁
    for(int i = 0;i < 6;i++)
    {
        temp |= (ucLed[i] << i);
    }
    rt_mutex_release(ucled_mutex);                 // 解锁


    if(temp != temp_old)
    {
        for(int i = 0;i < 6;i++)
        {
             if((temp >> i) & 0x01)
            {
                rt_pin_write(LED_PIN_BASE + i , PIN_LOW);//低电平点亮，如果是0，代表点亮
            }
             else
            {
                rt_pin_write(LED_PIN_BASE + i , PIN_HIGH);
            }
        }
       temp_old = temp;
    }
}

void led_proc(void *parameter)
{
    key_msg_t msg;
    while(1)
    {
            if (rt_mq_recv(key_mq, &msg, sizeof(msg), RT_WAITING_FOREVER) == RT_EOK)
           {
            rt_mutex_take(ucled_mutex, RT_WAITING_FOREVER);
            if (msg.action == 1)
                ucLed[msg.key_id - 1] = 0;   // 按下 → 点亮
            else
                ucLed[msg.key_id - 1] = 1;   // 松开 → 熄灭
            rt_mutex_release(ucled_mutex);
            led_disp(ucLed);
            rt_thread_delay(1); //让出cpu,空闲
           }
}
}
void led_init(void)
{
    ucled_mutex = rt_mutex_create("led_lock", RT_IPC_FLAG_PRIO);
    for(int i = 0; i < 6; i++)
    {
        rt_pin_mode(LED_PIN_BASE + i, PIN_MODE_OUTPUT);
    }

    /* 创建消息队列 */
    key_mq = rt_mq_create("key_mq",
                           sizeof(key_msg_t),   // 单条消息大小
                           8,                   // 消息数量
                           RT_IPC_FLAG_FIFO);   // 先进先出

    rt_thread_t led_thread = rt_thread_create("led_proc",
                                               led_proc, //线程入口函数
                                               RT_NULL, //线程入口参数
                                               1024,    //堆栈大小
                                               10,      //线程优先级
                                               10);      //时间片大小
    if(led_thread != RT_NULL)
    {
        rt_thread_startup(led_thread);
    }
}

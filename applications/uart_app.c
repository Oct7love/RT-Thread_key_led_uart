/*

 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-09-25     13615       the first version
*/
#include <uart_app.h>
#include <rtthread.h>

#define SAMPLE_UART_NAME "uart1"
extern rt_mq_t key_mq;

/* 信号量，用于接收数据通知 */
static struct rt_semaphore rx_sem;
static rt_device_t serial = RT_NULL;

/* 接收回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem);  // 发送信号量
    return RT_EOK;
}

/* 处理串口接收到的数据 */
static void serial_thread_entry(void *parameter)
{
    char ch;
    while (1)
    {
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);  // 等待信号量
        while (rt_device_read(serial, -1, &ch, 1) == 1)
        {
            rt_device_write(serial, 0, &ch, 1);  // 回显接收到的数据
        }
    }
}

void uart_proc(void *parameter)
{
    key_msg_t msg;
    char buf[32];

    while(1)
    {
        if (rt_mq_recv(key_mq, &msg, sizeof(msg), RT_WAITING_FOREVER) == RT_EOK)
        {
            if (msg.action == 1)
                rt_sprintf(buf, "key%d press\r\n", msg.key_id);
            else
                rt_sprintf(buf, "key%d release\r\n", msg.key_id);

            rt_device_write(serial, 0, buf, rt_strlen(buf));
        }
    }
}

static int uart_sample(void)
{
    /* 查找并打开串口设备 */
    serial = rt_device_find(SAMPLE_UART_NAME);
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);

    /* 初始化信号量和接收回调 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    rt_device_set_rx_indicate(serial, uart_input);

    /* 创建线程处理接收到的数据 */
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024, 25, 10);
    rt_thread_startup(thread);

    return 0;
}

void uart_init(void)
{
    serial = rt_device_find(SAMPLE_UART_NAME);
    if(serial)
        rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);

    rt_thread_t uart_thread = rt_thread_create("uart_proc",
                                               uart_proc,
                                               RT_NULL,
                                               1024,
                                               12,
                                               10);
    if(uart_thread != RT_NULL)
        rt_thread_startup(uart_thread);
}
INIT_APP_EXPORT(uart_sample);

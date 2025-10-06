# OLED (SSD1306) 移植到 RT-Thread 完整指南

## 📋 概述

本文档详细介绍如何将 OLED (SSD1306) 显示屏库从 STM32 HAL 库移植到 RT-Thread 操作系统。

## 🎯 移植目标

- 将基于 STM32 HAL 库的 OLED 驱动移植到 RT-Thread
- 保持原有的显示功能和 API 接口
- 添加 RT-Thread 应用层封装
- 支持多线程环境下的 OLED 显示

## 🔧 硬件环境

- **MCU**: STM32F429VET6
- **OLED**: 0.91寸 SSD1306 (I2C接口)
- **连接方式**:
  ```
  OLED    →    STM32F429VET6
  VCC     →    3.3V
  GND     →    GND
  SDA     →    PB9 (I2C1_SDA)
  SCL     →    PB8 (I2C1_SCL)
  ```

## 📁 文件结构

移植完成后的文件结构：
```
applications/
├── rt_oled.h          # OLED 驱动层头文件
├── rt_oled.c          # OLED 驱动层实现
├── oledfont.h         # 字体库文件
├── oled_app.h         # OLED 应用层头文件
├── oled_app.c         # OLED 应用层实现
└── oled_final_test.c  # 测试和诊断工具
```

## 🚀 移植步骤

### 步骤 1: 准备原始文件

将原始的 OLED 库文件放入 `uploads` 文件夹：
- `oled.h` - 原始头文件
- `oled.c` - 原始实现文件
- `oledfont.h` - 字体库
- `oledpic.h` - 图片库（可选）

### 步骤 2: 创建 RT-Thread 驱动层

#### 2.1 创建 `rt_oled.h`

```c
#ifndef __RT_OLED_H__
#define __RT_OLED_H__

#include <rtthread.h>
#include "stm32f4xx_hal.h"

/* OLED 设备地址 */
#define OLED_ADDR 0x78

/* OLED 尺寸定义 */
#define OLED_WIDTH  128
#define OLED_HEIGHT 32

/* 函数声明 */
rt_err_t rt_oled_init(void);
void rt_oled_clear(void);
void rt_oled_show_str(rt_uint8_t x, rt_uint8_t y, char *ch, rt_uint8_t fontsize);
void rt_oled_show_num(rt_uint8_t x, rt_uint8_t y, rt_uint32_t num, rt_uint8_t length, rt_uint8_t fontsize);
void rt_oled_show_char(rt_uint8_t x, rt_uint8_t y, rt_uint8_t ch, rt_uint8_t fontsize);
void rt_oled_set_position(rt_uint8_t x, rt_uint8_t y);
void rt_oled_display_on(void);
void rt_oled_display_off(void);

#endif
```

#### 2.2 创建 `rt_oled.c`

关键修改点：
1. **替换延时函数**: `HAL_Delay()` → `rt_thread_mdelay()`
2. **保持 I2C 通信**: 继续使用 `HAL_I2C_Mem_Write()`
3. **添加 RT-Thread 头文件**: `#include <rtthread.h>`

```c
#include "rt_oled.h"
#include "oledfont.h"

extern I2C_HandleTypeDef hi2c1;

// 保持原有的显示函数，只需修改延时调用
rt_err_t rt_oled_init(void)
{
    rt_thread_mdelay(100);  // 替换 HAL_Delay(100)
    
    // 原有的初始化序列...
    rt_kprintf("OLED initialized successfully\n");
    return RT_EOK;
}
```

### 步骤 3: 创建应用层

#### 3.1 创建 `oled_app.h`

```c
#ifndef __OLED_APP_H__
#define __OLED_APP_H__

#include <bsp_system.h>

/* 函数声明 */
void oled_init(void);

#endif
```

#### 3.2 创建 `oled_app.c`

```c
#include <oled_app.h>
#include <rt_oled.h>

static rt_thread_t oled_thread = RT_NULL;

static void oled_proc(void *parameter)
{
    rt_uint32_t count = 0;
    
    while (1)
    {
        rt_oled_clear();
        rt_oled_show_str(0, 0, "RT-Thread", 16);
        rt_oled_show_num(0, 2, count++, 5, 16);
        
        rt_thread_mdelay(100);
    }
}

void oled_init(void)
{
    /* 初始化 OLED 硬件 */
    rt_oled_init();
    
    /* 创建 OLED 显示线程 */
    oled_thread = rt_thread_create("oled", oled_proc, RT_NULL, 
                                   1024, 15, 20);
    if (oled_thread != RT_NULL)
    {
        rt_thread_startup(oled_thread);
        rt_kprintf("OLED thread created successfully\n");
    }
}
```

### 步骤 4: 配置 I2C 硬件

#### 4.1 确定正确的 I2C 引脚

**重要**: 必须确定 OLED 实际连接的 I2C 引脚！

STM32F429 的 I2C1 可以映射到：
- PB6 (SCL) + PB7 (SDA)
- **PB8 (SCL) + PB9 (SDA)** ← 常用配置

#### 4.2 I2C 配置参数

确保 I2C 配置与原 Keil 项目一致：
```c
hi2c1.Init.ClockSpeed = 400000;        // 400kHz Fast Mode
hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2; // Tlow/Thigh = 2
```

### 步骤 5: 集成到项目

#### 5.1 修改 `bsp_system.h`

```c
#include <oled_app.h>
```

#### 5.2 修改 `main.c`

```c
int main(void)
{
    // 其他初始化...
    
    oled_init();  // 添加 OLED 初始化
    
    return RT_EOK;
}
```

## 🔍 常见问题排查

### 问题 1: OLED 不亮

**可能原因**: I2C 引脚配置错误

**解决方案**: 
1. 使用 I2C 扫描工具确定设备地址
2. 检查实际的硬件连接引脚
3. 确认 I2C 时钟频率设置

**测试命令**:
```bash
# 扫描 I2C 设备
i2c_scan

# 测试不同引脚组合
test_pb8_pb9
check_all_pins
```

### 问题 2: 编译错误

**可能原因**: 头文件路径或依赖问题

**解决方案**:
1. 确保所有文件在 `applications` 文件夹中
2. 检查 `#include` 路径
3. 确认 RT-Thread 配置正确

### 问题 3: I2C 通信失败

**可能原因**: I2C 时钟频率不匹配

**解决方案**:
```c
// 尝试不同的时钟频率
hi2c1.Init.ClockSpeed = 100000;  // 100kHz Standard Mode
hi2c1.Init.ClockSpeed = 400000;  // 400kHz Fast Mode
```

## 🛠️ 测试工具

移植过程中提供了多个测试工具：

### 基础测试
```bash
oled_final_test    # 完整的 OLED 测试
i2c_status         # 检查 I2C 硬件状态
gpio_test          # 测试 GPIO 引脚状态
```

### 高级诊断
```bash
i2c_scan           # 扫描 I2C 总线设备
oled_manual_test   # 手动 OLED 通信测试
test_pin_combinations  # 测试不同引脚组合
```

## 📊 性能优化

### 显示刷新优化
```c
// 避免频繁清屏
static char last_str[32] = {0};
if (strcmp(current_str, last_str) != 0) {
    rt_oled_show_str(0, 0, current_str, 16);
    strcpy(last_str, current_str);
}
```

### 线程优先级设置
```c
// OLED 显示线程优先级建议设为中等
rt_thread_create("oled", oled_proc, RT_NULL, 1024, 15, 20);
//                                            ^    ^   ^
//                                         栈大小 优先级 时间片
```

## 🎯 API 使用示例

### 基础显示
```c
rt_oled_clear();                                    // 清屏
rt_oled_show_str(0, 0, "Hello RT-Thread", 16);    // 显示字符串
rt_oled_show_num(0, 2, 12345, 5, 16);             // 显示数字
```

### 系统信息显示
```c
void show_system_info(void)
{
    rt_oled_clear();
    rt_oled_show_str(0, 0, "RT-Thread", 16);
    
    // 显示内存使用情况
    rt_uint32_t total, used, max_used;
    rt_memory_info(&total, &used, &max_used);
    rt_oled_show_str(0, 1, "Mem:", 8);
    rt_oled_show_num(32, 1, used, 6, 8);
    
    // 显示线程数量
    rt_oled_show_str(0, 2, "Threads:", 8);
    rt_oled_show_num(64, 2, rt_object_get_length(RT_Object_Class_Thread), 2, 8);
}
```

## ✅ 移植验证

移植成功的标志：
1. ✅ 编译无错误无警告
2. ✅ I2C 扫描能找到 OLED 设备 (地址 0x78)
3. ✅ OLED 屏幕能正常显示文字
4. ✅ 应用线程正常运行
5. ✅ 系统稳定，无死机重启

## 📝 总结

OLED 移植的关键点：
1. **硬件配置正确** - 确认 I2C 引脚和时钟频率
2. **API 适配** - 替换 HAL 延时函数为 RT-Thread 函数
3. **线程安全** - 合理设计应用层线程
4. **错误处理** - 添加完善的错误检查和恢复机制

通过本指南，你可以成功将任何基于 STM32 HAL 库的 OLED 驱动移植到 RT-Thread 系统中。

---

**移植完成时间**: 约 30-60 分钟  
**难度等级**: ⭐⭐⭐☆☆ (中等)  
**适用范围**: STM32 + RT-Thread + SSD1306 OLED
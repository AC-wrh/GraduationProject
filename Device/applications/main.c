/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-09-09     RT-Thread    first version
 */

#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* PLEASE DEFINE the LED0 pin for your board, such as: PA5 */
#define LED0_PIN                GET_PIN(A, 5)

#define SHT3X_I2C_BUS_NAME      "i2c1"
#define SHT3X_ADDR              0x44

// const char *sht3x_i2c_bus_name = "i2c1";


int main(void)
{
    // int count = 1;
    // /* set LED0 pin mode to output */
    // rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    // while (count++)
    // {
    //     /* set LED0 pin level to high or low */
    //     rt_pin_write(LED0_PIN, count % 2);
    //     LOG_D("Hello RT-Thread!");
    //     rt_thread_mdelay(1000);
    // }

    // return RT_EOK;

    sht3x_device_t dev;
    int count = 1;
    rt_uint32_t read_value = 0;

    mqtt_start();
    rt_hw_i2c_init(SHT3X_I2C_BUS_NAME, GET_PIN(B, 8), GET_PIN(B, 9));
    dev = sht3x_init(SHT3X_I2C_BUS_NAME, SHT3X_ADDR);
    MX_ADC1_Init();

    while (count++)
    {
        read_value = get_adc_value();
        rt_thread_mdelay(1000);
        rt_kprintf("adc value = %d\r\n", read_value);

        sht3x_read_singleshot(dev);
        rt_thread_mdelay(1000);
        rt_kprintf("sht3x humidity   : %d.%d \n", (int)dev->humidity, (int)(dev->humidity * 10) % 10);
        rt_kprintf("sht3x temperature: %d.%d \n", (int)dev->temperature, (int)(dev->temperature * 10) % 10);
    }
    

    return RT_EOK;
}

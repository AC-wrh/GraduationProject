/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "drv_sensor.h"
#include "test.h"
// #include "drv_soft_i2c.h"
#include <wifi_mqtt.h>

/* defined the LED0 pin: PE7 */
// #define LED0_PIN    GET_PIN(E, 7)

#define SHT3X_ADDR      0x44


int main(void)
{
    // int count = 1;
    // /* set LED0 pin mode to output */
    // rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    // while (count++)
    // {
    //     rt_pin_write(LED0_PIN, PIN_HIGH);
    //     rt_thread_mdelay(500);
    //     rt_pin_write(LED0_PIN, PIN_LOW);
    //     rt_thread_mdelay(500);
    // }

    // const char *i2c_bus_name = "i2c1";

    
    mqtt_start();
    // rt_hw_i2c_init();
    rt_hw_i2c_init("i2c1", GET_PIN(B, 8), GET_PIN(B, 9));
    // sht3x_init(i2c_bus_name, SHT3X_ADDR);
    



    return RT_EOK;
}

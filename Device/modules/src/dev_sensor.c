/*
 * @Description: sensor 
 * @version: 
 * @Author: Adol
 * @Date: 2020-01-31 19:37:41
 * @LastEditTime : 2020-01-31 21:00:22
 */
#include <board.h>

#define DBG_TAG "dev_sensor"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/**
 * 1. sht3x sensor
*/
#include "drv_i2c.h"
#include "sht3x.h"

#define PIN_I2C1_SCL GET_PIN(B, 8)
#define PIN_I2C1_SDA GET_PIN(B, 9)
#define SHT3X_I2C_BUS_NAME "i2c1"
#define SHT3X_ADDR 0x44

static sht3x_device_t dev_sht3x = RT_NULL;
static uint8_t dev_sht3x_inited = 0;
static uint8_t dev_sht3x_busy = 0;

rt_err_t dev_sensor_sht3x_deinit(void)
{
    if (dev_sht3x == RT_NULL)
    {
        return -RT_ERROR;
    }

    dev_sht3x_busy = 1;

    sht3x_deinit(dev_sht3x);
    dev_sht3x = RT_NULL;
    dev_sht3x_inited = 0;
    dev_sht3x_busy = 0;

    return RT_EOK;
}

rt_err_t dev_sensor_sht3x_init(void)
{
    if (dev_sht3x_inited)
    {
        return RT_EOK;
    }
    else
    {
        dev_sensor_sht3x_deinit();
    }

    dev_sht3x_busy = 1;

    rt_hw_i2c_init(SHT3X_I2C_BUS_NAME, PIN_I2C1_SCL, PIN_I2C1_SDA);
    dev_sht3x = sht3x_init(SHT3X_I2C_BUS_NAME, SHT3X_ADDR);
    if (dev_sht3x == RT_NULL)
    {
        LOG_E("The sht3x sensor initializes failure");
        return -RT_ERROR;
    }

    dev_sht3x_inited = 1;
    dev_sht3x_busy = 0;

    LOG_I("sht3x device init success!");

    return RT_EOK;
}

/**
 * 2. mq2 sensor
*/
static uint8_t dev_mq2_inited = 0;
static uint8_t dev_mq2_busy = 0;

rt_err_t dev_sensor_mq2_deinit(void)
{
    ;
}

rt_err_t dev_sensor_mq2_init(void)
{
    if (dev_mq2_inited)
    {
        return RT_EOK;
    }

    dev_mq2_busy = 1;

    

    dev_sht3x_inited = 1;
    dev_sht3x_busy = 0;

    LOG_I("mq2 device init success!");

    return RT_EOK;
}

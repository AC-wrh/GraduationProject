/*
 * @Description: sensor device control
 * @version: 1.0
 * @Author: Adol
 * @Date: 2020-01-31 19:37:41
 * @LastEditTime : 2020-02-04 20:46:22
 */
#include <board.h>

#define DBG_TAG "dev_sensor"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

dev_sensor_data_t dev_sensor_data_result = {
    .sht3x_data_temp = 0.0f,
    .sht3x_data_humi = 0.0f,
    .mq2_data = 0,
    .zph02_data = 0,

    .sht3x_status = DEV_SENSOR_DEINIT,
    .mq2_status = DEV_SENSOR_DEINIT,
    .zph02_status = DEV_SENSOR_DEINIT,

    .relay1_status = DEV_SENSOR_DEINIT,
    .relay2_status = DEV_SENSOR_DEINIT,
    .beep_status = DEV_SENSOR_DEINIT,

    .status = 0};

/**
 * 1. relay and beep device control
*/
rt_err_t dev_sensor_relay1_ctl(dev_sensor_t relay_t)
{
    switch (relay_t)
    {
    case DEV_SENSOR_INIT:
        /* set the pin mode to push-pull output */
        rt_pin_mode(PIN_RELAY1, PIN_MODE_OUTPUT);
        rt_pin_write(PIN_RELAY1, PIN_HIGH);
        dev_sensor_data_result.relay1_status = DEV_SENSOR_CLOSE;
        break;
    case DEV_SENSOR_DEINIT:
        rt_pin_write(PIN_RELAY1, PIN_HIGH);
        dev_sensor_data_result.relay1_status = DEV_SENSOR_CLOSE;
        break;
    case DEV_SENSOR_OPEN:
        rt_pin_write(PIN_RELAY1, PIN_LOW);
        dev_sensor_data_result.relay1_status = DEV_SENSOR_OPEN;
        mqtt_publish(mqtt_message_buffer[RELAY_1_ON]);
        break;
    case DEV_SENSOR_CLOSE:
        rt_pin_write(PIN_RELAY1, PIN_HIGH);
        dev_sensor_data_result.relay1_status = DEV_SENSOR_CLOSE;
        mqtt_publish(mqtt_message_buffer[RELAY_1_OFF]);
        break;
    default:
        break;
    }

    return RT_EOK;
}

rt_err_t dev_sensor_relay2_ctl(dev_sensor_t relay_t)
{
    switch (relay_t)
    {
    case DEV_SENSOR_INIT:
        /* set the pin mode to push-pull output */
        rt_pin_mode(PIN_RELAY2, PIN_MODE_OUTPUT);
        rt_pin_write(PIN_RELAY2, PIN_HIGH);
        dev_sensor_data_result.relay2_status = DEV_SENSOR_CLOSE;
        break;
    case DEV_SENSOR_DEINIT:
        rt_pin_write(PIN_RELAY2, PIN_HIGH);
        dev_sensor_data_result.relay2_status = DEV_SENSOR_CLOSE;
        break;
    case DEV_SENSOR_OPEN:
        rt_pin_write(PIN_RELAY2, PIN_LOW);
        dev_sensor_data_result.relay2_status = DEV_SENSOR_OPEN;
        mqtt_publish(mqtt_message_buffer[RELAY_2_ON]);
        break;
    case DEV_SENSOR_CLOSE:
        rt_pin_write(PIN_RELAY2, PIN_HIGH);
        dev_sensor_data_result.relay2_status = DEV_SENSOR_CLOSE;
        mqtt_publish(mqtt_message_buffer[RELAY_2_OFF]);
        break;
    default:
        break;
    }

    return RT_EOK;
}

rt_err_t dev_sensor_beep_ctl(dev_sensor_t beep_t)
{
    switch (beep_t)
    {
    case DEV_SENSOR_INIT:
        /* set the pin mode to push-pull output */
        rt_pin_mode(PIN_BEEP, PIN_MODE_OUTPUT);
        rt_pin_write(PIN_BEEP, PIN_HIGH);
        dev_sensor_data_result.beep_status = DEV_SENSOR_CLOSE;
        break;
    case DEV_SENSOR_DEINIT:
        rt_pin_write(PIN_BEEP, PIN_HIGH);
        dev_sensor_data_result.beep_status = DEV_SENSOR_CLOSE;
        break;
    case DEV_SENSOR_OPEN:
        rt_pin_write(PIN_BEEP, PIN_LOW);
        dev_sensor_data_result.beep_status = DEV_SENSOR_OPEN;
        mqtt_publish(mqtt_message_buffer[BEEP_ON]);
        break;
    case DEV_SENSOR_CLOSE:
        rt_pin_write(PIN_BEEP, PIN_HIGH);
        dev_sensor_data_result.beep_status = DEV_SENSOR_CLOSE;
        mqtt_publish(mqtt_message_buffer[BEEP_OFF]);
        break;
    default:
        break;
    }

    return RT_EOK;
}

rt_err_t dev_sensor_ctl(rt_base_t pin_t, dev_sensor_t status_t)
{
    switch (status_t)
    {
    case DEV_SENSOR_INIT:
        /* set the pin mode to push-pull output */
        rt_pin_mode(pin_t, PIN_MODE_OUTPUT);
        rt_pin_write(pin_t, PIN_HIGH);
        break;
    case DEV_SENSOR_DEINIT:
        rt_pin_write(pin_t, PIN_HIGH);
        break;
    case DEV_SENSOR_OPEN:
        rt_pin_write(pin_t, PIN_LOW);
        break;
    case DEV_SENSOR_CLOSE:
        rt_pin_write(pin_t, PIN_HIGH);
        break;
    default:
        break;
    }

    return RT_EOK;
}

/**
 * 2. sht3x sensor
*/
#include "drv_i2c.h"
#include "sht3x.h"

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
 * 3. mq2 sensor
*/
#include "drv_adc.h"

static uint8_t dev_mq2_inited = 0;
static uint8_t dev_mq2_busy = 0;

// rt_err_t dev_sensor_mq2_deinit(void)
// {
//     ;
// }

rt_err_t dev_sensor_mq2_init(void)
{
    if (dev_mq2_inited)
    {
        return RT_EOK;
    }

    dev_mq2_busy = 1;
    MX_ADC1_Init();
    dev_mq2_inited = 1;
    dev_mq2_busy = 0;

    LOG_I("mq2 device init success!");

    return RT_EOK;
}

/**
 * 4. zph02 sensor
*/
// static uint8_t dev_zph02_inited = 0;
// static uint8_t dev_zph02_busy = 0;

/**
 * 5. sensor data 
*/
char dev_sensor_all[100];
char dev_sensor_data[100];

void dev_sensor_mqtt_buffer(void)
{
    if (dev_sensor_data_result.status)
    {
        rt_sprintf(dev_sensor_all, "D$S-A_%d.%d_%d.%d_%d_%d_%d_%d_%d\n", (int)dev_sensor_data_result.sht3x_data_temp,
                                                                        (int)(dev_sensor_data_result.sht3x_data_temp * 10) % 10,
                                                                        (int)dev_sensor_data_result.sht3x_data_humi, 
                                                                        (int)(dev_sensor_data_result.sht3x_data_humi * 10) % 10,
                                                                        (int)dev_sensor_data_result.mq2_data,
                                                                        0,
                                                                        (int)dev_sensor_data_result.relay1_status,
                                                                        (int)dev_sensor_data_result.relay2_status,
                                                                        (int)dev_sensor_data_result.beep_status);

        rt_sprintf(dev_sensor_data, "D$S-D_%d.%d_%d.%d_%d_%d\n", (int)dev_sensor_data_result.sht3x_data_temp,
                                                                (int)(dev_sensor_data_result.sht3x_data_temp * 10) % 10,
                                                                (int)dev_sensor_data_result.sht3x_data_humi, 
                                                                (int)(dev_sensor_data_result.sht3x_data_humi * 10) % 10,
                                                                (int)dev_sensor_data_result.mq2_data,
                                                                0);
    }
}

void dev_sensor_data_upload(dev_sensor_data_t *in_data, dev_sensor_data_t *out_data)
{
    rt_base_t level = rt_hw_interrupt_disable();
    
    out_data->sht3x_data_temp = in_data->sht3x_data_temp;
    out_data->sht3x_data_humi = in_data->sht3x_data_humi;
    out_data->mq2_data = in_data->mq2_data;
    out_data->zph02_data = in_data->zph02_data;
    
    rt_hw_interrupt_enable(level);
}



void dev_sensor_data_read(void)
{
    dev_sensor_data_t sensor_data;

    dev_sensor_data_result.status = 0;
    dev_sensor_data_upload(&dev_sensor_data_result, &sensor_data);

    if (dev_sht3x_inited == 0)
    { /* sht3x */
        LOG_E("sht3x init failed! Retry...");
        rt_thread_mdelay(2000);
        if (dev_sht3x_busy == 0)
        {
            dev_sensor_sht3x_init();
        }
    }
    else
    {
        sensor_data.sht3x_status = -RT_ERROR;
        sht3x_read_singleshot(dev_sht3x);
        // rt_thread_mdelay(1);
        sensor_data.sht3x_data_temp = dev_sht3x->temperature;
        sensor_data.sht3x_data_humi = dev_sht3x->humidity;
        sensor_data.sht3x_status = RT_EOK;
    }

    if (dev_mq2_inited == 0)
    { /* mq2 */
        LOG_E("mq2 init failed! Retry...");
        rt_thread_mdelay(2000);
        if (dev_mq2_busy == 0)
        {
            dev_sensor_mq2_init();
        }
    }
    else
    {
        sensor_data.mq2_status = -RT_ERROR;
        // rt_thread_mdelay(1);
        sensor_data.mq2_data = get_adc_value();
        sensor_data.mq2_status = RT_EOK;
    }

    // if (dev_sht3x_inited == 0)
    // {/* zph02 */
    //     LOG_E("sht3x init failed! Retry...");
    //     rt_thread_mdelay(2000);
    //     if (dev_sht3x_busy == 0)
    //     {
    //         dev_sensor_sht3x_init();
    //     }
    // }
    // else
    // {
    //     rt_thread_mdelay(1);
    //     sht3x_read_singleshot(dev_sht3x);
    //     sensor_data.sht3x_data_temp = dev_sht3x->temperature;
    //     sensor_data.sht3x_data_humi = dev_sht3x->humidity;
    // }


    dev_sensor_data_upload(&sensor_data, &dev_sensor_data_result);
    dev_sensor_data_result.status = 1;
    // rt_thread_mdelay(10);
    // dev_sensor_read_buffer();

    

}

static void _dev_sensor_read_thr(void *arg)
{
    do
    {
        dev_sensor_data_read();
        rt_thread_mdelay(1);
        dev_sensor_mqtt_buffer();
        rt_thread_mdelay(1000);
    } while (1);
}

void dev_sensor_read_start(void)
{
    rt_thread_t sensor_read_thr = RT_NULL;
    sensor_read_thr = rt_thread_create("sensor",
                                       _dev_sensor_read_thr,
                                       RT_NULL,
                                       1024, 12, 10);
    if (sensor_read_thr != RT_NULL)
    {
        rt_thread_startup(sensor_read_thr);
    }
}

rt_err_t dev_sensor_init(void)
{
    dev_sensor_relay1_ctl(DEV_SENSOR_INIT);
    dev_sensor_relay2_ctl(DEV_SENSOR_INIT);
    dev_sensor_beep_ctl(DEV_SENSOR_INIT);
    // rt_thread_mdelay(100);
    dev_sensor_sht3x_init();
    // rt_thread_mdelay(100);
    dev_sensor_mq2_init();
    rt_thread_mdelay(500);
    mqtt_start();
    rt_thread_mdelay(500);
    dev_sensor_read_start();

    return RT_EOK;
}

/*
 * @Description: 
 * @version: 
 * @Author: Adol
 * @Date: 2020-01-31 19:37:49
 * @LastEditTime : 2020-02-01 19:13:26
 */
#ifndef __DEV_SENSOR_H__
#define __DEV_SENSOR_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>
#include <rtdevice.h>
#include <rtdef.h>

/**
 * device pin config
*/
#define PIN_RELAY1      GET_PIN(D, 12)
#define PIN_RELAY2      GET_PIN(D, 13)
#define PIN_RELAY3      GET_PIN(D, 14)
#define PIN_BEEP        GET_PIN(D, 15)

/**
 * 1. device status list
*/
typedef enum
{
    DEV_SENSOR_INIT   = 0,
    DEV_SENSOR_DEINIT = 1,
    DEV_SENSOR_OPEN   = 2,
    DEV_SENSOR_CLOSE  = 3,
    DEV_SENSOR_MAX    = 4
} dev_sensor_t;

typedef struct
{
    float sht3x_data_temp;
    float sht3x_data_humi;
    rt_err_t sht3x_status;

    rt_uint32_t mq2_data;
    rt_err_t mq2_status;

    rt_uint8_t zph02_data;
    rt_err_t zph02_status;

    rt_err_t delay1_status;
    rt_err_t delay2_status;
    rt_err_t beep_status;

    uint16_t status; /* 0: read finish; 1: not finish */

    uint8_t reserved;
} dev_sensor_data_t;

// rt_err_t dev_sensor_beep_ctl(dev_sensor_t beep_t);
// rt_err_t dev_sensor_relay1_ctl(dev_sensor_t relay_t);
// rt_err_t dev_sensor_relay2_ctl(dev_sensor_t relay_t);
// rt_err_t dev_sensor_relay3_ctl(dev_sensor_t relay_t);
rt_err_t dev_sensor_ctl(rt_base_t pin_t, dev_sensor_t status_t);

void dev_sensor_read_start(void);

#ifdef __cplusplus
}
#endif

#endif 

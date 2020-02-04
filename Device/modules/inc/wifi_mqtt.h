/*
 * @Descripttion: 
 * @version: 
 * @Author: adol
 * @Date: 2019-11-07 21:03:16
 * @LastEditors  : Please set LastEditors
 * @LastEditTime : 2020-02-04 20:32:53
 */
/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-17     MurphyZhao   first implementation
 */

#ifndef __WIFI_MQTT_H__
#define __WIFI_MQTT_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <rtthread.h>
#include <stdint.h>
#include <rtdevice.h>

typedef enum
{
    DEVICE_NONE,
    
    APPLETS_ONLINE,
    APPLETS_OFFLINE,
    DEVICE_ONLINE,
    DEVICE_OFFLINE,
    
    DEVICE_SENSOR_ALL,
    DEVICE_SENSOR_DATA,
    RELAY_1_ON,
    RELAY_1_OFF,
    RELAY_2_ON,
    RELAY_2_OFF,
    BEEP_ON,
    BEEP_OFF,

    DEVICE_MAX
} iotb_mqtt_message_t;

int mqtt_start(void);
int mqtt_publish(const char *send_str);

extern char *mqtt_message_buffer[DEVICE_MAX];

#ifdef __cplusplus
}
#endif

#endif

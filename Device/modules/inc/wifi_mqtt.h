/*
 * @Descripttion: 
 * @version: 
 * @Author: adol
 * @Date: 2019-11-07 21:03:16
 * @LastEditors  : Please set LastEditors
 * @LastEditTime : 2020-02-02 14:15:21
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


// /**
//  * 4. SWITCH
// */
// typedef enum
// {
//     IOTB_SWITCH_INIT,
//     IOTB_SWITCH_1_ON,
//     IOTB_SWITCH_1_OFF,
//     IOTB_SWITCH_2_ON,
//     IOTB_SWITCH_2_OFF,
//     IOTB_SWITCH_3_ON,
//     IOTB_SWITCH_3_OFF,
// } iotb_switch_t;

typedef enum
{
    DEVICE_NONE,
    
    APPLETS_ONLINE,
    APPLETS_OFFLINE,
    DEVICE_ONLINE,
    DEVICE_OFFLINE,
    DEVICE_DATA,

    SWITCH_1_ON,
    SWITCH_1_OFF,
    SWITCH_2_ON,
    SWITCH_2_OFF,
    SWITCH_3_ON,
    SWITCH_3_OFF,

    DEVICE_MAX
} iotb_mqtt_message_t;



// rt_err_t wifi_mqtt_init();
// void wifi_mqtt_start();
// void wifi_mqtt_publish(const char *send_str);

int mqtt_start(void);
int mqtt_publish(const char *send_str);

extern char *mqtt_message_buffer[DEVICE_MAX];


// void iotb_init(void *arg);
#ifdef __cplusplus
}
#endif

#endif

/*
 * @Description: 
 * @version: 
 * @Author: Adol
 * @Date: 2020-01-28 18:51:13
 * @LastEditTime : 2020-02-02 16:58:27
 */

#include <board.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME "wifi_mqtt"
#define DBG_LEVEL DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#include "paho_mqtt.h"
/**
 * MQTT URI farmat:
 * domain mode
 * tcp://iot.eclipse.org:1883
 *
 * ipv4 mode
 * tcp://192.168.10.1:1883
 * ssl://192.168.10.1:1884
 *
 * ipv6 mode
 * tcp://[fe80::20c:29ff:fe9a:a07e]:1883
 * ssl://[fe80::20c:29ff:fe9a:a07e]:1884
 */
#define MQTT_URI            "tcp://www.wenruihao.com:1883"
#define MQTT_USERNAME       "Device"
#define MQTT_PASSWORD       "Device"
#define MQTT_SUBTOPIC       "Applets"
#define MQTT_PUBTOPIC       "Device"
#define MQTT_WILLMSG        "DEVICE_ONLINE!"

#define enum2str(s)         (#s)

char *mqtt_message_buffer[DEVICE_MAX] =
{
        enum2str(DEVICE_NONE),

        enum2str(APPLETS_ONLINE),
        enum2str(APPLETS_OFFLINE),
        enum2str(DEVICE_ONLINE),
        enum2str(DEVICE_OFFLINE),
        enum2str(DEVICE_DATA),

        enum2str(SWITCH_1_ON),
        enum2str(SWITCH_1_OFF),
        enum2str(SWITCH_2_ON),
        enum2str(SWITCH_2_OFF),
        enum2str(SWITCH_3_ON),
        enum2str(SWITCH_3_OFF)
};

/* define MQTT client context */
static MQTTClient client;
static int mqtt_is_started = 0;

static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub callback: %.*s %.*s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          msg_data->message->payloadlen,
          (char *)msg_data->message->payload);

    if (!(strcmp(((char *)msg_data->message->payload), mqtt_message_buffer[APPLETS_ONLINE])))
    {
        // mqtt_publish(mqtt_senson_data);
    }
    else if (!(strcmp(((char *)msg_data->message->payload), mqtt_message_buffer[DEVICE_DATA])))
    {
        // mqtt_publish(mqtt_senson_data);
    }
    else if (!(strcmp(((char *)msg_data->message->payload), mqtt_message_buffer[SWITCH_1_ON])))
    {
        dev_sensor_relay1_ctl(DEV_SENSOR_OPEN);
        mqtt_publish(mqtt_message_buffer[SWITCH_1_ON]);
    }
    else if (!(strcmp(((char *)msg_data->message->payload), mqtt_message_buffer[SWITCH_1_OFF])))
    {
        dev_sensor_relay1_ctl(DEV_SENSOR_CLOSE);
        mqtt_publish(mqtt_message_buffer[SWITCH_1_OFF]);
    }
    else if (!(strcmp(((char *)msg_data->message->payload), mqtt_message_buffer[SWITCH_2_ON])))
    {
        dev_sensor_relay2_ctl(DEV_SENSOR_OPEN);
        mqtt_publish(mqtt_message_buffer[SWITCH_2_ON]);
    }
    else if (!(strcmp(((char *)msg_data->message->payload), mqtt_message_buffer[SWITCH_2_OFF])))
    {
        dev_sensor_relay2_ctl(DEV_SENSOR_CLOSE);
        mqtt_publish(mqtt_message_buffer[SWITCH_2_OFF]);
    }
    else if (!(strcmp(((char *)msg_data->message->payload), mqtt_message_buffer[SWITCH_3_ON])))
    {
        dev_sensor_beep_ctl(DEV_SENSOR_OPEN);
        mqtt_publish(mqtt_message_buffer[SWITCH_3_ON]);
    }
    else if (!(strcmp(((char *)msg_data->message->payload), mqtt_message_buffer[SWITCH_3_OFF])))
    {
        dev_sensor_beep_ctl(DEV_SENSOR_CLOSE);
        mqtt_publish(mqtt_message_buffer[SWITCH_3_OFF]);
    }
}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub default callback: %.*s %.*s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          msg_data->message->payloadlen,
          (char *)msg_data->message->payload);
}

static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_connect_callback!");
}

static void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_online_callback!");
    rt_kprintf("Publish message: DEVICE_ONLINE to topic: %s\n", MQTT_SUBTOPIC);
    mqtt_publish("DEVICE_ONLINE");
}

static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_offline_callback!");
}

int mqtt_start(void)
{
    /* init condata param by using MQTTPacket_connectData_initializer */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = {0};

    // if (argc != 1)
    // {
    //     rt_kprintf("mqtt_start    --start a mqtt worker thread.\n");
    //     return -1;
    // }

    if (mqtt_is_started)
    {
        LOG_E("mqtt client is already connected.");
        return -1;
    }
    /* config MQTT context param */
    {
        client.isconnected = 0;
        client.uri = MQTT_URI;

        /* generate the random client ID */
        rt_snprintf(cid, sizeof(cid), "%s", MQTT_USERNAME);
        /* config connect param */
        memcpy(&client.condata, &condata, sizeof(condata));
        client.condata.clientID.cstring = cid;
        client.condata.keepAliveInterval = 60;
        client.condata.cleansession = 1;
        client.condata.username.cstring = MQTT_USERNAME;
        client.condata.password.cstring = MQTT_PASSWORD;

        /* config MQTT will param. */
        client.condata.willFlag = 1;
        client.condata.will.qos = 1;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = MQTT_PUBTOPIC;
        client.condata.will.message.cstring = MQTT_WILLMSG;

        /* malloc buffer. */
        client.buf_size = client.readbuf_size = 1024;
        client.buf = rt_calloc(1, client.buf_size);
        client.readbuf = rt_calloc(1, client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            LOG_E("no memory for MQTT client buffer!");
            return -1;
        }

        /* set event callback function */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;

        /* set subscribe table and event callback */
        client.messageHandlers[0].topicFilter = rt_strdup(MQTT_SUBTOPIC);
        client.messageHandlers[0].callback = mqtt_sub_callback;
        client.messageHandlers[0].qos = QOS1;

        /* set default subscribe event callback */
        client.defaultMessageHandler = mqtt_sub_default_callback;
    }

    /* run mqtt client */
    paho_mqtt_start(&client);
    mqtt_is_started = 1;

    return 0;
}

static int mqtt_stop(void)
{
    // if (argc != 1)
    // {
    //     rt_kprintf("mqtt_stop    --stop mqtt worker thread and free mqtt client object.\n");
    // }

    mqtt_is_started = 0;

    return paho_mqtt_stop(&client);
}

int mqtt_publish(const char *send_str)
{
    if (mqtt_is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }

    // if (argc == 2)
    // {
    //     paho_mqtt_publish(&client, QOS1, MQTT_PUBTOPIC, argv[1]);
    // }
    // else if (argc == 3)
    // {
    //     paho_mqtt_publish(&client, QOS1, argv[1], argv[2]);
    // }
    // else
    // {
    //     rt_kprintf("mqtt_publish <topic> [message]  --mqtt publish message to specified topic.\n");
    //     return -1;
    // }

    MQTTMessage message;
    const char *msg_str = send_str;
    const char *topic = MQTT_PUBTOPIC;
    message.qos = QOS1;
    message.retained = 0;
    message.payload = (void *)msg_str;
    message.payloadlen = strlen(message.payload);

    MQTTPublish(&client, topic, &message);

    return 0;
}

// static void mqtt_new_sub_callback(MQTTClient *client, MessageData *msg_data)
// {
//     *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
//     LOG_D("mqtt new subscribe callback: %.*s %.*s",
//                msg_data->topicName->lenstring.len,
//                msg_data->topicName->lenstring.data,
//                msg_data->message->payloadlen,
//                (char *)msg_data->message->payload);
// }

// static int mqtt_subscribe(void)
// {
//     if (argc != 2)
//     {
//         rt_kprintf("mqtt_subscribe [topic]  --send an mqtt subscribe packet and wait for suback before returning.\n");
//         return -1;
//     }

// 	if (mqtt_is_started == 0)
//     {
//         LOG_E("mqtt client is not connected.");
//         return -1;
//     }

//     return paho_mqtt_subscribe(&client, QOS1, argv[1], mqtt_new_sub_callback);
// }

// static int mqtt_unsubscribe(void)
// {
//     if (argc != 2)
//     {
//         rt_kprintf("mqtt_unsubscribe [topic]  --send an mqtt unsubscribe packet and wait for suback before returning.\n");
//         return -1;
//     }

// 	if (mqtt_is_started == 0)
//     {
//         LOG_E("mqtt client is not connected.");
//         return -1;
//     }

//     return paho_mqtt_unsubscribe(&client, argv[1]);
// }

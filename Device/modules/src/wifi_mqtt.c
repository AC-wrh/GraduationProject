
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//#include <wlan_mgnt.h>
//#include "wifi_config.h"
#include "paho_mqtt.h"
#include "wifi_mqtt.h"
//#include "drv_wlan.h"

#define DBG_ENABLE
#define DBG_SECTION_NAME    "MQTT"
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

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
#define MQTT_URI                "tcp://www.wenruihao.com:1883"
#define MQTT_USERNAME           "Device"
#define MQTT_PASSWORD           "Device"
#define MQTT_SUBTOPIC           "Applets"
#define MQTT_PUBTOPIC           "Device"
#define MQTT_WILLMSG            "DEVICE_ONLINE!"

#define enum2str(s)             (#s)

char *wifi_mqtt_message_buffer[DEVICE_MAX] = 
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
    enum2str(SWITCH_3_OFF),

};
/* define MQTT client context */
static MQTTClient client;

static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);

    if(!(strcmp(((char *)msg_data->message->payload), wifi_mqtt_message_buffer[APPLETS_ONLINE])))
	{
		// wifi_mqtt_publish(mqtt_senson_data);
	}
    else if(!(strcmp(((char *)msg_data->message->payload), wifi_mqtt_message_buffer[DEVICE_DATA])))
	{
		// wifi_mqtt_publish(mqtt_senson_data);
	}
    else if(!(strcmp(((char *)msg_data->message->payload), wifi_mqtt_message_buffer[SWITCH_1_ON])))
	{
		wifi_mqtt_publish(wifi_mqtt_message_buffer[SWITCH_1_ON]);
	}
	else if(!(strcmp(((char *)msg_data->message->payload), wifi_mqtt_message_buffer[SWITCH_1_OFF])))
	{
		wifi_mqtt_publish(wifi_mqtt_message_buffer[SWITCH_1_OFF]);
	}
	else if(!(strcmp(((char *)msg_data->message->payload), wifi_mqtt_message_buffer[SWITCH_2_ON])))
	{
		wifi_mqtt_publish(wifi_mqtt_message_buffer[SWITCH_2_ON]);
	}
	else if(!(strcmp(((char *)msg_data->message->payload), wifi_mqtt_message_buffer[SWITCH_2_OFF])))
	{
		wifi_mqtt_publish(wifi_mqtt_message_buffer[SWITCH_2_OFF]);
	}
	else if(!(strcmp(((char *)msg_data->message->payload), wifi_mqtt_message_buffer[SWITCH_3_ON])))
	{
		wifi_mqtt_publish(wifi_mqtt_message_buffer[SWITCH_3_ON]);
	}
	else if(!(strcmp(((char *)msg_data->message->payload), wifi_mqtt_message_buffer[SWITCH_3_OFF])))
	{
		wifi_mqtt_publish(wifi_mqtt_message_buffer[SWITCH_3_OFF]);
	}


    return;
}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub default callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
    return;
}

static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_connect_callback!");
}

static void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_online_callback!");
    rt_kprintf("Publish message: DEVICE_ONLINE to topic: %s\n", MQTT_SUBTOPIC);
    wifi_mqtt_publish("DEVICE_ONLINE");
}

static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_offline_callback!");
}

/**
 * This function create and config a mqtt client.
 *
 * @param void
 *
 * @return none
 */
void wifi_mqtt_start(void)
{
    /* init condata param by using MQTTPacket_connectData_initializer */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = { 0 };

    static int is_started = 0;
    if (is_started)
    {
        return;
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
        client.buf = malloc(client.buf_size);
        client.readbuf = malloc(client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            LOG_E("no memory for MQTT client buffer!");
            goto _exit;
        }

        /* set event callback function */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;

        /* set subscribe table and event callback */
        client.messageHandlers[0].topicFilter = MQTT_SUBTOPIC;
        client.messageHandlers[0].callback = mqtt_sub_callback;
        client.messageHandlers[0].qos = QOS1;

        /* set default subscribe event callback */
        client.defaultMessageHandler = mqtt_sub_default_callback;
    }

    rt_kprintf("Start mqtt client and subscribe topic:%s\n", MQTT_SUBTOPIC);

    /* run mqtt client */
    paho_mqtt_start(&client);
    is_started = 1;

_exit:
    return;
}

/**
 * This function publish message to specific mqtt topic.
 *
 * @param send_str publish message
 *
 * @return none
 */
static void mq_publish(const char *send_str)
{
    MQTTMessage message;
    const char *msg_str = send_str;
    const char *topic = MQTT_PUBTOPIC;
    message.qos = QOS1;
    message.retained = 0;
    message.payload = (void *)msg_str;
    message.payloadlen = strlen(message.payload);

    MQTTPublish(&client, topic, &message);

    return;
}

void wifi_mqtt_publish(const char *send_str)
{
    mq_publish(send_str);

    return;
}

// rt_err_t wifi_mqtt_init()
// {
//     /* Config the dependencies of the wlan autoconnect function */
//     wlan_autoconnect_init();

//     /* Enable wlan auto connect function */
//     rt_wlan_config_autoreconnect(RT_TRUE);

//     if (rt_hw_wlan_init() != RT_EOK)
//     {
//         LOG_E("wlan init failed!");
//         return -RT_ERROR;
//     }
    
//     /* register the wlan ready callback function */
//     rt_wlan_register_event_handler(RT_WLAN_EVT_READY, (void ( *)(int , struct rt_wlan_buff *, void *))mq_start, RT_NULL);

//     return RT_EOK;
// }

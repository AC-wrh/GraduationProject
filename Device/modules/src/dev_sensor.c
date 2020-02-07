/*
 * @Description: sensor device control
 * @version: 1.0
 * @Author: Adol
 * @Date: 2020-01-31 19:37:41
 * @LastEditTime : 2020-02-07 23:24:41
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
    .mq2_status   = DEV_SENSOR_DEINIT,
    .zph02_status = DEV_SENSOR_DEINIT,

    .relay1_status = DEV_SENSOR_DEINIT,
    .relay2_status = DEV_SENSOR_DEINIT,
    .beep_status   = DEV_SENSOR_DEINIT,

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

rt_err_t dev_sensor_xxx_ctl(rt_base_t pin_t, dev_sensor_t status_t)
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

rt_err_t dev_sensor_sht3x_deinit(void)
{
    if (dev_sht3x == RT_NULL)
    {
        return RT_ERROR;
    }

    dev_sensor_data_result.sht3x_status = DEV_SENSOR_BUSY;

    sht3x_deinit(dev_sht3x);
    dev_sht3x = RT_NULL;
    dev_sht3x_inited = 0;

    dev_sensor_data_result.sht3x_status = DEV_SENSOR_IDLE;

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

    dev_sensor_data_result.sht3x_status = DEV_SENSOR_BUSY;

    rt_hw_i2c_init(SHT3X_I2C_BUS_NAME, PIN_I2C1_SCL, PIN_I2C1_SDA);
    dev_sht3x = sht3x_init(SHT3X_I2C_BUS_NAME, SHT3X_ADDR);
    if (dev_sht3x == RT_NULL)
    {
        LOG_E("The sht3x sensor initializes failure!");
        dev_sht3x_inited = 0;
        return RT_ERROR;
    }

    dev_sensor_data_result.sht3x_status = DEV_SENSOR_IDLE;
    dev_sht3x_inited = 1;

    LOG_I("The sht3x sensor initializes success!");

    return RT_EOK;
}

/**
 * 3. mq2 sensor
*/
#include "drv_adc.h"

static uint8_t dev_mq2_inited = 0;

rt_err_t dev_sensor_mq2_init(void)
{
    if (dev_mq2_inited)
    {
        return RT_EOK;
    }

    dev_sensor_data_result.mq2_status = DEV_SENSOR_BUSY;
    MX_ADC1_Init();
    dev_sensor_data_result.mq2_status = DEV_SENSOR_IDLE;
    dev_mq2_inited = 1;

    LOG_I("mq2 device init success!");

    return RT_EOK;
}
/**
 * 4. zph02 sensor
*/
rt_device_t dev_zph02;                /* 串口设备句柄 */
struct rt_semaphore dev_zph02_rx_sem; /* 用于接收消息的信号量 */
rt_uint8_t dev_zph02_inited = 0;
rt_uint8_t dev_zph02_value_buffer[9];
rt_uint8_t dev_zph02_data_high;
rt_uint8_t dev_zph02_data_low;

struct serial_configure dev_zph02_uart_config = RT_SERIAL_CONFIG_DEFAULT; /* 初始化配置参数 */

static rt_err_t dev_zph02_value_check()
{
    rt_uint8_t i, check_sum = 0;

    for (i = 1; i < 8; i++)
    {
        check_sum += dev_zph02_value_buffer[i];
    }

    if (((check_sum ^ 0xff) + 1) == dev_zph02_value_buffer[i])
    {
        dev_zph02_data_high = dev_zph02_value_buffer[3];
        dev_zph02_data_low = dev_zph02_value_buffer[4];
        dev_sensor_data_result.zph02_status = DEV_SENSOR_IDLE;
        return RT_EOK;
    }
    else
    {
        return RT_ERROR;
    }
}

/* 接收数据回调函数 */
static rt_err_t dev_sensor_zph02_callback(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&dev_zph02_rx_sem);

    return RT_EOK;
}

static void dev_sensor_zph02_thread(void *parameter)
{
    static rt_int8_t ch;
    static rt_uint8_t count = 0;

    while (1)
    {
        /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        while (rt_device_read(dev_zph02, -1, &ch, 1) != 1)
        {
            /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
            rt_sem_take(&dev_zph02_rx_sem, RT_WAITING_FOREVER);
        }
        /* 读取到的数据通过串口错位输出 */
        // ch = ch + 1;
        // rt_device_write(dev_zph02, 0, &ch, 1);
        if (dev_sensor_data_result.zph02_status == DEV_SENSOR_IDLE)
        {
            if (ch == 0xff)
            {
                count = 0;
            }

            dev_zph02_value_buffer[count] = ch;
            count++;

            if (count == 9)
            {
                dev_sensor_data_result.zph02_status = DEV_SENSOR_BUSY;
                count = 0;
                dev_zph02_value_check();
            }
        }
    }
}

rt_err_t dev_sensor_zph02_init()
{
    if (dev_zph02_inited)
    {
        return RT_EOK;
    }
    dev_sensor_data_result.zph02_status = DEV_SENSOR_BUSY;

    /* step1：查找串口设备 */
    dev_zph02 = rt_device_find(ZPH02_UART_NAME); //
    if (!dev_zph02)
    {
        LOG_E("zph02 init failed!\n");
        return RT_ERROR;
    }

    /* step2：修改串口配置参数 */
    dev_zph02_uart_config.baud_rate = BAUD_RATE_9600; //修改波特率为 9600
    dev_zph02_uart_config.data_bits = DATA_BITS_8;    //数据位 8
    dev_zph02_uart_config.stop_bits = STOP_BITS_1;    //停止位 1
    dev_zph02_uart_config.bufsz = 128;                //修改缓冲区 buff size 为 128
    dev_zph02_uart_config.parity = PARITY_NONE;       //无奇偶校验位

    /* step3：控制串口设备。通过控制接口传入命令控制字，与控制参数 */
    rt_device_control(dev_zph02, RT_DEVICE_CTRL_CONFIG, &dev_zph02_uart_config);

    /* 初始化信号量 */
    rt_sem_init(&dev_zph02_rx_sem, "dev_zph02_rx_sem", 0, RT_IPC_FLAG_FIFO);

    /* step4：打开串口设备。以中断接收及轮询发送模式打开串口设备 */
    rt_device_open(dev_zph02, RT_DEVICE_FLAG_INT_RX);

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(dev_zph02, dev_sensor_zph02_callback);

    /* 创建 dev_zph02 线程 */
    rt_thread_t thread = rt_thread_create("zph02", dev_sensor_zph02_thread, RT_NULL, 1024, 25, 10);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        LOG_E("zph02 thread failed!\n");
        return RT_ERROR;
    }

    dev_sensor_data_result.zph02_status = DEV_SENSOR_IDLE;
    dev_zph02_inited = 1;

    return RT_EOK;
}

/**
 * 5. sensor data 
*/
char dev_sensor_all[100];
char dev_sensor_data[100];

void dev_sensor_mqtt_buffer(void)
{
    if (dev_sensor_data_result.status)
    {
        rt_sprintf(dev_sensor_all, "D$S-A_%d.%d_%d.%d_%d_%d.%d_%d_%d_%d_%d\n", (int)dev_sensor_data_result.sht3x_data_temp,
                                                                                (int)(dev_sensor_data_result.sht3x_data_temp * 10) % 10,
                                                                                (int)dev_sensor_data_result.sht3x_data_humi,
                                                                                (int)(dev_sensor_data_result.sht3x_data_humi * 10) % 10,
                                                                                (int)dev_sensor_data_result.mq2_data,
                                                                                (int)dev_sensor_data_result.zph02_data / 10,
                                                                                (int)dev_sensor_data_result.zph02_data % 10,
                                                                                (int)dev_sensor_data_result.relay1_status,
                                                                                (int)dev_sensor_data_result.relay2_status,
                                                                                (int)dev_sensor_data_result.beep_status);

        rt_sprintf(dev_sensor_data, "D$S-D_%d.%d_%d.%d_%d_%d.%d\n", (int)dev_sensor_data_result.sht3x_data_temp,
                                                                    (int)(dev_sensor_data_result.sht3x_data_temp * 10) % 10,
                                                                    (int)dev_sensor_data_result.sht3x_data_humi,
                                                                    (int)(dev_sensor_data_result.sht3x_data_humi * 10) % 10,
                                                                    (int)dev_sensor_data_result.mq2_data,
                                                                    (int)dev_sensor_data_result.zph02_data / 10,
                                                                    (int)dev_sensor_data_result.zph02_data % 10);
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

    dev_sensor_data_result.status = DEV_SENSOR_BUSY;
    dev_sensor_data_upload(&dev_sensor_data_result, &sensor_data);

    if (dev_sht3x_inited)
    { /* sht3x */
        dev_sensor_data_result.sht3x_status = DEV_SENSOR_BUSY;
        sht3x_read_singleshot(dev_sht3x);
        sensor_data.sht3x_data_temp = dev_sht3x->temperature;
        sensor_data.sht3x_data_humi = dev_sht3x->humidity;
        dev_sensor_data_result.sht3x_status = DEV_SENSOR_IDLE;
    }
    else
    {
        LOG_E("sht3x init failed! Retry...");
        rt_thread_mdelay(2000);
        if (dev_sensor_data_result.sht3x_status == DEV_SENSOR_IDLE)
        {
            dev_sensor_sht3x_init();
        }
    }

    if (dev_mq2_inited)
    { /* mq2 */
        dev_sensor_data_result.mq2_status = DEV_SENSOR_BUSY;
        sensor_data.mq2_data = get_adc_value();
        dev_sensor_data_result.mq2_status = DEV_SENSOR_IDLE;
    }
    else
    {
        LOG_E("mq2 init failed! Retry...");
        rt_thread_mdelay(2000);
        if (dev_sensor_data_result.mq2_status == DEV_SENSOR_IDLE)
        {
            dev_sensor_mq2_init();
        }
    }

    if (dev_zph02_inited)
    {
        sensor_data.zph02_data = ((dev_zph02_data_high * 100) + dev_zph02_data_low);
    }
    else
    {
        LOG_E("zph02 init failed! Retry...");
        rt_thread_mdelay(2000);
        if (dev_sensor_data_result.zph02_status == DEV_SENSOR_IDLE)
        {
            dev_sensor_zph02_init();
        }
    }

    dev_sensor_data_upload(&sensor_data, &dev_sensor_data_result);
    dev_sensor_data_result.status = DEV_SENSOR_IDLE;
}

static void _dev_sensor_read_thr(void *arg)
{
    while (1)
    {
        dev_sensor_data_read();
        rt_thread_mdelay(1);
        dev_sensor_mqtt_buffer();
        rt_thread_mdelay(1000);
    }
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
    dev_sensor_sht3x_init();
    dev_sensor_mq2_init();
    dev_sensor_zph02_init();
    rt_thread_mdelay(500);
    mqtt_start();
    rt_thread_mdelay(500);
    dev_sensor_read_start();

    return RT_EOK;
}

'use strict'

import mqtt from '../../utils/mqtt.js'

const host = 'wxs://www.wenruihao.com/mqtt'

var mqttInit = 0
var timer
var message
var messageBuffer = new Array()

Page({

  /**
   * 页面的初始数据
   */
  data: {
    client: null,
    //记录重连的次数
    //reconnectCounts: 0,
    Sht3x_Temperature: "温度：-- ℃",
    Sht3x_HumiditySensor: "湿度：-- %",
    Mq2_Value: "天然气浓度：--",
    Zph02_Value: "PM2.5：--",
    Relay1_Status: "---",
    Relay2_Status: "---",
    Beep_Status: "---",

    //MQTT连接的配置
    options: {
      // protocolVersion: 4, //MQTT连接协议版本
      // clientId: 'Applets',
      // clean: true,
      // password: 'Applets',
      // username: 'Applets',
      // reconnectPeriod: 1000, //1000毫秒，两次重新连接之间的间隔
      // connectTimeout: 30 * 1000, //1000毫秒，两次重新连接之间的间隔
      // resubscribe: true //如果连接断开并重新连接，则会再次自动订阅已订阅的主题（默认true）
      // keepalive: 10,
      clientId: 'Applets',
      protocolVersion: 4,
      clean: true,
      reconnectPeriod: 1000,
      connectTimeout: 30 * 1000,
      // will: {
      //   topic: 'Device',
      //   payload: 'Connection Closed abnormally..!',
      //   qos: 0,
      //   retain: false
      // },
      username: 'Applets',
      password: 'Applets',
      rejectUnauthorized: false
    }
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var that = this;

    //开始连接
    this.data.client = mqtt.connect(host, this.data.options)
    this.data.client.on('connect', function (connack) {
      wx.showToast({
        title: '服务连接成功'
      })
    })
    mqttInit = 1
    this.data.client.publish('Applets', 'APPLETS_ONLINE', { qos: 0, retain: false })

    timer = setInterval(function () {
      that.data.client.publish('Applets', 'DEVICE_DATA', { qos: 0, retain: false })
    }, 2000)

    //接收到订阅主题的消息的回调函数
    that.data.client.on("message", function (topic, payload) {
      /*
      console.log( topic + " publish a message: " + payload )
      wx.showModal({
        content: topic + " publish a message: " + payload,
        showCancel: false,
      })*/
      message = String(payload)
      console.log("messageBuffer value: " + message)

      messageBuffer = message.split("_")
      console.log("0:" + messageBuffer[0],
        "1:" + messageBuffer[1],
        "2:" + messageBuffer[2],
        "3:" + messageBuffer[3],
        "4:" + messageBuffer[4],
        "5:" + messageBuffer[5],
        "6:" + messageBuffer[6],
        "7:" + messageBuffer[7],
        "8:" + messageBuffer[8])

      switch (message) {

        case "DEVICE_ONLINE":
          that.data.client.publish('Applets', 'DEVICE_DATA', { qos: 0, retain: false })
          console.log("DEVICE_ONLINE")
          break

        case "SWITCH_1_ON":
          that.setData({ Switch1Status: messageBuffer[2] })
          console.log("SWITCH_1_ON")
          break

        case "SWITCH_1_OFF":
          that.setData({ Switch1Status: messageBuffer[2] })
          console.log("SWITCH_1_OFF")
          break

        case "SWITCH_2_ON":
          that.setData({ Switch2Status: messageBuffer[2] })
          console.log("SWITCH_2_ON")
          break

        case "SWITCH_2_OFF":
          that.setData({ Switch2Status: messageBuffer[2] })
          console.log("SWITCH_2_OFF")
          break

        case "SWITCH_3_ON":
          that.setData({ Switch3Status: messageBuffer[2] })
          console.log("SWITCH_3_ON")
          break

        case "SWITCH_3_OFF":
          that.setData({ Switch3Status: messageBuffer[2] })
          console.log("SWITCH_3_OFF")
          break

        default:
          that.setData({ TemperatureSensor: messageBuffer[2] + "℃" })
          that.setData({ HumiditySensor: messageBuffer[3] + "%" })
          that.setData({ LightSensor: messageBuffer[4] + "lux" })
          that.setData({ GasSensor: messageBuffer[5] + "ps" })
          that.setData({ Switch1Status: messageBuffer[6] })
          that.setData({ Switch2Status: messageBuffer[7] })
          that.setData({ Switch3Status: messageBuffer[8] })
          console.log("default")
          break
      }
    })

    //服务器连接异常的回调
    that.data.client.on("error", function (error) {
      console.log("服务器 error 的回调" + error)

    })

    //服务器重连连接异常的回调
    that.data.client.on("reconnect", function () {
      console.log("服务器 reconnect的回调")
    })

    //服务器连接异常的回调
    that.data.client.on("offline", function (errr) {
      console.log("服务器 offline 的回调")

    })

    that.data.client.subscribe('Device', { qos: 1 })
  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {
    
  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {
    console.log("进入")
    
  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {
    console.log("隐藏")
    clearInterval(timer)
  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
    console.log("退出")
    this.data.client.publish('Applets', 'APPLETS_OFFLINE', { qos: 0, retain: false })
    clearInterval(timer)
  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {
    
  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {
    
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
    
  },

  /**
   * 定时器事件
   */
  timer_callback: function () {
    this.data.client.publish('Applets', 'DEVICE_DATA', { qos: 0, retain: false })
  },



  /**
   * 按键点击事件
   */
  onClick_switch1on: function () {
    //switch1开启
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'SWITCH_1_ON', { qos: 0, retain: false })
      wx.showToast({
        title: '发布成功'
      })
    } else {
      wx.showToast({
        title: '请先连接服务器',
        icon: 'none',
        duration: 2000
      })
    }
    //this.setData({ Switch1Status: 'on' })
  },

  onClick_switch1off: function () {
    //switch1关闭
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'SWITCH_1_OFF', { qos: 0, retain: false })
      wx.showToast({
        title: '发布成功'
      })
    } else {
      wx.showToast({
        title: '请先连接服务器',
        icon: 'none',
        duration: 2000
      })
    }
  },

  onClick_switch2on: function () {
    //switch2开启
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'SWITCH_2_ON', { qos: 0, retain: false })
      wx.showToast({
        title: '发布成功'
      })
    } else {
      wx.showToast({
        title: '请先连接服务器',
        icon: 'none',
        duration: 2000
      })
    }
  },

  onClick_switch2off: function () {
    //switch2关闭
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'SWITCH_2_OFF', { qos: 0, retain: false })
      wx.showToast({
        title: '发布成功'
      })
    } else {
      wx.showToast({
        title: '请先连接服务器',
        icon: 'none',
        duration: 2000
      })
    }
  },

  onClick_switch3on: function () {
    //switch3开启
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'SWITCH_3_ON', { qos: 0, retain: false })
      wx.showToast({
        title: '发布成功'
      })
    } else {
      wx.showToast({
        title: '请先连接服务器',
        icon: 'none',
        duration: 2000
      })
    }
  },

  onClick_switch3off: function () {
    //switch3关闭
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'SWITCH_3_OFF', { qos: 0, retain: false })
      wx.showToast({
        title: '发布成功'
      })
    } else {
      wx.showToast({
        title: '请先连接服务器',
        icon: 'none',
        duration: 2000
      })
    }
  }

  
})

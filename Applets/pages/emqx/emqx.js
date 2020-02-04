'use strict'

import mqtt from '../../utils/mqtt.js'

const host = 'wxs://www.wenruihao.com/mqtt'

var timer_flag = 0
var timer_count = 0
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
    sht3x_temp_value: "-- ℃",
    sht3x_humi_value: "-- %",
    mq2_value: "--",
    zph02_value: "--",
    relay1_status: "---",
    relay2_status: "---",
    beep_status: "---",

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
      //   qos: 1,
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
        title: '设备连接成功'
      })
    })

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
        "7:" + messageBuffer[7])

      switch (message) {

        case "DEVICE_ONLINE":
          that.data.client.publish('Applets', 'DEVICE_SENSOR_ALL', { qos: 1, retain: false })
          console.log("DEVICE_ONLINE")
          break

        case "RELAY_1_ON":
          that.setData({ relay1_status: messageBuffer[2] })
          console.log("RELAY_1_ON")
          break

        case "RELAY_1_OFF":
          that.setData({ relay1_status: messageBuffer[2] })
          console.log("RELAY_1_OFF")
          break

        case "RELAY_2_ON":
          that.setData({ relay2_status: messageBuffer[2] })
          console.log("RELAY_2_ON")
          break

        case "RELAY_2_OFF":
          that.setData({ relay2_status: messageBuffer[2] })
          console.log("RELAY_2_OFF")
          break

        case "BEEP_ON":
          that.setData({ beep_status: messageBuffer[1] })
          console.log("BEEP_ON")
          break

        case "BEEP_OFF":
          that.setData({ beep_status: messageBuffer[1] })
          console.log("BEEP_OFF")
          break

        default:
          if (messageBuffer[0] === "D$S-A") {
            that.setData({ sht3x_temp_value: messageBuffer[1] + "℃" })
            that.setData({ sht3x_humi_value: messageBuffer[2] + "%" })
            that.setData({ mq2_value: messageBuffer[3] + "lux" })
            that.setData({ zph02_value: messageBuffer[4] + "ps" })

            if (messageBuffer[5] === "2") {
              that.setData({ relay1_status: "ON" })
            } else {
              that.setData({ relay1_status: "OFF" })
            }

            if (messageBuffer[6] === "2") {
              that.setData({ relay2_status: "ON" })
            } else {
              that.setData({ relay2_status: "OFF" })
            }

            if (messageBuffer[7] === "2") {
              that.setData({ beep_status: "ON" })
            } else {
              that.setData({ beep_status: "OFF" })
            }
          }

          if (messageBuffer[0] === "D$S-D") {
            that.setData({ sht3x_temp_value: messageBuffer[1] + "℃" })
            that.setData({ sht3x_humi_value: messageBuffer[2] + "%" })
            that.setData({ mq2_value: messageBuffer[3] + "lux" })
            that.setData({ zph02_value: messageBuffer[4] + "ps" })
          }
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
    var that = this

    that.data.client.publish('Applets', 'DEVICE_SENSOR_ALL', { qos: 1, retain: false })
    timer_flag = setInterval(function () {
      timer_count++
      if (timer_count === 5) {
        that.data.client.publish('Applets', 'DEVICE_SENSOR_ALL', { qos: 1, retain: false })
        timer_count = 0
      } else {
        that.data.client.publish('Applets', 'DEVICE_SENSOR_DATA', { qos: 1, retain: false })
      }
    }, 2000)
    
  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {
    console.log("隐藏")
    clearInterval(timer_flag)
    timer_count = 0
  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
    console.log("退出")
    var that = this
    clearInterval(timer_flag)
    this.data.client.publish('Applets', 'APPLETS_OFFLINE', { qos: 1, retain: false })
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
   * 按键点击事件
   */
  onClick_relay1_on: function () {
    //switch1开启
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'RELAY_1_ON', { qos: 1, retain: false })
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

  onClick_relay1_off: function () {
    //switch1关闭
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'RELAY_1_OFF', { qos: 1, retain: false })
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

  onClick_relay2_on: function () {
    //switch2开启
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'RELAY_2_ON', { qos: 1, retain: false })
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

  onClick_relay2_off: function () {
    //switch2关闭
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'RELAY_2_OFF', { qos: 1, retain: false })
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

  onClick_beep_on: function () {
    //switch3开启
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'BEEP_ON', { qos: 1, retain: false })
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

  onClick_beep_off: function () {
    //switch3关闭
    if (this.data.client && this.data.client.connected) {
      this.data.client.publish('Applets', 'BEEP_OFF', { qos: 1, retain: false })
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

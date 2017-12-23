// start.js

Page({
  data: {
  },
  
  onPullDownRefresh: function () {
      console.log('onPullDownRefresh', new Date())
  },

  getDataFromOneNet: function(){
    //从oneNET请求我们的Wi-Fi气象站的数据
    const requestTask = wx.request({
      url: 'https://api.heclouds.com/devices/20493222/datapoints?datastream_id=Cmtong,Cmren&limit=15',
      header: {
        'content-type': 'application/json',
        'api-key': 'IqsdqcPQRPnJhnk6VLHlQhTuFFE='
      },
      success: function (res) {
        //console.log(res.data)
        //拿到数据后保存到全局数据
        var app = getApp()
        app.globalData.cmtong = res.data.data.datastreams[0]
        app.globalData.cmren= res.data.data.datastreams[1]
        //app.globalData.humidity = res.data.data.datastreams[2]
        console.log(app.globalData.cmren)//????????????????????????????????????????????????
        //跳转到天气页面，根据拿到的数据绘图
        wx.navigateTo({
          url: '../tianqi/tianqi',
        })
      },

      fail: function (res) {
        console.log("fail!!!")
      },

      complete: function (res) {
        console.log("end")
      }
    })
  },
  
  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
  
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
  
  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {
  
  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
  
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
  
  }
})

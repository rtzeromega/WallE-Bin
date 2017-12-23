var myCharts = require("../../utils/wxcharts.js")//引入一个绘图的插件
var lineChart_ct = null
var lineChart_cr = null
var app = getApp()

Page({
  data: {
  },
  onPullDownRefresh: function () {
    console.log('onPullDownRefresh', new Date())
  },


  //把拿到的数据转换成绘图插件需要的输入格式
  convert: function () {
    var categories = [];
    var ct = [];
    var cr = [];
    //var tempe = [];

    var length = app.globalData.cmren.datapoints.length
    for (var i = 0; i < length; i++) {
      categories.push(app.globalData.cmtong.datapoints[i].at.slice(11, 19));
      ct.push(app.globalData.cmtong.datapoints[i].value);
      cr.push(app.globalData.cmren.datapoints[i].value);
      //tempe.push(app.globalData.temperature.datapoints[i].value);
    }
    return {
      categories: categories,
      cr: cr,
      ct: ct,
      //tempe: tempe
    }
  },

  onLoad: function () {
    var wheatherData = this.convert();

    //得到屏幕宽度
    var windowWidth = 320;
    try {
      var res = wx.getSystemInfoSync();
      windowWidth = res.windowWidth;
    } catch (e) {
      console.error('getSystemInfoSync failed!');
    }

    var wheatherData = this.convert();

    //新建桶距图标
    lineChart_ct = new myCharts({
      canvasId: 'ct',
      type: 'line',
      categories: wheatherData.categories,
      animation: true,
      background: '#f5f5f5',
      series: [{
        name: 'cmtong',
        data: wheatherData.ct,
        format: function (val, name) {
          return val.toFixed(2);
        }
      }],
      xAxis: {
        disableGrid: true
      },
      yAxis: {
        title: '桶距 (cm)',
        format: function (val) {
          return val.toFixed(2);
        },
        min: 0
      },
      width: windowWidth,
      height: 200,
      dataLabel: false,
      dataPointShape: true,
      extra: {
        lineStyle: 'curve'
      }
    });

    //新建温度图表
    lineChart_cr = new myCharts({
      canvasId: 'cr',
      type: 'line',
      categories: wheatherData.categories,
      animation: true,
      background: '#f5f5f5',
      series: [{
        name: 'cmren',
        data: wheatherData.cr,
        format: function (val, name) {
          return val.toFixed(2);
        }
      }],
      xAxis: {
        disableGrid: true
      },
      yAxis: {
        title: '人距(cm)',
        format: function (val) {
          return val.toFixed(2);
        },
        min: 0
      },
      width: windowWidth,
      height: 200,
      dataLabel: false,
      dataPointShape: true,
      extra: {
        lineStyle: 'curve'
      }
    });
  },


})

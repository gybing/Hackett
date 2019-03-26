// Device.js
// 
const app = require('electron').remote.app;


var Device = {

  // 用户信息
  userdata: {},

  // 服务器设置
  setting: {},

  // 设备令牌
  device_token: '',

  // cookie
  cookie: null,

  setDeviceData : function(data, cookie)
  {
    this.cookie = cookie;
    this.device_token = this.getTokenFromCookie(cookie);

    var set = {};
    Array.prototype.forEach.call(data.setting, function (o) {
      set[o.key] = o.value;
    });
    this.setting = set;

    //console.log(data.setting);
    this.userdata = data;
  },

  setDeviceToken: function(token) {
    this.device_token = token;
  },

  getDeviceToken: function() { return this.device_token; },
  getDeviceType: function() { return 2; },
  getDeviceID: function() { return '596aa89f68895cd6e9cd2ae07bbf65e8'; },
  getVersion: function() { return app.getVersion(); },

  // 从cookie中获取token
  getTokenFromCookie: function(cookie) {
    for (var i in cookie) {
      if (cookie[i]['key'] == 'sc1') {
        return cookie[i]['value'];
      }
    }
  },
};

module.exports = Device;
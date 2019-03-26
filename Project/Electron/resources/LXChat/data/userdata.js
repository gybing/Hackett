// UserData.js
// 

const request = require('../comm/request');

var UserData = function() {

  this.userdata = {};

  this.getUserData = function(uid, callback){
    var u = this.userdata[uid];
    if (u != undefined) {
      if (arguments[1]);
        callback(u);
      return u;
    } else {
      var _this = this;
      request('http://api.lejiaolexue.com/rest/userinfo/full/' + uid, false, function (error, data) {
        if (data.result != 0) {
          console.log('取用户信息错误:' + data.msg);
          return;
        }

        _this.userdata[uid] = data.uig[0];
        if (arguments[1]);
          callback(_this.userdata[uid]);
        return _this.userdata[uid];
      });
    }
  }
};

module.exports = new UserData();

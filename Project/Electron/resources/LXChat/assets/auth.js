const electron = require('electron')
const path = require('path')
const request = require('../comm/request');
const ipcRenderer = require('electron').ipcRenderer;
const storage = require('electron-json-storage');
const device = require('../comm/device');

document.getElementById("button-header-close").addEventListener('click', function (event) {
  ipcRenderer.send('message-close', "success");
})

document.onkeydown = function () {
  if(window.event.keyCode == 13) {
    loginButtonClick();
  }
};

document.getElementById("button-auth-login").addEventListener('click', function (event) {
  loginButtonClick();
})

function loginButtonClick() {
  var username= document.getElementById("input-phone").value;
  var password= document.getElementById("input-pwd").value;
  loginByPassword(username, password);
}

function loginByPassword(u, p){
  var url = "http://id.lejiaolexue.com/sso/login.ashx" + "?uid=" + u + "&pwd=" + p + "&device_type=" + device.getDeviceType() + "&device_name=" + device.getDeviceID();
  request(url, true, function(err, data, token) {
    if (data.result != 0){
      console.log(body + ' 登录失败: ' + data.msg);
      return;
    }

    storage.set('storage-user-name', u);
    storage.set('storage-user-token', token);
    storage.set('storage-data-auth-data', data);
    ipcRenderer.send('message-login', "success");
  });
}

function loginByToken(u, t){
  var url = "http://id.lejiaolexue.com/sso/login.ashx" + "?uid=" + u + "&user_token=" + t + "&mode=1" + "&device_type=" + device.getDeviceType() + "&device_name=" + device.getDeviceID();
  request(url, false, function(err, data) {
    if (data.result != 0){
      console.log(data + ' 登录失败: ' + data.msg);
      return;
    }

    storage.set('storage-data-auth-data', data);
    ipcRenderer.send('message-login', "success");
  });
}

storage.get('storage-user-name', function (err, user_name) {
  if (err) return console.error(err)
  if (user_name && user_name.length) {

    document.getElementById("input-phone").value=user_name;

    storage.get('storage-user-token', function (err, user_token) {
    if (err) return console.error(err)
      if (user_token && user_token.length) {
        loginByToken(user_name, user_token);
      }
    })
  }
})


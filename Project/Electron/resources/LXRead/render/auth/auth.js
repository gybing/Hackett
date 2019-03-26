const electron = require('electron')
const path = require('path')
const request = require('request');
const ipcRenderer = require('electron').ipcRenderer;
const storage = require('electron-json-storage')

document.getElementById("button-header-close").addEventListener('click', function (event) {
  ipcRenderer.send('message-close', "success");
})

function login(u, t){
  var url = "http://i8.hanwenbook.com/i.ashx" + "?reqid=120&shelfno=" + u + "&lxtoken=" + t + "&deviceid=O6kxDxUgp%2BCxiye9fnRm0uJM";
  var j = request.jar();
  request({url: url, jar: j}, function (error, response, body) {
    if (!error && response.statusCode == 200) {
      var cookies = j.getCookies(url);
      var data = eval('(' + body + ')');
      if (data.error != 0){
        console.log(body + ' url: ' + url);
        return;
      }

      // console.log(cookies);
      // console.log(body);
      storage.set('storage-user-token', '&uid=' + u + '&' + 'token=' + t);
      storage.set('storage-data-auth-cookies', cookies);
      storage.set('storage-data-auth-data', data.data);
      ipcRenderer.send('message-login', "success");
    }
  })
}

function GetQueryString(url, name) {
  var reg = new RegExp("(^|&)?"+ name +"=([^&]*)(&|$)");
  var r = url.substr(1).match(reg);
  if(r!=null)return unescape(r[2]); return null;
}

const view = document.getElementById("content-auth-view");
view.addEventListener('did-get-redirect-request', function (e) {
  if (e != undefined && e['newURL'] != undefined) {
    var uid = GetQueryString(e['newURL'], "uid");
    var token = GetQueryString(e['newURL'], "token");
    if (!uid || !token) return;
    login(uid, token);
  }
})

storage.get('storage-user-token', function (err, token_cache) {
  if (err) return console.error(err)
  if (token_cache && token_cache.length) {
    var uid = GetQueryString(token_cache, "uid");
    var token = GetQueryString(token_cache, "token");
    if (!uid || !token) return;
    login(uid, token);
  }
})


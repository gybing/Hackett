// request.js
// 
function Request(u, savecookie, callback) {
  const request = require('request');
  const storage = require('electron-json-storage');

  if (!savecookie) {
    storage.get('storage-auth-cookies', function (err, coo) {
      var j = request.jar();
      Array.prototype.forEach.call(coo, function (cookie) {
        j.setCookie(request.cookie(cookie['key'] + '=' + cookie['value']), u);
      });
      request({url: u, jar: j}, function (error, response, body) {
        if (!error && response.statusCode == 200) {
          callback(error, eval('(' + body + ')'));
        }
      })
    })
  } else {
    var j = request.jar();
    request({url: u, jar: j}, function (error, response, body) {
      if (!error && response.statusCode == 200) {
        var cookie = j.getCookies(u);
        var token = require('./device').getTokenFromCookie(cookie);
        storage.set('storage-auth-cookies', cookie);
        callback(error, eval('(' + body + ')'), token);
      }
    })
  }
}

module.exports = Request;
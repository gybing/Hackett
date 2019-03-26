const storage = require('electron-json-storage')
const request = require('request');
const common = require('../../data/common')
const ipcRenderer = require('electron').ipcRenderer;

var cookies;
var userinfo;
var userCollectType;
var userCollectBook;

storage.get('storage-data-auth-cookies', function (err, coo) {
  cookies = coo;
  storage.get('storage-data-auth-data', function (err, data) {
    userinfo = data.userinfo;
  
    SetTitleUserInfo();
    LoadUserCollectType();
    LoadUserCollectBook();
  })
})

function SetTitleUserInfo(){
  document.getElementById("label-shelf-native-title").innerHTML = userinfo.name;
  document.getElementById("label-shelf-native-icon").src = 'http://image.lejiaolexue.com/userlogo/'+ userinfo.shelfno + '/3';
}

function SetTitleUserCollectCount(cnt){
  document.getElementById("label-shelf-native-count").innerHTML = '藏书: ' + cnt + '本';
}

function LoadUserCollectType(){
  var url = common.URL_AjaxInterface + "?reqid=301";
  var j = request.jar();
  Array.prototype.forEach.call(cookies, function (coo) {
    var c = request.cookie(coo['key'] + '=' + coo['value']);
    //alert(c);
    j.setCookie(c, url);
  });
  request({url: url, jar: j}, function (error, response, body) {
    if (!error && response.statusCode == 200) {
      var data = eval('(' + body + ')');
      if (data.error != 0) {
          return;
      };
      if (data.busierror == 300) {
          // 学生不能访问老师的的藏书
          alert("不能访问老师的的藏书");
          return;
      };
      userCollectType = [{ "id": 0, "name": "未分类", "books": [] }];
      var cnt = data.data.tags.length;      
      if (cnt > 0) {
          for (var i = 0; i < cnt; i++) {
              var tag = data.data.tags[i];
              var tagBook = {};
              tagBook.id = tag.id;
              tagBook.name = tag.name;
              tagBook.books = [];
              userCollectType.push(tagBook);
          }
      };

      SetCollectTypeList();
    }
  })
}

function SetCollectTypeList(){
  var content = '';
  Array.prototype.forEach.call(userCollectType, function (type, index) {
    content += '<li class="native-shelf-list-li">' + type.name + '</li>';
  })
  document.getElementById("ul-shelf-list").innerHTML = content;
}

function LoadUserCollectBook(){
  var url = common.URL_AjaxInterface + "?reqid=302";
  var j = request.jar();
  Array.prototype.forEach.call(cookies, function (coo) {
    var c = request.cookie(coo['key'] + '=' + coo['value']);
    //alert(c);
    j.setCookie(c, url);
  });
  request({url: url, jar: j}, function (error, response, body) {
    if (!error && response.statusCode == 200) {
      var data = eval('(' + body + ')');
      if (data.error != 0) {
          return;
      };
      if (data.busierror == 300) {
          // 学生不能访问老师的的藏书
          alert("不能访问老师的的藏书");
          return;
      };
      var cnt = data.data.books.length;
      userCollectBook = [];
      SetTitleUserCollectCount(cnt);
      if (cnt > 0) {
        for (var i = 0; i < cnt; i++) {
          var book = data.data.books[i];
          userCollectBook = data.data.books;
        };

        ipcRenderer.sendToHost('message-open-book', '123');
      }
    }
  })
}

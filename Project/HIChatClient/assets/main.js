(function() {
  'use strict'
  const storage = require('electron-json-storage')
  const ipcRenderer = require('electron').ipcRenderer;
  const request = require('../comm/request');
  const device = require('../comm/device');
  const message = require('../comm/message');
  const datetime = require('../comm/datetime');
  const userdata = require('../data/userdata');
  const socket = require('../comm/socket');
  const protocol = require('../comm/protocol');

  var menus = document.getElementById("main-toobar-ul").getElementsByTagName('li');
  var views = document.getElementById("main-native-div").getElementsByTagName('ul');
  var contents = [
    document.getElementById("main-chat-view-div"),
    document.getElementById("main-contact-view-div"),
    document.getElementById("main-app-view-div")
  ];

  // 初始化
  document.getElementById("button-header-close").addEventListener('click', function (event) {
    ipcRenderer.send('message-close', "success");
  })
  document.getElementById("button-header-min").addEventListener('click', function (event) {
    ipcRenderer.send('message-minsize', "success");
  })

  Array.prototype.forEach.call(menus, function (btn, index) {
    btn.addEventListener('click', function (event) {
      activeNativeView(index);
      storage.set('storage-main-native-active', index, function (err) {
        if (err) return console.error(err)
      })
    })
  })

  storage.get('storage-main-native-active', function (err, id) {
    let act = 0;
    if (err) {
      console.error(err);
    }
    else if (id && id.length) {
        act = id;
    }
    activeNativeView(act);
  })

  storage.get('storage-data-auth-data', function (err, data) {
    storage.get('storage-auth-cookies', function (err, cookie) {
      device.setDeviceData(data, cookie);
      mainprocess();
    });
  });

  // 函数方法
  // main
  function mainprocess() {
    // 用户信息
    document.getElementById("main-toobar-info-title").innerHTML = device.userdata.uname;
    document.getElementById("main-toobar-info-icon").src = 'http://image.lejiaolexue.com/userlogo/'+ device.userdata.uid + '/3';
  
    // 取应用列表
    loadAppList();

    // 取好友列表
    loadContactList();

    // 聊天记录
    loadChatList();
  };

  function activeNativeView(idx){
    if (!menus[idx].classList.contains('is-selected')){
      for (var i = 0; i < 3; i++) {
        if (i == idx) {
          menus[i].classList.add('is-selected')
          views[i].classList.add('is-shown')
          contents[i].classList.add('is-shown')
        } else {
          menus[i].classList.remove('is-selected')
          views[i].classList.remove('is-shown')
          contents[i].classList.remove('is-shown')
        }
      }
    }
  }

  function setChatContent(view, message){
    if (!message || message==null || message==undefined)
      return;

    var uid = message.target;
    userdata.getUserData(uid, function(user){
      document.getElementById("main-chat-view-title-title").innerHTML = (user.remark && user.remark!='null' ? user.remark : user.uname);
      document.getElementById("main-chat-view-title-text").innerHTML = ((user.remark && user.remark!='null') ? user.uname : '');
      document.getElementById("main-chat-view-title-icon").src = 'http://image.lejiaolexue.com/userlogo/'+ user.uid + '/3';
    });
  	
    const showTimeDelay = 300; // 秒
    var lastChatTime = 0;
  	function getChatBubble(msg){
      var me = (msg.from === device.userdata.uid);
      var body = JSON.parse(msg.body);
      var avatar = 'http://image.lejiaolexue.com/userlogo/'+ msg.from + '/3';

      var html = '<li class="chat-item">' +
                  '<div class="chat-profile">';
      if (msg.ts - lastChatTime > showTimeDelay) {
        html += '<div class="chat-status">'+datetime.getDateTime(msg.ts)+'</div>';
      }
      lastChatTime = msg.ts;
      
      var date = new Date(parseInt(msg.ts) * 1000).toLocaleString().replace(/:\d{1,2}$/,' ');

      html += '<div class="chat-profile-info">' +
                      '<span class="select-username name-text' + (me ? ' name-text-me' : '')+'" uid="'+msg.from+'"></span>' +
                      '<span class="chat-time' + (me ? ' chat-time-me' : '')+'">'+date+'</span>' + 
                  '</div></div>' +
                  '<img class="avatar' + (me ? ' avatar-me' : '')+'" src="'+avatar+'"/>' +
                  '<div class="chat-content' + (me ? ' chat-content-me' : '')+'">' +
                '<div class="msg-bubble">';

      if (body.Type === 'text') {
        html += '<pre class="text' + (me ? ' text-me' : '')+'">'+body.Text+'</pre>';
      } else if (body.Type === 'image') {
        if (body.action === 1) {
          html += '<img class="text' + (me ? ' text-me' : '')+'" height="50px" width="50px" src="http://'+device.setting['servers.imfile.download']+'/rest/dl/'+body.Name+'">'+'<p>'+body.title+'</p>'+'</img>';
        } else if (body.action === 2) {
          html += '<img class="text' + (me ? ' text-me' : '')+'" height="50px" width="50px" src="http://'+device.setting['servers.imfile.download']+'/rest/dl/'+body.Name+'">'+'<p>'+body.title+'</p>'+'</img>';
        } else {
          html += '<img class="text' + (me ? ' text-me' : '')+'" src="http://'+device.setting['servers.imfile.download']+'/rest/dl/'+body.Name+'"/>';
        }
      } else if (body.Type === 'voice') {
        html += '<pre class="text' + (me ? ' text-me' : '')+'">'+body.Text+'</pre>';
      }

      html += '</div></div></li>';

  		var item = document.createElement("div"); 
  		item.innerHTML = html;

  		return item.childNodes[0];
  	};

  	var view = document.getElementById('chat-view-ul');
  	view.innerHTML = '';

  	Array.prototype.forEach.call(message.list, function (msg) {
      view.appendChild(getChatBubble(msg));
  	});

    // 更新用户名
    const sels = document.querySelectorAll('.select-username')
    Array.prototype.forEach.call(sels, function (sel) {
      userdata.getUserData(sel.getAttribute('uid'), function(user){
        sel.innerHTML=user.uname;
      });
    })

    view.scrollTop = view.scrollHeight;
  }

  function setContactContent(view, uid){
    
  }
  function setAppContent(view, appid){
    
  }

  // 加载聊天记录
  function loadChatList(){
    function getChatItem(image, uid, time, text){
    	var item = document.createElement("div"); 
  		item.innerHTML = '<li>' +
                '<i class="delete iconfont" htitle="删除会话"></i>' +
                '<img class="icon" src="'+image+'"/>' +
                '<div class="item-content">' +
                  '<p class="title select-username" uid="'+uid+'"></p>' +
                  '<p class="time">'+time+'</p>' +
                  '<p class="text">'+text+'</p>' +
                '</div>' +
              '</li>';
        return item.childNodes[0];
    };

    message.onChange = function(){
      var view = document.getElementById('native-chat-ul');
   	  view.innerHTML = '';

      Array.prototype.forEach.call(message.messages, function (msg) {
        var m = msg.list[msg.list.length-1];
        var body = JSON.parse(m.body);
        var item = getChatItem('http://image.lejiaolexue.com/userlogo/'+ msg.target + '/3',
          msg.target,
          datetime.getDateTime(m.ts),
          body.Text
        );
        view.appendChild(item);

        item.addEventListener('click', function (event) {
          setChatContent(contents[0], msg);
          Array.prototype.forEach.call(view.getElementsByTagName('li'), function (it, idx) {
  	        if (it == item) {
  	          it.classList.add('is-selected')
  	        } else {
  	          it.classList.remove('is-selected')
  	        }
  	      });
          storage.set('storage-chat-target', msg);
        });
      });

      const sels = document.querySelectorAll('.select-username');
      Array.prototype.forEach.call(sels, function (sel) {
        userdata.getUserData(sel.getAttribute('uid'), function(user){
          sel.innerHTML=user.uname;
        });
      });

      storage.get('storage-chat-target', function (err, msg) {
        if (err) {
          setChatContent(contents[0], message.messages[0]);
          return console.log(err);
        } else {
          Array.prototype.forEach.call(message.messages, function (ms) {
            if (ms.target === msg.target) {
              setChatContent(contents[0], ms);
            }
          });
        }
      });
    };

    message.load();

    socket.connect(device.setting['servers.lexin.im'], function(msg){
      message.append(msg);
    });

    // 发送按钮点击事件
    document.getElementById('send-message-button').addEventListener('click', function (event) {
      const ta = document.getElementById('msg-box-textarea');
      var text = ta.value;
      if (text && text!=undefined && text.length) {
        storage.get('storage-chat-target', function (err, msg) {
        if (err) return console.log(err);

        var body = {};
        body['Type'] = 'text';
        body['Text'] = text;

        var data = {};
        data['zone_id'] = msg.zone?msg.target:0;
        data['from'] = device.userdata.uid;
        data['to'] = msg.target;
        data['body'] = JSON.stringify(body);
        data['cb_id'] = Math.round(Math.random()*100000000000000000);
        if (socket.send(msg.zone?protocol.ProtocolType.CHAT_ZONE_SPEAK:protocol.ProtocolType.CHAT_SPEAK, JSON.stringify(data))) {
          ta.value = '';
        }
      });
      }
    });
  };

  // 加载好友列表
  function loadContactList(){
    const view = document.getElementById('native-contact-ul');
    view.innerHTML = '';

    view.innerHTML += getContactItem('./images/contact_top_friend.png', '新的朋友');
    view.innerHTML += getContactItem('./images/contact_top_zone.png', '群组');
    view.innerHTML += getContactItem('./images/contact_top_class.png', '我的班级');
    view.innerHTML += getContactItem('./images/contact_top_group.png', '标签');
    view.innerHTML += getContactItem('./images/contact_top_mutlisend.png', '群发助手');
    view.innerHTML += getContactTitle('联系人');

    function getContactItem(image, title, detail) {
      return  '<li>' +
                '<img class="icon" src="'+image+'"/>' +
                '<div class="item-content">' +
                  '<p class="title">'+ title +'</p>' +
                  '<p class="text">'+ (detail!=undefined?detail:'') +'</p>' +
                '</div>' +
              '</li>';
    }

    function getContactTitle(title) {
      return  '<li class="main-native-item-title">' +
                '<p class="title">'+ title +'</p>' +
              '</li>';
    }

    request('http://api.lejiaolexue.com/rest/userinfo/closefriend2.ashx', false, function (error, data) {
      if (data.result != 0) {
        console.log(' 取好友列表列表: ' + data.msg);
        return;
      }

      Array.prototype.forEach.call(data.uis, function (user) {
        if (true) {
          view.innerHTML += getContactItem('http://image.lejiaolexue.com/userlogo/'+ user.uid + '/3', 
            (user.remark && user.remark!='null' ? user.remark : user.uname), 
            (user.remark && user.remark!='null' ? '('+user.uname+')' : ''));
        }
      });

      Array.prototype.forEach.call(view.getElementsByTagName('li'), function (item, index) {
        item.addEventListener('click', function (event) {
          setContactContent(contents[2], data.uis[index]);
        })
      })

    });
  };

  // 加载app列表
  function loadAppList(){
    const view = document.getElementById('native-application-ul');
    view.innerHTML = '';

    request('http://api.lejiaolexue.com/rest/user/subscribeapp/2', false, function (error, data) {
      if (data.result != 0) {
        console.log(' 取应用列表: ' + data.msg);
        return;
      }

      function getAppItem(image, title){
        return  '<li>' + 
                  '<img class="icon" src="' + image +'"/>'+
                  '<div class="item-content">'+
                    '<p class="title">' + title + '</p>'+
                  '</div>'+
                '</li>';
      }

      Array.prototype.forEach.call(data.app, function (app) {
        if (true) {
          view.innerHTML += getAppItem('http://image.lejiaolexue.com/applogo/'+ app.app_id + '_v16.png',
            app.app_name);
        }
      });

      Array.prototype.forEach.call(view.getElementsByTagName('li'), function (item, index) {
        item.addEventListener('click', function (event) {
          setAppContent(contents[2], data.app[index]);
        })
      })
    });
  };
})();
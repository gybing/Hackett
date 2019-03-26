// Message.js
// 
// message [{
//  target: 149880;
//  me: 1;
//  list: [
//    msg: {};
//  ];
// }]
//

const device = require('./device');
const storage = require('electron-json-storage');

var Message = function(){
  this.messages = [];

  this.onChange = null;
  this.change = function(){
    this.save();
    if (this.onChange)
      this.onChange();
  };

  this.append = function(msg){
    var me = (msg.from == device.userdata.uid);
    var zone = msg.zone_id != 0;
    var target = (zone ? msg.zone_id : (me ? msg.to : msg.from));

    // 查找已有聊天记录，添加
    for (x in this.messages)
    {
      if (this.messages[x].target == target) {
        // 排重
        for (m in this.messages[x].list){
          if (this.messages[x].list[m].cb_id == msg.cb_id)
            return false;
        }

        this.messages[x].list.push(msg);
        this.messages.unshift(this.messages.splice(x, 1)[0]);
        this.change();
        return true;
      }
    }

    // 未找到聊天记录，新建
    var chat = {'target': target, 'zone': zone, 'list':[msg]};
    this.messages.unshift(chat);
    this.change();
  };

  this.remove = function(target){
    for (x in messages)
    {
      if (this.messages[x].target == target) {
        this.messages.splice(x, 1);
        this.change();
        return;
      }
    }
  };

  this.load = function(){
    var _this = this;
    storage.get('storage-data-chat-messages', function(err, data){
      _this.messages = JSON.parse(data);
      if (_this.onChange)
        _this.onChange();
    });
  };

  this.save = function(){
    storage.set('storage-data-chat-messages', JSON.stringify(this.messages));
  };
};

module.exports = new Message();
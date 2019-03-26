// request.js
// 
// 协议格式 contentlength=4bytes | signal(string) | 0x00 | body(string) | 0x00
// 
function IMSocket() {
  const net = require('net');
  const protocol = require('./protocol')
  
  this.messagecallback = null;
  this.keepalive = null;
  this.socket = new net.Socket();

  this.connect = function(host, callbask) {
    var _this = this;

    this.messagecallback = callbask;
    this.socket.connect(host.split(':')[1], host.split(':')[0], function() {
      _this.send(protocol.ProtocolType.USER_LOGIN, protocol.getLoginData());
      keepalive = self.setInterval(function(){
        _this.send(protocol.ProtocolType.PING);
      },60 * 1000);

      console.log('CONNECTED IM TO: ' + host);
    });

    this.socket.setKeepAlive(true, 60*1000);

    // 为客户端添加“data”事件处理函数
    // data是服务器发回的数据
    this.socket.on('data', function(data) {
      var __this = _this;
      protocol.parseMessage(data, function(proto, msg){
        if (proto === protocol.ProtocolType.USER_LOGIN_ACKS) {
          if (msg.result != 0) {
            alert('IM服务器连接失败:' + msg.msg);
          }
          return;
        }

        if (proto == protocol.ProtocolType.CHAT_MESSAGE_ARRIVED) {
          __this.send(proto + 'AckC', '{cb_id:'+msg.cb_id+'}'); // 发送ACK
          __this.messagecallback(msg);
          // console.log(msg);
        }
        else if (proto === protocol.ProtocolType.NET_CLOSE) {
          __this.socket.destroy();
        }
      });
    });
  }

  // 为客户端添加“close”事件处理函数
  this.socket.on('close', function() {
    clearInterval(this.keepalive);
    console.log('Connection closed');
  });

  // 发送消息
  this.send = function(proto, data) {
    if (!arguments[1]) data = '';
    var ret = this.socket.write(protocol.getProtocolMessage(proto, data));
    if (ret && (proto == protocol.ProtocolType.CHAT_SPEAK || proto == protocol.ProtocolType.CHAT_ZONE_SPEAK)) {
      this.messagecallback(JSON.parse(data));
    }
    return ret;
  };
}

module.exports = new IMSocket();

// protocol.js
// 
// IM 消息协议
// 

const device = require('./device');

// 协议类型定义
exports.ProtocolType = {
  PING: "Ping",
  DISCOVERY_QUERYSERVER: "Discovery.QueryServer",
  DISCOVERY_QUERYSERVER_ACKS: "Discovery.QueryServerAckS",
  USER_LOGIN: "User.Login",
  USER_LOGIN_ACKS: "User.LoginAckS",
  CHAT_SPEAK: "Chat.Speak",
  CHAT_SPEAK_ACKS: "Chat.SpeakAckS",
  CHAT_ZONE_SPEAK: "Chat.ZoneSpeak",
  CHAT_MESSAGE_ARRIVED: "Chat.MessageArrived",
  CHAT_MESSAGE_ARRIVED_ACKC: "Chat.MessageArrivedAckC",
  NET_CLOSE: "Net.Close",
  PUSH_NOTIFICATION: "Push.Notification",
  PUSH_NOTIFICATION_ACKC: "Push.NotificationAckC",
  PUSH_NOTIFICATION_AGENT: "Push.NotificationAgent",
  PUSH_NOTIFICATION_AGENT_ACKS: "Push.NotificationAgentAckS",
}

exports.messageData = new Buffer(0);

exports.parseMessage = function(data, callback){

  this.messageData = Buffer.concat([this.messageData, data], this.messageData.length + data.length);

  for (;this.messageData.length > 1;) {
    var pos = 0;
    var len = this.messageData.readInt32BE(pos);
    var start = pos + 4;
    var pos = start + len;

    if (pos > this.messageData.length)
      return;

    var buf = this.messageData.slice(start, pos - 1);
    this.messageData = this.messageData.slice(pos);

    var msg = buf.toString();
    var index = msg.indexOf('{');
    callback(msg.substr(0, index-1), JSON.parse(msg.substr(index)));
  }

};

exports.getProtocolMessage = function(proto, data){
  var len = 6 + data.length + proto.length;
  var buf = new Buffer(len);
  var pos = buf.writeUInt32BE(len-4);
  pos += buf.write(proto, pos);
  pos = buf.writeUInt8(0x00, pos);
  pos += buf.write(data, pos, len-pos-1);
  pos = buf.writeUInt8(0x00, pos);
  return buf;
};

exports.getLoginData = function(){
  var data = {};
  data['token'] = device.device_token;
  data['device_id'] = device.getDeviceType();
  data['device_name'] = device.getDeviceID();
  data['version'] = parseInt(device.getVersion());
  return JSON.stringify(data);
};
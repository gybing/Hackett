
var minute = 60;
var hour = minute * 60;
var day = hour * 24;
var halfamonth = day * 15;
var month = day * 30;

exports.getDateTime = function (dateTimeStamp){
  var now = Math.round(new Date().getTime()/1000);
  var diffValue = now - dateTimeStamp + (8 * 3600);

  if(diffValue < 0){
    return "刚刚";
  }

  var monthC =diffValue/month;
  var weekC =diffValue/(7*day);
  var dayC =diffValue/day;
  var hourC =diffValue/hour;
  var minC =diffValue/minute;

  if(monthC>=12){
    result= parseInt(monthC/12) + "年前";
  }
  else if(monthC>=1){
    result= parseInt(monthC) + "月前";
  }
  else if(weekC>=1){
    result= parseInt(weekC) + "周前";
  }
  else if(dayC>=1){
    result= parseInt(dayC) +"天前";
  }
  else if(hourC>=1){
    result= parseInt(hourC) +"小时前";
  }
  else if(minC>=1){
    result= parseInt(minC) +"分钟前";
  }else
    result="刚刚";

  return result;
};
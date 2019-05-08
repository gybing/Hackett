import 'dart:async';
import 'package:cookie_jar/cookie_jar.dart';
import 'package:dio/dio.dart';

typedef CallBack = void Function();

class Network {
  final dio = Dio(BaseOptions(
    baseUrl: "http://144.34.183.114",
    connectTimeout: 5000,
    receiveTimeout: 3000,
  ));

  Network() {
    dio.interceptors.add(CookieManager(PersistCookieJar()));
  }

  // 登陆
  Future<dynamic> login({String mobile, String pass}) {
    return httpGet('/Auth/Login',
        params: {'Mobile': '$mobile', 'Pass': '$pass', 'Device': '0'});
  }

  // 退出
  Future<dynamic> logout() {
    return httpGet('/Auth/Logout');
  }

  // 注册
  Future<dynamic> regist({String mobile, String name, String pass}) {
    return httpGet('/Auth/Regist', params: {
      'Mobile': '$mobile',
      'Pass': '$pass',
      'Device': '0',
      'Name': '$name',
      'Face': ''
    });
  }

  // 用户
  Future<dynamic> getUserInfo({String id: ""}) {
    return httpGet('/User/GetUserInfo', params: {'ID': id});
  }

  // 好友
  Future<dynamic> contactRequest({String id: ""}) {
    return httpGet('/Contact/Request', params: {'ContactID': id});
  }

  Future<dynamic> contactProcess({String id: "", String type: ""}) {
    return httpGet('/Contact/Process', params: {'ContactID': id, "Type": type});
  }

  Future<dynamic> contactRemove({String id: ""}) {
    return httpGet('/Contact/Remove', params: {'ContactID': id});
  }

  Future<dynamic> getContactList() {
    return httpGet('/Contact/GetContactList');
  }

  Future<dynamic> getRequestList({String type: ""}) {
    return httpGet('/Contact/GetRequestList', params: {"Type": type});
  }

  Future<dynamic> httpGet(String url, {Map<String, dynamic> params}) async {
    return await dio.get(url, queryParameters: params).then((v) {
      if (v.data["Code"] != 0) {
        return Future.error(v.data["Message"]);
      }
      return v.data["Data"];
    }).catchError((e) {
      return Future.error(e.toString());
    });
  }
}

Network api = Network();

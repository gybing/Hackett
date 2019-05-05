import 'dart:async';
import 'dart:convert';
import 'dart:io';

typedef CallBack = void Function();

class Network {
  static const String host = '144.34.183.114';
  final _httpClient = new HttpClient();

  // 认证
  Future<dynamic> login({String mobile, String pass}) {
    return _requestGet(Uri.http(
      host,
      '/Auth/Login',
      <String, String>{'Mobile': '$mobile', 'Pass': '$pass', 'Device': '0'},
    ));
  }

  Future<dynamic> logout() {
    return _requestGet(Uri.http(
      host,
      '/Auth/Logout',
      <String, String>{},
    ));
  }

  Future<dynamic> regist({String mobile, String name, String pass}) {
    return _requestGet(Uri.http(
      host,
      '/Auth/Regist',
      <String, String>{
        'Mobile': '$mobile',
        'Pass': '$pass',
        'Device': '0',
        'Name': '$name',
        'Face': ''
      },
    ));
  }

// 用户
  Future<dynamic> getUserInfo({String id: ""}) {
    return _requestGet(Uri.http(
      host,
      '/User/GetUserInfo',
      <String, String>{'ID': id},
    ));
  }

  // 好友
  Future<dynamic> contactRequest({String id: ""}) {
    return _requestGet(Uri.http(
      host,
      '/Contact/Request',
      <String, String>{'ContactID': id},
    ));
  }

  Future<dynamic> contactProcess({String id: "", String type: ""}) {
    return _requestGet(Uri.http(
      host,
      '/Contact/Process',
      <String, String>{'ContactID': id, "Type": type},
    ));
  }

  Future<dynamic> contactRemove({String id: ""}) {
    return _requestGet(Uri.http(
      host,
      '/Contact/Remove',
      <String, String>{'ContactID': id},
    ));
  }

  Future<dynamic> getContactList() {
    return _requestGet(Uri.http(
      host,
      '/Contact/GetContactList',
      <String, String>{},
    ));
  }

  Future<dynamic> getRequestList({String type: ""}) {
    return _requestGet(Uri.http(
      host,
      '/Contact/GetRequestList',
      <String, String>{"Type": type},
    ));
  }

  Future<dynamic> _requestGet(Uri uri) async {
    var request = await _httpClient.getUrl(uri);
    var response = await request.close();
    var responseBody = await response.transform(utf8.decoder).join();
    var data = json.decode(responseBody);

    if (data["Code"] != 0) {
      return Future.error(data["Message"]);
    }

    return data["Data"];
  }
}

Network api = Network();

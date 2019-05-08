import 'package:flutter/material.dart';
import 'package:locate/module/module.dart';
import 'package:locate/network/network.dart';

class UserBloc with ChangeNotifier {
  String id;
  UserInfo data;

  UserBloc(String id) {
    this.id = id;
    getUserInfo(id: this.id);
  }

  Future<dynamic> getUserInfo({String id: ""}) async {
    notifyListeners();

    return Future.value(await api.getUserInfo(id: id).then((u) {
      this.data = UserInfo.fromJson(u);
    }).whenComplete(() {
      notifyListeners();
    }));
  }
}
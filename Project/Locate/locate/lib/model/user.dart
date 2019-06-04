import 'package:flutter/material.dart';
import 'package:locate/data/struct.dart';
import 'package:locate/data/network.dart';

class UserBloc extends ChangeNotifier {
  String id;
  UserInfo data;

  Future<dynamic> getUserInfo(String id) async {
    this.id = id;
    data = UserInfo.fromJson(await api.getUserInfo(id: id));
    notifyListeners();
  }
}

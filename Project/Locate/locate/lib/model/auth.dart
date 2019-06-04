import 'package:flutter/material.dart';
import 'package:locate/data/struct.dart';
import 'package:locate/data/network.dart';

class AuthBloc extends ChangeNotifier {
  AuthBloc() {
    this.current = null;
    this.isloading = false;
    this.isregist = false;
  }

  UserInfo current;
  bool isloading;
  bool isregist;

  bool isLogin() {
    return current != null;
  }

  Future<dynamic> login({String mobile: "", String pass: ""}) async {
    this.isloading = true;
    notifyListeners();

    return Future.value(await api.login(mobile: mobile, pass: pass).then((u) {
      this.current = UserInfo.fromJson(u);
    }).whenComplete(() {
      this.isloading = false;
      notifyListeners();
    }));
  }

  void changeRegist(bool regist) {
    if (this.isregist != regist) {
      this.isregist = regist;
      notifyListeners();
    }
  }

  Future<dynamic> regist(
      {String mobile: "", String name: "", String pass: ""}) async {
    this.isloading = true;
    notifyListeners();

    return Future.value(
        await api.regist(mobile: mobile, name: name, pass: pass).then((u) {
      this.current = UserInfo.fromJson(u);
      isregist = false;
    }).whenComplete(() {
      this.isloading = false;
      notifyListeners();
    }));
  }

  Future<dynamic> logout() async {
    if (isLogin()) {
      this.current = null;
      await api.logout();
      notifyListeners();
    }
  }
}

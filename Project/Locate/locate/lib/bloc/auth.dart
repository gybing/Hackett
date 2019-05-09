import 'package:flutter/material.dart';
import 'package:locate/module/module.dart';
import 'package:locate/network/network.dart';

class AuthBloc with ChangeNotifier {
  UserInfo current;
  bool isloading;
  bool isregist;

  AuthBloc() {
    this.current = null;
    this.isloading = false;
    this.isregist = false;
  }

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

  void logout() async {
    if (isLogin()) {
      this.current = null;
      await api.logout();
      notifyListeners();
    }
  }
}

import 'package:flutter/material.dart';
import 'package:locate/module/module.dart';
import 'package:locate/network/network.dart';

class ContactBloc with ChangeNotifier {
  List<ContactInfo> contacts;
  List<ContactInfo> requests;
  List<ContactInfo> receives;

  ContactBloc() {
    this.contacts = null;
    this.requests = null;
    this.receives = null;
  }

  Future<dynamic> getContactList() async {
    return Future.value(await api.getContactList().then((cs) {
      this.contacts = cs.map((c) => ContactInfo.fromJson(c));
    }).catchError((e) {

      var v = e;
    }).whenComplete(() {
      notifyListeners();
    }));
  }

  Future<dynamic> getRequestList({String type: ""}) async {
    return Future.value(await api.getRequestList(type: type).then((cs) {
      if (type == "1") {
        this.requests = cs.map((c) => ContactInfo.fromJson(c));
      } else if (type == "2") {
        this.receives = cs.map((c) => ContactInfo.fromJson(c));
      }
    }).whenComplete(() {
      notifyListeners();
    }));
  }

  Future<dynamic> request({String id: ""}) async {
    return Future.value(await api.contactRequest(id: id).whenComplete(() {
      getRequestList(type: "1");
      notifyListeners();
    }));
  }

  Future<dynamic> remove({String id: ""}) async {
    return Future.value(await api.contactRemove(id: id).whenComplete(() {
      getContactList();
      getRequestList(type: "1");
      getRequestList(type: "2");
      notifyListeners();
    }));
  }

  Future<dynamic> process({String id: "", String type: ""}) async {
    return Future.value(
        await api.contactProcess(id: id, type: type).whenComplete(() {
      getContactList();
      getRequestList(type: "2");
      notifyListeners();
    }));
  }
}

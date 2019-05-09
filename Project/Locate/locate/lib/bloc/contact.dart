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

  void getContactList() async {
    notifyListeners();
    this.contacts = (await api.getContactList())
        ?.map<ContactInfo>((c) => ContactInfo.fromJson(c))
        .toList();
    notifyListeners();
  }

  void getRequestList({String type: ""}) async {
    notifyListeners();
    this.requests = (await api.getRequestList())
        ?.map<ContactInfo>((c) => ContactInfo.fromJson(c))
        .toList();
    notifyListeners();
  }

  void request({String id: ""}) async {
    await api.contactRequest(id: id).whenComplete(() {
      getRequestList(type: "1");
      notifyListeners();
    });
  }

  void remove({String id: ""}) async {
    await api.contactRemove(id: id).whenComplete(() {
      getContactList();
      getRequestList(type: "1");
      getRequestList(type: "2");
      notifyListeners();
    });
  }

  void process({String id: "", String type: ""}) async {
    await api.contactProcess(id: id, type: type).whenComplete(() {
      getContactList();
      getRequestList(type: "2");
      notifyListeners();
    });
  }
}

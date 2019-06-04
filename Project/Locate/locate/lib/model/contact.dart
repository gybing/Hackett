import 'package:flutter/material.dart';
import 'package:locate/data/struct.dart';
import 'package:locate/data/network.dart';

class ContactBloc extends ChangeNotifier {
  List<ContactInfo> contacts;

  List<ContactInfo> requests;

  List<ContactInfo> receives;

  ContactBloc() {
    this.contacts = null;
    this.requests = null;
    this.receives = null;
  }

  Future<dynamic> getContactList() async {
    this.contacts = (await api.getContactList())
        .map<ContactInfo>((c) => ContactInfo.fromJson(c))
        .toList();
    notifyListeners();
  }

  Future<dynamic> getRequestList({String type: ""}) async {
    this.requests = (await api.getRequestList())
        .map<ContactInfo>((c) => ContactInfo.fromJson(c))
        .toList();
    notifyListeners();
  }

  Future<dynamic> request({String id: ""}) async {
    await api.contactRequest(id: id).whenComplete(() {
      getRequestList(type: "1");
      notifyListeners();
    });
  }

  Future<dynamic> remove({String id: ""}) async {
    await api.contactRemove(id: id).whenComplete(() {
      getContactList();
      getRequestList(type: "1");
      getRequestList(type: "2");
      notifyListeners();
    });
  }

  Future<dynamic> process({String id: "", String type: ""}) async {
    await api.contactProcess(id: id, type: type).whenComplete(() {
      getContactList();
      getRequestList(type: "2");
      notifyListeners();
    });
  }
}

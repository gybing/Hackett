import 'dart:io';
import 'package:flutter/services.dart';
import 'package:flutter/material.dart';
import 'package:locate/view/view.dart';
import 'package:locate/model/model.dart';

void main() {
  final providers = Providers()
     ..provide(Provider.function((context) => AuthBloc()))
     ..provide(Provider.function((context) => ContactBloc()))
     ..provide(Provider.withFactory((context) => Provider.function((context) => UserBloc())));

  runApp(ProviderNode(child: LocateApp(), providers: providers));

  // 设置android状态栏为透明
  if (Platform.isAndroid) {
    SystemUiOverlayStyle systemUiOverlayStyle =
        SystemUiOverlayStyle(statusBarColor: Colors.transparent);
    SystemChrome.setSystemUIOverlayStyle(systemUiOverlayStyle);
  }
}

class LocateApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(home: ContactView(), routes: {
      "/StartView": (context) => StartView(),
      "/LoginView": (context) => LoginView(),
      "/HomeView": (context) => HomeView(),
      "/ContactView": (context) => ContactView(),
    });
  }
}

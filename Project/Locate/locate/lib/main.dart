import 'dart:io';
import 'package:flutter/services.dart';
import 'package:flutter/material.dart';
import 'package:provide/provide.dart';
import 'package:locate/bloc/bloc.dart';
import 'package:locate/view/view.dart';

void main() {
  final providers = Providers()
    ..provide(Provider.function((context) => AuthBloc()))
    ..provide(Provider.function((context) => ContactBloc()))
    ..provideValue(UserBloc);

  runApp(ProviderNode(
    providers: providers,
    child: LocateApp(),
  ));

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

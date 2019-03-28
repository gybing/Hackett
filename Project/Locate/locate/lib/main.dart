import 'package:flutter/material.dart';
import 'package:locate/bloc/bloc.dart';

import 'package:locate/view/home.dart';
import 'package:locate/view/login.dart';
import 'package:locate/view/splash.dart';
import 'package:locate/view/loading.dart';

void main() {
  BlocSupervisor().delegate = MainBlocDelegate();
  runApp(LocateApp(authStore: AuthStore()));
}

class LocateApp extends StatefulWidget {
  final AuthStore authStore;

  LocateApp({Key key, @required this.authStore}) : super(key: key);

  @override
  State<LocateApp> createState() => _LocateAppState();
}

class _LocateAppState extends State<LocateApp> {
  AuthBloc _authBloc;
  AuthStore get _authStore => widget.authStore;

  @override
  void initState() {
    _authBloc = AuthBloc(authStore: _authStore);
    _authBloc.dispatch(AppStartEvent());
    super.initState();
  }

  @override
  void dispose() {
    _authBloc.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return BlocProvider<AuthBloc>(
      bloc: _authBloc,
      child: MaterialApp(
        home: BlocBuilder<BaseEvent, AuthState>(
          bloc: _authBloc,
          builder: (BuildContext context, AuthState state) {
            if (state is AuthenticationUninitialized) {
              return SplashPage();
            }
            if (state is AuthenticationAuthenticated) {
              return HomePage();
            }
            if (state is AuthenticationUnauthenticated) {
              return LoginPage(authStore: _authStore);
            }
            if (state is AuthenticationLoading) {
              return LoadingIndicator();
            }
          },
        ),
      ),
    );
  }
}

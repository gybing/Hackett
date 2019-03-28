import 'package:flutter/material.dart';
import 'package:locate/bloc/bloc.dart';

class LoginPage extends StatefulWidget {
  final AuthStore authStore;

  LoginPage({Key key, @required this.authStore})
      : assert(authStore != null),
        super(key: key);

  @override
  State<LoginPage> createState() => _LoginPageState();
}

class _LoginPageState extends State<LoginPage> {
  AuthBloc _authBloc;

  AuthStore get _authStore => widget.authStore;

  @override
  void initState() {
    _authBloc = BlocProvider.of<AuthBloc>(context);
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Login'),
      ),
      body: LoginForm(
        authBloc: _authBloc,
      ),
    );
  }
}

class LoginForm extends StatefulWidget {
  final AuthBloc authBloc;

  LoginForm({
    Key key,
    @required this.authBloc,
  }) : super(key: key);

  @override
  State<LoginForm> createState() => _LoginFormState();
}

class _LoginFormState extends State<LoginForm> {
  final _usernameController = TextEditingController();
  final _passwordController = TextEditingController();

  AuthBloc get _authBloc => widget.authBloc;

  @override
  Widget build(BuildContext context) {
    return BlocBuilder<BaseEvent, AuthState>(
      bloc: _authBloc,
      builder: (BuildContext context, AuthState state) 
      {
        if (state is LoginFailure) {
          _onWidgetDidBuild(() {
            Scaffold.of(context).showSnackBar(
              SnackBar(
                content: Text('${state.error}'),
                backgroundColor: Colors.red,
              ),
            );
          });
        }

        return Container(
          padding: const EdgeInsets.all(55),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              InputWiget(
                text: '用户名',
                obscure: false,
                controller: _usernameController),
              SizedBox(height: 20,),
              InputWiget(
                text: '密码',
                obscure: true,
                controller: _passwordController),
              SizedBox(height: 30,),
              LoginButtonWiget(
                onPressed: state is! LoginLoading ? _onLoginButtonPressed : null,
                text: '登录',
              ),
              Container(
                child: state is LoginLoading ? CircularProgressIndicator() : null,
              ),
            ],
          ),
        );
      },
    );
  }

  void _onWidgetDidBuild(Function callback) {
    WidgetsBinding.instance.addPostFrameCallback((_) {
      callback();
    });
  }

  _onLoginButtonPressed() {
    _authBloc.dispatch(LoginEvent(
      username: _usernameController.text,
      password: _passwordController.text,
    ));
  }
}

// 输入框
class InputWiget extends StatelessWidget {
  final String text;
  final bool obscure;
  final TextEditingController controller;

  InputWiget({
    Key key,
    @required this.text,
    @required this.obscure,
    @required this.controller
    }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return new SizedBox(
      child: new Container(
        padding: EdgeInsets.fromLTRB(20, 2, 8, 2),
        decoration: BoxDecoration(
          borderRadius: BorderRadius.circular(30),
          color: Colors.black12,
        ),
        alignment: Alignment.center,
        child: TextField(
          maxLines: 1,
          obscureText: this.obscure,
          controller: this.controller,
          decoration: InputDecoration(
            hintText: this.text,
            border: InputBorder.none,
          ),
        ),
      ),
    );
  }
}


// 登录按钮
class LoginButtonWiget extends StatelessWidget {
  final VoidCallback onPressed;
  final String text;

  LoginButtonWiget({
    Key key,
    @required this.text,
    @required this.onPressed
    }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return new RaisedButton(
      onPressed: this.onPressed,
      child: new Container(
        padding: EdgeInsets.fromLTRB(2, 15, 2, 15),
        decoration: BoxDecoration(
          borderRadius: BorderRadius.circular(30),
          color: Colors.deepOrangeAccent,
        ),
        alignment: Alignment.center,
        child: Text(
          this.text,
          textAlign: TextAlign.center,
          style: TextStyle(letterSpacing: 20,
          fontWeight: FontWeight.bold,
          color: Colors.white70),
        )
      ),
    );
  }
}
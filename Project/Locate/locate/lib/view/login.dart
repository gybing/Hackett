import 'package:flutter/material.dart';
import 'package:provide/provide.dart';
import 'package:locate/bloc/bloc.dart';

class LoginView extends StatefulWidget {
  @override
  State<StatefulWidget> createState() {
    return new LoginViewState();
  }
}

class LoginViewState extends State<LoginView> {
  final textMobile = TextEditingController();
  final textName = TextEditingController();
  final textPass = TextEditingController();

  @override
  Widget build(BuildContext context) {
    return Provide<AuthBloc>(
      builder: (BuildContext context, Widget widget, AuthBloc auth) {
        return Scaffold(
          body: new Builder(builder: (BuildContext context) {
            return Container(
              padding: const EdgeInsets.all(45),
              decoration: BoxDecoration(
                color: Colors.black12,
              ),
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  InputWiget(
                      text: '登录名', obscure: false, controller: textMobile),
                  SizedBox(height: 25),
                  InputWiget(text: '密码', obscure: true, controller: textPass),
                  SizedBox(height: 25),
                  auth.isregist
                      ? InputWiget(
                          text: '姓名', obscure: false, controller: textName)
                      : Container(height: 0.0, width: 0.0),
                  auth.isregist
                      ? SizedBox(height: 25)
                      : Container(height: 0.0, width: 0.0),
                  SizedBox(height: 25),
                  LoginButtonWiget(
                      text: auth.isregist ? '注册' : '登陆',
                      textMobile: textMobile,
                      textName: textName,
                      textPass: textPass),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceAround,
                    children: <Widget>[
                      FlatButton(
                          textColor: Colors.blueGrey,
                          child: Text('忘记密码'),
                          onPressed: () => Scaffold.of(context).showSnackBar(
                              new SnackBar(
                                  content: new Text('请联系管理员: 15311666086')))),
                      FlatButton(
                          textColor: Colors.blueGrey,
                          child: Text(auth.isregist ? '登陆' : '注册'),
                          onPressed: () => auth.changeRegist(!auth.isregist))
                    ],
                  )
                ],
              ),
            );
          }),
        );
      },
    );
  }
}

// 输入框
class InputWiget extends StatelessWidget {
  final String text;
  final bool obscure;
  final TextEditingController controller;

  InputWiget(
      {Key key,
      @required this.text,
      @required this.obscure,
      @required this.controller})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    return new SizedBox(
      child: new Container(
        padding: EdgeInsets.all(5),
        decoration: BoxDecoration(
          borderRadius: BorderRadius.circular(30),
          color: Colors.white,
        ),
        alignment: Alignment.center,
        child: TextField(
          textAlign: TextAlign.center,
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

class LoginButtonWiget extends StatefulWidget {
  final TextEditingController textMobile;
  final TextEditingController textName;
  final TextEditingController textPass;
  final String text;

  LoginButtonWiget(
      {Key key,
      @required this.text,
      @required this.textMobile,
      @required this.textName,
      @required this.textPass})
      : super(key: key);

  @override
  State<StatefulWidget> createState() {
    return new LoginButtonWigetState();
  }
}

class LoginButtonWigetState extends State<LoginButtonWiget>
    with SingleTickerProviderStateMixin {
  AnimationController _animateControllerWidth;
  Animation _animateWidth;

  String get _text => widget.text;

  void onTap() async {
    if (_animateControllerWidth.isAnimating) {
      return;
    }

    AuthBloc auth = Provide.value<AuthBloc>(context);
    if (auth.isloading) {
      return;
    }

    if (_animateControllerWidth.isDismissed) {
      await _animateControllerWidth.forward();
    }

    auth
        .login(mobile: widget.textMobile.text, pass: widget.textPass.text)
        .catchError((error) {
      Scaffold.of(context)
          .showSnackBar(new SnackBar(content: new Text(error.toString())));
    }).whenComplete(() async {
      if (auth.current == null) {
        await _animateControllerWidth.reverse();
      } else {
        Navigator.pushReplacementNamed(context, "/HomeView");
      }
    });
  }

  @override
  void initState() {
    super.initState();

    _animateControllerWidth = new AnimationController(
        vsync: this, duration: new Duration(milliseconds: 200));

    _animateWidth = new Tween(begin: 320.0, end: 60.0).animate(
        new CurvedAnimation(
            parent: _animateControllerWidth, curve: Curves.easeOutQuad))
      ..addListener(() {
        setState(() {});
      });
  }

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
        onTap: onTap,
        child: Container(
            decoration: BoxDecoration(
                color: const Color.fromRGBO(255, 120, 110, 1.0),
                borderRadius: BorderRadius.all(Radius.circular(35))),
            width: _animateWidth.value,
            height: 60.0,
            alignment: Alignment.center,
            child: _animateControllerWidth.isDismissed
                ? Text(
                    _text,
                    style: TextStyle(color: Color(0xffffffff)),
                  )
                : _animateControllerWidth.isCompleted
                    ? CircularProgressIndicator(
                        valueColor:
                            new AlwaysStoppedAnimation<Color>(Colors.white),
                        strokeWidth: 3,
                      )
                    : null));
  }

  @override
  void dispose() {
    _animateControllerWidth?.dispose();
    super.dispose();
  }
}

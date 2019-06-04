import 'package:flutter/material.dart';
import 'package:locate/model/model.dart';

class HomeView extends StatefulWidget {
  @override
  State<StatefulWidget> createState() {
    return new HomeViewState();
  }
}

class HomeViewState extends State<HomeView>
    with SingleTickerProviderStateMixin {
  AnimationController _animateController;
  Animation _animate;

  @override
  void initState() {
    super.initState();

    _animateController = new AnimationController(
        vsync: this, duration: new Duration(milliseconds: 200));

    _animate = new Tween(begin: 320.0, end: 60.0).animate(
        new CurvedAnimation(
            parent: _animateController, curve: Curves.easeOutQuad))
      ..addListener(() {
        setState(() {});
      });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: Text('Home'),
        ),
        body: Provide<AuthBloc>(builder: (context, child, auth) => Container(
                child: Center(
                    child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    Text(auth.current?.id),
                    Text(auth.current?.name),
                    Text(auth.current?.mobile),
                    Text(auth.current?.face),
                    Text(auth.current?.time.toString()),
                    RaisedButton(
                      child: Text('退出'),
                      onPressed: () {
                        auth.logout();
                        Navigator.pushReplacementNamed(context, "/LoginView");
                      },
                    ),
                  ],
                )),
              ),
        ));
  }
}

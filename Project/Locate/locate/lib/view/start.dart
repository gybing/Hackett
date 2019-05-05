import 'dart:async';

import 'package:flutter/material.dart';
import 'package:font_awesome_flutter/font_awesome_flutter.dart';
import 'starttext.dart';

class StartView extends StatefulWidget {
  @override
  State<StatefulWidget> createState() {
    return new StartState();
  }
}

class StartState extends State<StartView> with SingleTickerProviderStateMixin {
  AnimationController colorController;
  Animation colorAnimation;
  String text;

  @override
  void initState() {
    super.initState();
    text = StartGetRandomText();
    colorController = new AnimationController(
        vsync: this, duration: new Duration(seconds: 3));

    colorAnimation = colorController
        .drive(ColorTween(begin: Color(0xFFFF786E), end: Color(0xFFFFA07A)))
          ..addListener(() {
            if (colorController.isDismissed) {
              Navigator.pushReplacementNamed(context, "/LoginView");
            }
            setState(() {});
          });
    playAnimation();
  }

  Future<Null> playAnimation() async {
    try {
      await colorController.forward();
      await colorController.reverse();
    } on TickerCanceled {}
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Container(
        decoration: BoxDecoration(color: colorAnimation.value),
        child: Center(
          child: Wrap(
              direction: Axis.vertical,
              crossAxisAlignment: WrapCrossAlignment.center,
              children: <Padding>[
                    Padding(
                      padding: EdgeInsets.all(50),
                      child: new Icon(FontAwesomeIcons.cloud, size: 100, color: Colors.white,),
                    )
                  ] +
                  text
                      .split('.')
                      .map((t) => Padding(
                          child: Text(
                            t,
                            softWrap: true,
                            style: TextStyle(color: Colors.white, fontSize: 20),
                            textAlign: TextAlign.start,
                          ),
                          padding: EdgeInsets.all(5)))
                      .toList()),
        ),
      ),
    );
  }

  @override
  void dispose() {
    colorController.dispose();
    super.dispose();
  }
}

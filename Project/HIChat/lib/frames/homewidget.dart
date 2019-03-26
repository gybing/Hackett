import 'dart:async';
import 'dart:math' as math;

import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:flutter/material.dart';
import 'login/ui/home_page.dart';
import 'login/ui/user_provider.dart';

class HomeWidget extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return CupertinoPageScaffold(
      navigationBar: CupertinoNavigationBar(
        trailing: Row(
        mainAxisSize: MainAxisSize.min,
        children: <Widget>[
          const ExitButton(),
          const Padding(padding: EdgeInsets.only(left: 8.0)),
          const ExitButton(),
        ],
      ),
      ),
      child: DecoratedBox(
        decoration: const BoxDecoration(color: Color(0xFFEFEFF4)),
        child: ListView(
          children: <Widget>[
            const Padding(padding: EdgeInsets.only(top: 32.0)),
            GestureDetector(
              onTap: () {
                Navigator.of(context, rootNavigator: true).push(
                  CupertinoPageRoute<bool>(
                    fullscreenDialog: true,
                    builder: (BuildContext context) => new UserContainer(user: null, child: new HomePage()),
                  ),
                );
              },
              child: Container(
                decoration: const BoxDecoration(
                  color: CupertinoColors.white,
                  border: Border(
                    top: BorderSide(color: Color(0xFFBCBBC1), width: 0.0),
                    bottom: BorderSide(color: Color(0xFFBCBBC1), width: 0.0),
                  ),
                ),
                height: 44.0,
                child: Padding(
                  padding: const EdgeInsets.symmetric(horizontal: 16.0, vertical: 8.0),
                  child: SafeArea(
                    top: false,
                    bottom: false,
                    child: Row(
                      children: const <Widget>[
                        Text(
                          'Sign in',
                          style: TextStyle(color: CupertinoColors.activeBlue),
                        )
                      ],
                    ),
                  ),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class ExitButton extends StatelessWidget {
  const ExitButton();

  @override
  Widget build(BuildContext context) {
    return CupertinoButton(
      padding: EdgeInsets.zero,
      child: const Tooltip(
        message: 'Back',
        child: Text('Exit'),
        excludeFromSemantics: true,
      ),
      onPressed: () {
        // The demo is on the root navigator.
        Navigator.of(context, rootNavigator: true).pop();
      },
    );
  }
}

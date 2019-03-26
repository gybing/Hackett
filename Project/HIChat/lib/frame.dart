import 'dart:async';
import 'dart:math' as math;

import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';

import 'frames/chatwidget.dart';
import 'frames/contactwidget.dart';
import 'frames/homewidget.dart';


const List<Color> coolColors = <Color>[
  Color.fromARGB(255, 255, 59, 48),
  Color.fromARGB(255, 255, 149, 0),
  Color.fromARGB(255, 255, 204, 0),
  Color.fromARGB(255, 76, 217, 100),
  Color.fromARGB(255, 90, 200, 250),
  Color.fromARGB(255, 0, 122, 255),
  Color.fromARGB(255, 88, 86, 214),
  Color.fromARGB(255, 255, 45, 85),
];

const List<String> coolColorNames = <String>[
  'Sarcoline', 'Coquelicot', 'Smaragdine', 'Mikado', 'Glaucous', 'Wenge',
  'Fulvous', 'Xanadu', 'Falu', 'Eburnean', 'Amaranth', 'Australien',
  'Banan', 'Falu', 'Gingerline', 'Incarnadine', 'Labrador', 'Nattier',
  'Pervenche', 'Sinoper', 'Verditer', 'Watchet', 'Zaffre',
];

class FrameWidget extends StatelessWidget {
  FrameWidget()
      : colorItems = List<Color>.generate(50, (int index) {
          return coolColors[math.Random().nextInt(coolColors.length)];
        }) ,
        colorNameItems = List<String>.generate(50, (int index) {
          return coolColorNames[math.Random().nextInt(coolColorNames.length)];
        });

  static const String routeName = '/cupertino/navigation';

  final List<Color> colorItems;
  final List<String> colorNameItems;

  @override
  Widget build(BuildContext context) {
    return WillPopScope(
      // Prevent swipe popping of this page. Use explicit exit buttons only.
      onWillPop: () => Future<bool>.value(true),
      child: DefaultTextStyle(
        style: const TextStyle(
          fontFamily: '.SF UI Text',
          fontSize: 17.0,
          color: CupertinoColors.black,
        ),
        child: CupertinoTabScaffold(
          tabBar: CupertinoTabBar(
            items: const <BottomNavigationBarItem>[
              BottomNavigationBarItem(
                icon: Icon(CupertinoIcons.conversation_bubble),
                title: Text('消息'),
              ),
              BottomNavigationBarItem(
                icon: Icon(IconData(0xf47d, fontFamily: 'CupertinoIcons', fontPackage: 'cupertino_icons', matchTextDirection: true)),
                title: Text('联系人'),
              ),
              BottomNavigationBarItem(
                icon: Icon(IconData(0xf46d, fontFamily: 'CupertinoIcons', fontPackage: 'cupertino_icons', matchTextDirection: true)),
                title: Text('应用'),
              ),
              BottomNavigationBarItem(
                icon: Icon(IconData(0xf447, fontFamily: 'CupertinoIcons', fontPackage: 'cupertino_icons', matchTextDirection: true)),
                title: Text('我的'),
              ),
            ],
          ),
          tabBuilder: (BuildContext context, int index) {
            assert(index >= 0 && index <= 3);
            switch (index) {
              case 0:
                return CupertinoTabView(
                  builder: (BuildContext context) {
                    return ChatWidget(
                      colorItems: colorItems,
                      colorNameItems: colorNameItems
                    );
                  },
                  defaultTitle: '消息',
                );
                break;
              case 1:
                return CupertinoTabView(
                  builder: (BuildContext context) {
                    return ContactWidget(
                      colorItems: colorItems,
                      colorNameItems: colorNameItems
                    );
                  },
                  defaultTitle: '联系人',
                );
                break;
              case 2:
                return CupertinoTabView(
                  builder: (BuildContext context) => HomeWidget(),
                  defaultTitle: '应用',
                );
                break;
              case 3:
                return CupertinoTabView(
                  builder: (BuildContext context) => HomeWidget(),
                  defaultTitle: '我的',
                );
                break;
            }
            return null;
          },
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

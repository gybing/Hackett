import 'dart:async';
import 'dart:math' as math;

import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';

const int _kChildCount = 500;

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

class ContactWidget extends StatelessWidget {
  const ContactWidget({this.colorItems, this.colorNameItems});

  final List<Color> colorItems;
  final List<String> colorNameItems;

  @override
  Widget build(BuildContext context) {
    return CupertinoPageScaffold(
      child: CustomScrollView(
        semanticChildCount: _kChildCount,
        slivers: <Widget>[
          CupertinoSliverNavigationBar(
            trailing: Row(
            mainAxisSize: MainAxisSize.min,
            children: <Widget>[
              const ExitButton(),
              const Padding(padding: EdgeInsets.only(left: 8.0)),
              const ExitButton(),
            ],
          ),
          ),
          SliverPadding(
            // Top media padding consumed by CupertinoSliverNavigationBar.
            // Left/Right media padding consumed by ContactItem.
            padding: MediaQuery.of(context).removePadding(
              removeTop: true,
              removeLeft: true,
              removeRight: true,
            ).padding,
            sliver: SliverList(
              delegate: SliverChildBuilderDelegate(
                (BuildContext context, int index) {
                  return ContactItem(
                    index: index,
                    lastItem: index == _kChildCount - 1,
                    color: colorItems[index],
                    colorName: colorNameItems[index],
                  );
                },
                childCount: _kChildCount,
              ),
            ),
          ),
        ],
      ),
    );
  }
}


class ContactItem extends StatelessWidget {
  const ContactItem({this.index, this.lastItem, this.color, this.colorName});

  final int index;
  final bool lastItem;
  final Color color;
  final String colorName;

  @override
  Widget build(BuildContext context) {
    final Widget row = GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: () {
        Navigator.of(context).push(CupertinoPageRoute<void>(
          title: colorName,
          builder: (BuildContext context) => ContactItemPage(
            color: color,
            colorName: colorName,
            index: index,
          ),
        ));
      },
      child: SafeArea(
        top: false,
        bottom: false,
        child: Padding(
          padding: const EdgeInsets.only(left: 16.0, top: 8.0, bottom: 8.0, right: 8.0),
          child: Row(
            children: <Widget>[
              Container(
                height: 60.0,
                width: 60.0,
                decoration: BoxDecoration(
                  color: color,
                  borderRadius: BorderRadius.circular(8.0),
                ),
              ),
              Expanded(
                child: Padding(
                  padding: const EdgeInsets.symmetric(horizontal: 12.0),
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: <Widget>[
                      Text(colorName),
                      const Padding(padding: EdgeInsets.only(top: 8.0)),
                      const Text(
                        'Buy this cool color',
                        style: TextStyle(
                          color: Color(0xFF8E8E93),
                          fontSize: 13.0,
                          fontWeight: FontWeight.w300,
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              CupertinoButton(
                padding: EdgeInsets.zero,
                child: const Icon(CupertinoIcons.plus_circled,
                  color: CupertinoColors.activeBlue,
                  semanticLabel: 'Add',
                ),
                onPressed: () { },
              ),
              CupertinoButton(
                padding: EdgeInsets.zero,
                child: const Icon(CupertinoIcons.share,
                  color: CupertinoColors.activeBlue,
                  semanticLabel: 'Share',
                ),
                onPressed: () { },
              ),
            ],
          ),
        ),
      ),
    );

    if (lastItem) {
      return row;
    }

    return Column(
      children: <Widget>[
        row,
        Container(
          height: 1.0,
          color: const Color(0xFFD9D9D9),
        ),
      ],
    );
  }
}

class ContactItemPage extends StatefulWidget {
  const ContactItemPage({this.color, this.colorName, this.index});

  final Color color;
  final String colorName;
  final int index;

  @override
  State<StatefulWidget> createState() => ContactItemPageState();
}

class ContactItemPageState extends State<ContactItemPage> {
  @override
  void initState() {
    super.initState();
    relatedColors = List<Color>.generate(10, (int index) {
      final math.Random random = math.Random();
      return Color.fromARGB(
        255,
      (widget.color.red + random.nextInt(100) - 50).clamp(0, 255),
        (widget.color.green + random.nextInt(100) - 50).clamp(0, 255),
        (widget.color.blue + random.nextInt(100) - 50).clamp(0, 255),
      );
    });
  }

  List<Color> relatedColors;

  @override
  Widget build(BuildContext context) {
    return CupertinoPageScaffold(
      navigationBar: const CupertinoNavigationBar(
        trailing: ExitButton(),
      ),
      child: SafeArea(
        top: false,
        bottom: false,
        child: ListView(
          children: <Widget>[
            const Padding(padding: EdgeInsets.only(top: 16.0)),
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 16.0),
              child: Row(
                mainAxisSize: MainAxisSize.max,
                children: <Widget>[
                  Container(
                    height: 128.0,
                    width: 128.0,
                    decoration: BoxDecoration(
                      color: widget.color,
                      borderRadius: BorderRadius.circular(24.0),
                    ),
                  ),
                  const Padding(padding: EdgeInsets.only(left: 18.0)),
                  Expanded(
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      mainAxisSize: MainAxisSize.min,
                      children: <Widget>[
                        Text(
                          widget.colorName,
                          style: const TextStyle(fontSize: 24.0, fontWeight: FontWeight.bold),
                        ),
                        const Padding(padding: EdgeInsets.only(top: 6.0)),
                        Text(
                          'Item number ${widget.index}',
                          style: const TextStyle(
                            color: Color(0xFF8E8E93),
                            fontSize: 16.0,
                            fontWeight: FontWeight.w100,
                          ),
                        ),
                        const Padding(padding: EdgeInsets.only(top: 20.0)),
                        Row(
                          mainAxisAlignment: MainAxisAlignment.spaceBetween,
                          children: <Widget>[
                            CupertinoButton(
                              color: CupertinoColors.activeBlue,
                              minSize: 30.0,
                              padding: const EdgeInsets.symmetric(horizontal: 24.0),
                              borderRadius: BorderRadius.circular(32.0),
                              child: const Text(
                                'GET',
                                style: TextStyle(
                                  fontSize: 14.0,
                                  fontWeight: FontWeight.w700,
                                  letterSpacing: -0.28,
                                ),
                              ),
                              onPressed: () { },
                            ),
                            CupertinoButton(
                              color: CupertinoColors.activeBlue,
                              minSize: 30.0,
                              padding: EdgeInsets.zero,
                              borderRadius: BorderRadius.circular(32.0),
                              child: const Icon(CupertinoIcons.ellipsis, color: CupertinoColors.white),
                              onPressed: () { },
                            ),
                          ],
                        ),
                      ],
                    ),
                  ),
                ],
              ),
            ),
            const Padding(
              padding: EdgeInsets.only(left: 16.0, top: 28.0, bottom: 8.0),
              child: Text(
                'USERS ALSO LIKED',
                style: TextStyle(
                  color: Color(0xFF646464),
                  letterSpacing: -0.60,
                  fontSize: 15.0,
                  fontWeight: FontWeight.w500,
                ),
              ),
            ),
            SizedBox(
              height: 200.0,
              child: ListView.builder(
                scrollDirection: Axis.horizontal,
                itemCount: 10,
                itemExtent: 160.0,
                itemBuilder: (BuildContext context, int index) {
                  return Padding(
                    padding: const EdgeInsets.only(left: 16.0),
                    child: Container(
                      decoration: BoxDecoration(
                        borderRadius: BorderRadius.circular(8.0),
                        color: relatedColors[index],
                      ),
                      child: Center(
                        child: CupertinoButton(
                          child: const Icon(
                            CupertinoIcons.plus_circled,
                            color: CupertinoColors.white,
                            size: 36.0,
                          ),
                          onPressed: () { },
                        ),
                      ),
                    ),
                  );
                },
              ),
            ),
          ],
        ),
      ),
    );
  }
}
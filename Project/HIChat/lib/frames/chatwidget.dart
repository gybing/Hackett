import 'dart:async';
import 'dart:math' as math;

import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';

const int _kChildCount = 5000;
const String _kGalleryAssetsPackage = 'flutter_gallery_assets';

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

class ChatWidget extends StatelessWidget {
  const ChatWidget({this.colorItems, this.colorNameItems});

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
            // Left/Right media padding consumed by ChatItem.
            padding: MediaQuery.of(context).removePadding(
              removeTop: true,
              removeLeft: true,
              removeRight: true,
            ).padding,
            sliver: SliverList(
              delegate: SliverChildBuilderDelegate(
                (BuildContext context, int index) {
                  return ChatItem(
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


class ChatItem extends StatelessWidget {
  const ChatItem({this.index, this.lastItem, this.color, this.colorName});

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
          builder: (BuildContext context) => CupertinoTabView(
                  builder: (BuildContext context) => CupertinoDemoTab2(),
                  defaultTitle: 'Support Chat',
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

class CupertinoDemoTab2 extends StatelessWidget {
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
      child: ListView(
        children: <Widget>[
          Tab2Header(),
        ]..addAll(buildTab2Conversation()),
      ),
    );
  }
}

class Tab2Header extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(16.0),
      child: SafeArea(
        top: false,
        bottom: false,
        child: ClipRRect(
          borderRadius: const BorderRadius.all(Radius.circular(16.0)),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: <Widget>[
              Container(
                decoration: const BoxDecoration(
                  color: Color(0xFFE5E5E5),
                ),
                child: Padding(
                  padding: const EdgeInsets.symmetric(horizontal: 18.0, vertical: 12.0),
                  child: Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: const <Widget>[
                      Text(
                        'SUPPORT TICKET',
                        style: TextStyle(
                          color: Color(0xFF646464),
                          letterSpacing: -0.9,
                          fontSize: 14.0,
                          fontWeight: FontWeight.w500,
                        ),
                      ),
                      Text(
                        'Show More',
                        style: TextStyle(
                          color: Color(0xFF646464),
                          letterSpacing: -0.6,
                          fontSize: 12.0,
                          fontWeight: FontWeight.w500,
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              Container(
                decoration: const BoxDecoration(
                  color: Color(0xFFF3F3F3),
                ),
                child: Padding(
                  padding: const EdgeInsets.symmetric(horizontal: 18.0, vertical: 12.0),
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: <Widget>[
                      const Text(
                        'Product or product packaging damaged during transit',
                        style: TextStyle(
                          fontSize: 16.0,
                          fontWeight: FontWeight.w700,
                          letterSpacing: -0.46,
                        ),
                      ),
                      const Padding(padding: EdgeInsets.only(top: 16.0)),
                      const Text(
                        'REVIEWERS',
                        style: TextStyle(
                          color: Color(0xFF646464),
                          fontSize: 12.0,
                          letterSpacing: -0.6,
                          fontWeight: FontWeight.w500,
                        ),
                      ),
                      const Padding(padding: EdgeInsets.only(top: 8.0)),
                      Row(
                        children: <Widget>[
                          Container(
                            width: 44.0,
                            height: 44.0,
                            decoration: const BoxDecoration(
                              image: DecorationImage(
                                image: AssetImage(
                                  'people/square/trevor.png',
                                  package: _kGalleryAssetsPackage,
                                ),
                              ),
                              shape: BoxShape.circle,
                            ),
                          ),
                          const Padding(padding: EdgeInsets.only(left: 8.0)),
                          Container(
                            width: 44.0,
                            height: 44.0,
                            decoration: const BoxDecoration(
                              image: DecorationImage(
                                image: AssetImage(
                                  'people/square/sandra.png',
                                  package: _kGalleryAssetsPackage,
                                ),
                              ),
                              shape: BoxShape.circle,
                            ),
                          ),
                          const Padding(padding: EdgeInsets.only(left: 2.0)),
                          const Icon(
                            CupertinoIcons.check_mark_circled,
                            color: Color(0xFF646464),
                            size: 20.0,
                          ),
                        ],
                      ),
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

enum Tab2ConversationBubbleColor {
  blue,
  gray,
}

class Tab2ConversationBubble extends StatelessWidget {
  const Tab2ConversationBubble({this.text, this.color});

  final String text;
  final Tab2ConversationBubbleColor color;

  @override
  Widget build(BuildContext context) {
    return Container(
      decoration: BoxDecoration(
        borderRadius: const BorderRadius.all(Radius.circular(18.0)),
        color: color == Tab2ConversationBubbleColor.blue
            ? CupertinoColors.activeBlue
            : CupertinoColors.lightBackgroundGray,
      ),
      margin: const EdgeInsets.symmetric(horizontal: 8.0, vertical: 8.0),
      padding: const EdgeInsets.symmetric(horizontal: 14.0, vertical: 10.0),
      child: Text(
        text,
        style: TextStyle(
          color: color == Tab2ConversationBubbleColor.blue
              ? CupertinoColors.white
              : CupertinoColors.black,
          letterSpacing: -0.4,
          fontSize: 15.0,
          fontWeight: FontWeight.w400,
        ),
      ),
    );
  }
}

class Tab2ConversationAvatar extends StatelessWidget {
  const Tab2ConversationAvatar({this.text, this.color});

  final String text;
  final Color color;

  @override
  Widget build(BuildContext context) {
    return Container(
      decoration: BoxDecoration(
        shape: BoxShape.circle,
        gradient: LinearGradient(
          begin: FractionalOffset.topCenter,
          end: FractionalOffset.bottomCenter,
          colors: <Color>[
            color,
            Color.fromARGB(
              color.alpha,
              (color.red - 60).clamp(0, 255),
              (color.green - 60).clamp(0, 255),
              (color.blue - 60).clamp(0, 255),
            ),
          ],
        ),
      ),
      margin: const EdgeInsets.only(left: 8.0, bottom: 8.0),
      padding: const EdgeInsets.all(12.0),
      child: Text(
        text,
        style: const TextStyle(
          color: CupertinoColors.white,
          fontSize: 13.0,
          fontWeight: FontWeight.w500,
        ),
      ),
    );
  }
}

class Tab2ConversationRow extends StatelessWidget {
  const Tab2ConversationRow({this.avatar, this.text});

  final Tab2ConversationAvatar avatar;
  final String text;

  @override
  Widget build(BuildContext context) {
    final List<Widget> children = <Widget>[];
    if (avatar != null)
      children.add(avatar);

    final bool isSelf = avatar == null;
    children.add(
      Tab2ConversationBubble(
        text: text,
        color: isSelf
          ? Tab2ConversationBubbleColor.blue
          : Tab2ConversationBubbleColor.gray,
      ),
    );
    return SafeArea(
      child: Row(
        mainAxisAlignment: isSelf ? MainAxisAlignment.end : MainAxisAlignment.start,
        mainAxisSize: MainAxisSize.min,
        crossAxisAlignment: isSelf ? CrossAxisAlignment.center : CrossAxisAlignment.end,
        children: children,
      ),
    );
  }
}

List<Widget> buildTab2Conversation() {
 return <Widget>[
    const Tab2ConversationRow(
      text: "My Xanadu doesn't look right",
    ),
    const Tab2ConversationRow(
      avatar: Tab2ConversationAvatar(
        text: 'KL',
        color: Color(0xFFFD5015),
      ),
      text: "We'll rush you a new one.\nIt's gonna be incredible",
    ),
    const Tab2ConversationRow(
      text: 'Awesome thanks!',
    ),
    const Tab2ConversationRow(
      avatar: Tab2ConversationAvatar(
        text: 'SJ',
        color: Color(0xFF34CAD6),
      ),
      text: "We'll send you our\nnewest Labrador too!",
    ),
    const Tab2ConversationRow(
      text: 'Yay',
    ),
    const Tab2ConversationRow(
      avatar: Tab2ConversationAvatar(
        text: 'KL',
        color: Color(0xFFFD5015),
      ),
      text: "Actually there's one more thing...",
    ),
    const Tab2ConversationRow(
      text: "What's that?",
    ),
  ];
}
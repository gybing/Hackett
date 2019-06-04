// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'package:flutter/material.dart';
import 'package:font_awesome_flutter/font_awesome_flutter.dart';
import 'package:flutter/gestures.dart' show DragStartBehavior;
import 'package:locate/data/struct.dart';
import 'package:locate/data/network.dart';
import 'package:locate/model/model.dart';

class ContactView extends StatefulWidget {
  @override
  _ContactViewState createState() => _ContactViewState();
}

class _ContactViewState extends State<ContactView>
    with TickerProviderStateMixin {
  final GlobalKey<ScaffoldState> _scaffoldKey = GlobalKey<ScaffoldState>();

  static final Animatable<Offset> _drawerDetailsTween = Tween<Offset>(
    begin: const Offset(0.0, -1.0),
    end: Offset.zero,
  ).chain(CurveTween(
    curve: Curves.fastOutSlowIn,
  ));

  AnimationController _controller;
  Animation<double> _drawerContentsOpacity;
  Animation<Offset> _drawerDetailsPosition;
  bool _showDrawerContents = true;

  @override
  void initState() {
    super.initState();
    _controller = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 200),
    );
    _drawerContentsOpacity = CurvedAnimation(
      parent: ReverseAnimation(_controller),
      curve: Curves.fastOutSlowIn,
    );
    _drawerDetailsPosition = _controller.drive(_drawerDetailsTween);
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  void _showNotImplementedMessage() {
    Navigator.pop(context); // Dismiss the drawer.
    _scaffoldKey.currentState.showSnackBar(
        const SnackBar(content: Text("The drawer's items don't do anything")));
  }

  Widget buildAppBar(BuildContext context) {
    return AppBar(
      leading: IconButton(
        icon: Icon(
          FontAwesomeIcons.solidUserCircle,
          color: Colors.white,
        ),
        // tooltip: auth.current?.Name,
        onPressed: () {
          _scaffoldKey.currentState.openDrawer();
        },
      ),
      title: Provide<AuthBloc>(
          builder: (context, child, auth) =>
              Text(auth.current != null ? auth.current.name : "联系人")),
      centerTitle: true,
      actions: <Widget>[
        IconButton(
          icon: Icon(
            FontAwesomeIcons.plus,
            color: Colors.white,
          ),
          onPressed: () async {
            final auth = Provide.value<AuthBloc>(context);
            await auth.login(mobile: "13671090924", pass: "123456");

            final contact = Provide.value<ContactBloc>(context);
            await contact.getContactList();
          },
        )
      ],
    );
  }

  Widget buildAccount(BuildContext context) {
    return Provide<AuthBloc>(
        builder: (context, child, auth) => UserAccountsDrawerHeader(
              accountName: Text(auth.current != null ? auth.current.name : ""),
              accountEmail:
                  Text(auth.current != null ? auth.current.mobile : ""),
              currentAccountPicture: auth.current != null
                  ? (auth.current.face != null && auth.current.face != ""
                      ? CircleAvatar(
                          backgroundImage: NetworkImage(auth.current.face),
                        )
                      : CircleAvatar(
                          backgroundColor: Colors.white,
                          child: Text(
                            auth.current.name[0],
                            style: TextStyle(
                                color: Colors.blue,
                                fontWeight: FontWeight.w700,
                                fontSize: 35),
                          ),
                        ))
                  : CircleAvatar(
                      backgroundImage: AssetImage(
                        'images/trevor.png',
                      ),
                    ),
              margin: EdgeInsets.zero,
              onDetailsPressed: () {
                _showDrawerContents = !_showDrawerContents;
                if (_showDrawerContents)
                  _controller.reverse();
                else
                  _controller.forward();
              },
            ));
  }

  Widget buildContacts(BuildContext context) {
    return Provide<ContactBloc>(
        builder: (context, child, contact) => ListView.builder(
            padding: new EdgeInsets.all(5.0),
            itemCount: 100,
            itemBuilder: (BuildContext context, int index) {
              return ListTile(
                leading: CircleAvatar(child: Text("")),
                title: Text('Drawer item $index'),
              );
            }));
  }

  Widget buildDrawer(BuildContext context) {
    return Drawer(
      child: Column(
        children: <Widget>[
          buildAccount(context),
          MediaQuery.removePadding(
            context: context,
            // DrawerHeader consumes top MediaQuery padding.
            removeTop: true,
            child: Expanded(
              child: ListView(
                dragStartBehavior: DragStartBehavior.down,
                padding: const EdgeInsets.only(top: 8.0),
                children: <Widget>[
                  Stack(
                    children: <Widget>[
                      // The initial contents of the drawer.
                      buildContacts(context),
                      // The drawer's "details" view.
                      SlideTransition(
                        position: _drawerDetailsPosition,
                        child: FadeTransition(
                          opacity: ReverseAnimation(_drawerContentsOpacity),
                          child: Column(
                            mainAxisSize: MainAxisSize.min,
                            crossAxisAlignment: CrossAxisAlignment.stretch,
                            children: <Widget>[
                              ListTile(
                                leading: const Icon(Icons.add),
                                title: const Text('添加好友'),
                                onTap: _showNotImplementedMessage,
                              ),
                              ListTile(
                                leading: const Icon(Icons.settings),
                                title: const Text('设置'),
                                onTap: _showNotImplementedMessage,
                              ),
                            ],
                          ),
                        ),
                      ),
                    ],
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      drawerDragStartBehavior: DragStartBehavior.down,
      key: _scaffoldKey,
      appBar: buildAppBar(context),
      drawer: buildDrawer(context),
      body: Center(
        child: Text("map"),
      ),
    );
  }
}

typedef ContactItemCallback = void Function();

class ContactItem extends StatefulWidget {
  final ContactInfo contact;
  final ContactItemCallback callback;

  const ContactItem({Key key, this.contact, this.callback}) : super(key: key);

  @override
  ContactItemState createState() => ContactItemState();
}

class ContactItemState extends State<ContactItem> {
  ContactItemCallback get callback => widget.callback;
  ContactInfo get contact => widget.contact;
  UserInfo userinfo;

  @override
  void initState() {
    super.initState();

    api.getUserInfo(id: contact.contact).then((u) {
      setState(() {
        userinfo = UserInfo.fromJson(u);
      });
    });
  }

  @override
  Widget build(BuildContext context) {
    var name = userinfo == null ? "" : userinfo.name;
    return ListTile(
      leading: CircleAvatar(child: Text(name)),
      title: Text('Drawer item $name'),
      onTap: callback,
    );
  }
}

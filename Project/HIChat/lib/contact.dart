import 'package:flutter/material.dart';

import 'map.dart';

class ContactWidget extends StatelessWidget {
  List<String> contacts =
      List<String>.generate(20, (int index) => '联系人${index}');

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('联系人')),
      body: Column(
        children: <Widget>[
          Flexible(
            flex: 3,
            child: MapWidget(),
          ),
          Flexible(
            flex: 7,
            child: ListView.builder(
                itemCount: contacts.length,
                itemBuilder: (context, index) {
                  return new ListTile(
                    title: new Text('${contacts[index]}'),
                  );
                }),
          ),
        ],
      ),
    );
  }
}

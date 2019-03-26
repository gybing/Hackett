import 'package:flutter/material.dart';

import 'contact.dart';
import 'contacts.dart';

void main() => runApp(HIChatApp());

class HIChatApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'HIChat',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: ContactsDemo(),
    );
  }
}

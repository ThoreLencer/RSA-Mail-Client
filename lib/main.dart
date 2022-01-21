import 'package:flutter/material.dart';
import 'package:rsamail/contact_list.dart';
import 'package:rsamail/database.dart';
import 'package:rsamail/rsa_encryptor.dart';

import 'network.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  //Application root
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'RSA-Mail-Client',
      theme: ThemeData(
        primarySwatch: Colors.lightGreen
      ),
      home: ContactList(client: Network(), rsa: RSAEncryptor(), db: MailDatabase()),
    );
  }
}
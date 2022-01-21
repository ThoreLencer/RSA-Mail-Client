import 'dart:async';
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:path_provider/path_provider.dart';

Future<void> showMessageDialog(
    BuildContext context, String title, String message) async {
  return showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text(title),
          content: Text(message),
          actions: <Widget>[
            TextButton(
                onPressed: () {
                  Navigator.of(context).pop();
                },
                child: const Text('Ok'))
          ],
        );
      });
}

Future<bool> showMessageDialogYesNo(
    BuildContext context, String title, String message) async {
  Completer<bool> result = Completer<bool>();
  showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text(title),
          content: Text(message),
          actions: <Widget>[
            TextButton(
                onPressed: () {
                  Navigator.of(context).pop();
                  result.complete(true);
                },
                child: const Text('Ja')),
            TextButton(
                onPressed: () {
                  Navigator.of(context).pop();
                  result.complete(false);
                },
                child: const Text('Nein'))
          ],
        );
      });
  return result.future;
}

Future<String> getSavePath() async {
  if (Platform.isLinux || Platform.isWindows || Platform.isMacOS) {
    String path = Platform.resolvedExecutable;
    path = path.replaceAll("\\", "/");
    int last = path.lastIndexOf("/");
    return path.substring(0, last);
  } else {
    //Get Android/IPhone path
    Directory appDocDir = await getApplicationDocumentsDirectory();
    return appDocDir.path;
  }
}

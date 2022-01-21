
import 'dart:async';

import 'package:drift/drift.dart';
import 'package:flutter/material.dart';
import 'package:rsamail/database.dart';
import 'package:rsamail/network.dart';
import 'package:rsamail/rsa_encryptor.dart';

class MailSynchronizer {
  late Network client;
  late MailDatabase db;
  late RSAEncryptor rsa;
  late Timer _timer;
  bool running = true;

  MailSynchronizer(Network network, RSAEncryptor encryptor, MailDatabase mailDatabase){
    db = mailDatabase;
    client = network;
    rsa = encryptor;
    _timer = Timer(const Duration(seconds: 3), _timerCallback);
  }

  void _timerCallback() async {
    bool updateNeeded = false;
    //Load Messages
    if (await client.ServerReachable()) {
      //Get Messages
      List<int> ids = await client.GetMessageIDs(rsa);
      for (int x in ids) {
        if (!await db.ContainsMessage(client.userID, x)) {
          MessagesCompanion m = await client.GetMessage(rsa, x);
          db.AddMessage(m);
          updateNeeded = true;
          if (m.Sender.value == client.userID) {
            if (!await db.ContainsContact(client.userID, m.Receiver.value)) {
              db.AddContact(ContactsCompanion(UserID: Value(client.userID),
                  ContactID: m.Receiver,
                  Name: Value(
                      await client.GetUsername(rsa, m.Receiver.value))));
              updateNeeded = true;
            }
          } else {
            if (!await db.ContainsContact(client.userID, m.Sender.value)) {
              db.AddContact(ContactsCompanion(UserID: Value(client.userID),
                  ContactID: m.Sender,
                  Name: Value(await client.GetUsername(rsa, m.Sender.value))));
              updateNeeded = true;
            }
          }
        }
      }
      //Update Contacts
      List<Message> mes = await db.GetMessages(client.userID);
      for (Message m in mes) {
        if (ids.contains(m.MessageID)) {
          if (m.Sender == client.userID) {
            if (!await db.ContainsContact(client.userID, m.Receiver)) {
              db.AddContact(ContactsCompanion(UserID: Value(client.userID),
                  ContactID: Value(m.Receiver),
                  Name: Value(await client.GetUsername(rsa, m.Receiver))));
              updateNeeded = true;
            }
          } else {
            if (!await db.ContainsContact(client.userID, m.Sender)) {
              db.AddContact(ContactsCompanion(UserID: Value(client.userID),
                  ContactID: Value(m.Sender),
                  Name: Value(await client.GetUsername(rsa, m.Receiver))));
              updateNeeded = true;
            }
          }

          //Update read state
          if (await db.GetRead(client.userID, m.MessageID) == 0) {
            List<String> readState = await client.GetRead(rsa, m.MessageID);
            if (readState[0] == "1") {
              DateTime now = DateTime.now();
              db.SetRead(client.userID, m.MessageID, readState[1]);
              updateNeeded = true;
            }
          }
        } else {
          //Delete
          db.DeleteMessage(client.userID, m.MessageID);
          updateNeeded = true;
        }
      }

      List<Contact> contacts = await db.GetContacts(client.userID);
      for (Contact c in contacts) {
        int id = await client.GetUserID(c.Name);
        if (id == -1) {
          //User was deleted
          db.DeleteContact(client.userID, c.ContactID);
          updateNeeded = true;
        }
      }
    }
    if(running) {
      _timer = Timer(const Duration(seconds: 3), _timerCallback);
    }
  }

  void stop(){
    running = false;
  }
}

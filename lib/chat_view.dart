import 'dart:async';
import 'dart:ui';

import 'package:drift/drift.dart' hide Column;
import 'package:flutter/material.dart';
import 'package:rsamail/rsa_encryptor.dart';
import 'package:rsamail/utils.dart';

import 'database.dart';
import 'mail_sync.dart';
import 'network.dart';

abstract class MessageListItem{
  Widget build(BuildContext context);
}

class SelfMessageItem implements MessageListItem{
  final int read;
  final String message;
  final String date;
  SelfMessageItem(this.message, this.date, this.read);

  @override
  Widget build(BuildContext context){
    bool readState = false;
    if(read == 1){
      readState = true;
    }
    return Row(
        children: [
          Container(margin: const EdgeInsets.only(right:10),child:
              Column(children:[
                Text(readState
                    ? ""
                    : "(ungelesen)", style: TextStyle(fontWeight: FontWeight.bold),),
                Text(date)
              ],
              ),
          ),
          Expanded(
              child: Container(
                child: Container(child: Text(message,style: const TextStyle(color: Colors.white)), padding: const EdgeInsets.all(10),),
                decoration: const BoxDecoration(
                  color: Colors.blueAccent,
                  borderRadius: BorderRadius.all(Radius.circular(10)),
                ),
                constraints: const BoxConstraints(minHeight: 30),
              )
          ),
        ],
    );
  }
}

class OtherMessageItem implements MessageListItem{
  final String message;
  final String date;
  OtherMessageItem(this.message, this.date);

  @override
  Widget build(BuildContext context){
    return Row(
      children: [
        Expanded(
            child: Container(
              child: Container(child: Text(message), padding: EdgeInsets.all(10),),
              decoration: const BoxDecoration(
                color: Colors.black12,
                borderRadius: BorderRadius.all(Radius.circular(10)),
              ),
              constraints: BoxConstraints(minHeight: 30),
            )
        ),
        Container(margin: const EdgeInsets.only(left:10),child: Text(date)),
      ],
    );
  }
}

class ChatView extends StatefulWidget {
  final Network client;
  final RSAEncryptor rsa;
  final MailDatabase db;
  final int contactID;
  const ChatView({Key? key, required this.client, required this.rsa, required this.db, required this.contactID}) : super(key: key);

  @override
  State<StatefulWidget> createState() => ChatViewState(client, rsa, db, contactID);
}
class ChatViewState extends State<ChatView> {
  late Network client;
  late RSAEncryptor rsa;
  late MailDatabase db;
  late int contactID;
  late Timer _synctimer;
  bool _sending = false;
  List<MessageListItem> messageList = [];
  TextEditingController editMessage = TextEditingController();
  ScrollController scrollController = ScrollController();
  String contactName = "";

  ChatViewState(Network network, RSAEncryptor encryptor, MailDatabase mailDatabase, int cID){
    client = network;
    rsa = encryptor;
    db = mailDatabase;
    contactID = cID;
  }

  @override
  void initState() {
    super.initState();
    _GetContactName();
    _loadInitial();

    _synctimer = Timer.periodic(const Duration(seconds: 1), (timer) {
      _updateEvent();
    });
  }

  void _loadInitial() async {
    _updateEvent().then((value) => Future.delayed(Duration(milliseconds: 100), _scrollBottom));
  }

  void _scrollBottom(){
    scrollController.jumpTo(scrollController.position.maxScrollExtent);
    setState(() {});
  }

  void _GetContactName() async{
    contactName = (await db.GetContact(client.userID, contactID)).Name;
    setState(() {});
  }

  @override
  Widget build(BuildContext context) {
    return WillPopScope(child:Scaffold(
      appBar: AppBar(
        title: Text(contactName),
      ),
      body:
        Container(
          padding: EdgeInsets.all(10),
          child:
              Column(
              children: [
                Expanded(
                    child: ListView.builder(
                      controller: scrollController,
                      itemCount: messageList.length,
                      itemBuilder: (context, index){
                        return InkWell(child: Container(
                              child: messageList[index].build(context),
                              margin: const EdgeInsets.only(bottom: 5),
                            ),
                          onLongPress: () async {
                            //Delete Message
                            bool doIt = await showMessageDialogYesNo(context, "Löschen", "Wollen Sie die Nachricht wirklich löschen?");
                            if(doIt){
                              int messageID = (await db.GetMessagesWith(client.userID, contactID))[index].MessageID;
                              client.DeleteMessage(rsa, messageID);
                              db.DeleteMessage(client.userID, messageID);
                            }
                          },
                        );
                      },
                    ),
                ),
                Row(
                  children: [
                    Expanded(
                      child:
                        TextField(
                          keyboardType: TextInputType.multiline,
                          minLines: 1,
                          maxLines: 5,
                          decoration: const InputDecoration(
                            labelText: 'Nachricht',
                          ),
                          controller: editMessage,
                        ),
                    ),
                    ElevatedButton(
                      onPressed: _handleSendButton,
                      child: const Icon(Icons.send),
                    ),
                  ],
                ),
              ],
              ),
        ),
    ),
      onWillPop: () async {
        _synctimer.cancel();
        return true;
      },
    );
  }

  void _handleSendButton() async{
    if(!_sending) {
      _sending = true;
      String message = editMessage.text;
      if (message != "") {
        if (await client.ServerReachable()) {
          DateTime now = DateTime.now();
          String date = now.year.toString() + "-" +
              now.month.toString().padLeft(2, '0') + "-" +
              now.day.toString().padLeft(2, '0') + " " +
              now.hour.toString().padLeft(2, '0') + ":" +
              now.minute.toString().padLeft(2, '0') + ":" +
              now.second.toString().padLeft(2, '0');
          int messageID = await client.SendMessage(rsa, contactID, message);
          db.AddMessage(MessagesCompanion(MessageID: Value(messageID),
              UserID: Value(client.userID),
              Sender: Value(client.userID),
              Receiver: Value(contactID),
              MessageStr: Value(message),
              Read: Value(0),
              Date: Value(date),
              ReceiveDate: Value("")));
          editMessage.clear();
          _updateEvent();
        } else {
          showMessageDialog(
              context, "Fehler", "Verbindung zum Server verloren.");
        }
      }
      _scrollBottom();
      setState(() {});
      _sending = false;
    }
  }

  Future<void> _updateEvent() async {
    List<Message> list = await db.GetMessagesWith(client.userID, contactID);
    bool updateNeeded = false;
    //Set Read
    for (Message m in list) {
      if(m.Receiver == client.userID) {
        if (m.Read == 0) {
          client.SetRead(rsa, m.MessageID, m.Sender);
        }
      }
      if(m.Sender == client.userID){
        for(MessageListItem item in messageList){
          if(item is SelfMessageItem){
            if(item.read != m.Read){
              updateNeeded = true;
            }
          }
        }
      }
    }
    if(list.length != messageList.length || updateNeeded) {
      setState(() {
        messageList.clear();
      });

      //Load Messages
      for (Message m in list) {
        if (m.Sender == client.userID) {
          SelfMessageItem item = SelfMessageItem(m.MessageStr, m.Date, m.Read);
          setState(() {
            messageList.add(item);
          });
        } else {
          OtherMessageItem item = OtherMessageItem(m.MessageStr, m.Date);
          setState(() {
            messageList.add(item);
          });
        }
      }
    }


  }
}
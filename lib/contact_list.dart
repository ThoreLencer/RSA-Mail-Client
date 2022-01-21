import 'dart:async';
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:rsamail/contact_add.dart';
import 'package:rsamail/login_page.dart';
import 'package:rsamail/mail_sync.dart';
import 'package:rsamail/utils.dart';
import 'package:rsamail/network.dart';
import 'package:rsamail/rsa_encryptor.dart';

import 'chat_view.dart';
import 'database.dart';

class ContactList extends StatefulWidget {
  final Network client;
  final RSAEncryptor rsa;
  final MailDatabase db;
  const ContactList({Key? key, required this.client, required this.rsa, required this.db}) : super(key: key);

  @override
  State<StatefulWidget> createState() => ContactListState(client, rsa, db);
}
class ContactListState extends State<ContactList> with TickerProviderStateMixin {
  late Network client;
  late RSAEncryptor rsa;
  late TabController tabController;
  late MailDatabase db;
  late MailSynchronizer sync;
  late Timer _synctimer;
  List<String> contactNames = [];
  List<String> contactLastMessages = [];

  ContactListState(Network network, RSAEncryptor encryptor, MailDatabase mailDatabase){
    client = network;
    rsa = encryptor;
    db = mailDatabase;
  }

  @override
  void initState() {
    super.initState();
    tabController = TabController(length: 2, vsync: this);
    checkSaveFile();
    if(client.loggedIn == true) {
      sync = MailSynchronizer(client, rsa, db);
      _updateEvent();
      _synctimer = Timer.periodic(const Duration(seconds: 1), (timer) {
        _updateEvent();
      });
    }
  }

  void checkSaveFile() async {
    //Check for saved LogIn Data
    File saveFile = File(await getSavePath()+"/save.data");
    if(!client.loggedIn && await saveFile.exists()){
      //Lade Daten
      List<String> lines = await saveFile.readAsLines();
      client.serverIP = lines[0];
      client.username = lines[1];
      client.password = lines[2];
      if(await client.ServerReachable()) {
        int userID = await client.GetUserID(client.username);
        client.userID = userID;
        rsa.loadPrivKey(client.username);
        rsa.pubKey.e =
            BigInt.parse(await client.GetE(userID), radix: 16);
        rsa.pubKey.n =
            BigInt.parse(await client.GetN(userID), radix: 16);
        client.loggedIn = true;
        sync = MailSynchronizer(client, rsa, db);
        _updateEvent();
        _synctimer = Timer.periodic(const Duration(seconds: 1), (timer) {
          _updateEvent();
        });
      }
    }
    if(!client.loggedIn || (!await saveFile.exists() && !client.loggedIn)) {
      //Timer(const Duration(milliseconds: 250), _switchToLoginPage);
      Future.delayed(Duration.zero, _switchToLoginPage);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: const Text('RSA-Mail-Client'),
          actions: <Widget>[
            PopupMenuButton(onSelected: _menuSelected,itemBuilder: (BuildContext context){
              return {'Abmelden'}.map((String choice){
                return PopupMenuItem(value: choice, child: Text(choice));
              }).toList();
            }),
          ],
          bottom: TabBar(
            controller: tabController,
            tabs: const [
              Tab(
                icon: Icon(Icons.person),
                text: "Kontakte",
              ),
              Tab(
                icon: Icon(Icons.group),
                text: "Gruppen",
              )
            ],
          ),
        ),
      body: TabBarView(
        controller: tabController,
        children: [
          Container(
            padding: const EdgeInsets.all(20.0),
            child: Column(
              children: [
                Expanded(
                  child: ListView.builder(
                    itemCount: contactNames.length,
                    itemBuilder: (BuildContext context, int index){
                      return InkWell(
                        child: Container(
                          margin: const EdgeInsets.only(bottom: 5),
                          color: Colors.black12,
                          child: Column(
                            crossAxisAlignment: CrossAxisAlignment.start,
                            children: [
                              Text(contactNames[index], style: const TextStyle(fontSize: 20),),
                              Text(contactLastMessages[index], maxLines: 1,)
                            ],
                          ),
                        ),
                        onTap: () async {
                          //Move to Chat Activity
                          int contactID = await db.GetContactID(client.userID, contactNames[index]);
                          Navigator.push(context, MaterialPageRoute(
                              builder: (context) => ChatView(rsa: rsa,client: client, db: db, contactID: contactID)));
                        },
                      );
                    },
                  ),
                ),
                ElevatedButton(
                  onPressed: _addContact,
                  child: const Icon(Icons.person_add),
                ),
              ]
            ),
          ),
          Container(
            padding: const EdgeInsets.all(20.0),
            child: Column(
              children:[
                const Spacer(),
                ElevatedButton(
                onPressed: _addGroup,
                child: const Icon(Icons.group_add),
                ),
              ]
            ),
          )
        ],
      ),
    );
  }

  Future<void> _updateEvent() async{
    contactNames.clear();
    contactLastMessages.clear();
    List<Contact> contacts = await db.GetContacts(client.userID);
    for(Contact c in contacts){
      contactNames.add(c.Name);
      Message m = await db.GetLastMessageWith(client.userID, c.ContactID);
      if(m.ID != -1) {
        contactLastMessages.add(m.MessageStr);
      } else {
        contactLastMessages.add("");
      }
    }
    setState(() {});
  }

  void _addContact(){
    Navigator.push(context, MaterialPageRoute(
        builder: (context) => ContactAddPage(db: db,client: client,)));
  }

  void _addGroup(){
    showMessageDialog(context, "Hinweis", "Diese Funktion befindet sich noch in der Entwicklungsphase.");
  }

  void _switchToLoginPage(){
    Navigator.pushReplacement(context, MaterialPageRoute(
        builder: (context) => LoginPage(rsa: rsa, client: client, db: db,)));
  }

  void _menuSelected(String value){
    if(value == "Abmelden"){
      client.loggedIn = false;
      _synctimer.cancel();
      Navigator.pushReplacement(context, MaterialPageRoute(
          builder: (context) => LoginPage(rsa: rsa, client: client, db: db,)));
    }
  }

}
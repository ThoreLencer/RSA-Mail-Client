import 'package:drift/drift.dart' hide Column;
import 'package:flutter/material.dart';
import 'package:rsamail/utils.dart';
import 'database.dart';
import 'network.dart';

class ContactAddPage extends StatefulWidget {
  final MailDatabase db;
  final Network client;

  const ContactAddPage({Key? key, required this.db, required this.client}) : super(key: key);

  @override
  State<StatefulWidget> createState() => ContactAddPageState(db, client);
}
class ContactAddPageState extends State<ContactAddPage> {
  late MailDatabase db;
  late Network client;
  TextEditingController editName = TextEditingController();

  ContactAddPageState(MailDatabase mailDatabase, Network network){
    db = mailDatabase;
    client = network;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Kontakt hinzufügen'),
      ),
      body:
        Container(
          padding: const EdgeInsets.all(30),
          child: Column(
            children: [
              TextField(
                decoration: const InputDecoration(
                  labelText: 'Benutzername',
                ),
                controller: editName,
              ),
              Container(child:ElevatedButton(onPressed: _addContact, child: const Icon(Icons.person_add)), margin: const EdgeInsets.only(top: 10),)
            ],
          ),
        ),
    );
  }

  void _addContact()async{
    if(await client.ServerReachable()){
      String name = editName.text;
      if(name != ""){
        int id = await client.GetUserID(name);
        if(id != -1){
          if(!await db.ContainsContact(client.userID, id)){
            db.AddContact(ContactsCompanion(UserID: Value(client.userID), ContactID: Value(id), Name: Value(name)));
            Navigator.pop(context);
          } else {
            showMessageDialog(context, "Fehler", "Der angegebene Benutzer befindet sich bereits in ihrer Kontaktliste.");
          }
        } else {
          showMessageDialog(context, "Fehler", "Der angegebene Benutzer ist nicht registriert.");
        }
      } else {
        showMessageDialog(context, "Fehler", "Bitte geben Sie einen Benutzername an.");
      }
    } else {
      showMessageDialog(context, "Fehler", "Verbindung zum Server abgebrochen.");
    }
  }

}
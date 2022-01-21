import 'dart:io';

import 'package:flutter/rendering.dart';
import 'package:rsamail/database.dart';
import 'package:file_picker/file_picker.dart';

import 'contact_list.dart';
import 'utils.dart';
import 'package:flutter/material.dart';
import 'package:rsamail/utils.dart';
import 'package:rsamail/rsa_encryptor.dart';
import 'network.dart';

class LoginPage extends StatefulWidget {
  final Network client;
  final RSAEncryptor rsa;
  final MailDatabase db;

  const LoginPage({Key? key, required this.client, required this.rsa, required this.db}) : super(key: key);

  @override
  State<StatefulWidget> createState() => LoginFormularState(client, rsa, db);
}
class LoginFormularState extends State<LoginPage> {
  final GlobalKey<FormState> _formKey = GlobalKey<FormState>();
  bool _saveData = false;
  bool _registering = false;
  bool _logging_in = false;
  late MailDatabase db;

  final TextEditingController editIP = TextEditingController(
      text: "rsamail.dynv6.net");
  final TextEditingController editName = TextEditingController();
  final TextEditingController editPassword = TextEditingController();

  late Network client;
  late RSAEncryptor rsa;

  LoginFormularState(Network net, RSAEncryptor encryptor, MailDatabase mailDatabase){
    client = net;
    rsa = encryptor;
    db = mailDatabase;
  }

  @override
  void initState() {
    super.initState();
    checkSaveFile();
    rsa = RSAEncryptor();
  }

  void checkSaveFile() async{
    File saveFile = File(await getSavePath()+"/save.data");
    if(await saveFile.exists()){
      _saveData = true;
    } else {
      _saveData = false;
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('RSA-Mail-Client'),
      ),
      body: Container(
        padding: const EdgeInsets.all(30.0),
        child: SingleChildScrollView(child:Form(
          key: _formKey,
          child: Column(
            children: [
              TextFormField(
                decoration: const InputDecoration(
                  icon: Icon(Icons.public),
                  labelText: 'IP-Adresse',
                ),
                controller: editIP,
              ),
              TextFormField(
                decoration: const InputDecoration(
                  icon: Icon(Icons.account_box),
                  labelText: 'Benutzername',

                ),
                controller: editName,
              ),
              TextFormField(
                obscureText: true,
                controller: editPassword,
                decoration: const InputDecoration(
                  icon: Icon(Icons.vpn_key),
                  labelText: 'Passwort',
                ),
                validator: (value) {
                  return null;
                },

              ),

              Container(
                padding: const EdgeInsets.all(10),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    const Text("Angemeldet bleiben: "),
                    Switch(
                      value: _saveData,
                      onChanged: (bool value) {
                        setState(() {
                          _saveData = value;
                        });
                      },
                    ),
                  ],
                ),

              ),
              Container(
                padding: const EdgeInsets.all(10),
                child: Column(
                  children: [
                    SizedBox(
                      child: ElevatedButton(
                        child: Text(_logging_in ? "Melde an..." : "Anmelden"),
                        onPressed: _handleLoginButton,

                      ),
                      width: 300,
                    ),
                    Container(
                      margin: const EdgeInsets.only(top: 10),
                      child: SizedBox(
                        child: ElevatedButton(
                          child: Text(_registering
                              ? "Registriere..."
                              : "Registrieren"),
                          onPressed: _handleRegisterButton,
                        ),
                        width: 300,
                      ),
                    ),
                        Container(
                          margin: const EdgeInsets.only(top: 50),
                          child: SizedBox(
                            child: ElevatedButton(
                              child: Text("Schlüssel importieren", textAlign: TextAlign.center,),
                              onPressed: _importKey,
                            ),
                            width: 300,
                          ),
                        ),
                        Container(
                          margin: const EdgeInsets.only(top: 10),
                          child: SizedBox(
                            child: ElevatedButton(
                              child: Text("Schlüssel exportieren", textAlign: TextAlign.center,),
                              onPressed: _exportKey,
                            ),
                            width: 300,
                          ),
                        ),
                      ],
                    ),
              ),
            ],
          ),
        ),
        ),
      ),
    );
  }

  Future<void> _exportKey() async {
    if(editName.text != "") {
      String? outputPath = await FilePicker.platform.getDirectoryPath(dialogTitle: 'Schlüssel exportieren',);

      if (outputPath != null) {
        File keyFile = File(await getSavePath()+"/"+editName.text + ".key");
        if(await keyFile.exists()){
          File new_file = File(outputPath + "/"+editName.text + ".key");
          if(!await new_file.exists()){
            new_file.create();
          }
          new_file.writeAsString(await keyFile.readAsString());
        } else {
          showMessageDialog(context, "Fehler", "Der angegebene Benutzer ist nicht auf diesem Gerät registriert.");
        }
      }
    } else {
      showMessageDialog(context, "Fehler", "Geben Sie den Namen des Benutzers ein!");
    }
  }

  Future<void> _importKey() async {
    FilePickerResult? result = await FilePicker.platform.pickFiles();

    if (result != null) {
      File file = File(result.files.single.path.toString());
      String line = await file.readAsString();
      File new_file = File(await getSavePath()+"/"+result.files.single.name);
      new_file.writeAsString(line);
    } else {
      // User canceled the picker
    }
  }

  void _handleLoginButton() async {
    if (_logging_in == false) {
      _logging_in = true;
      setState(() {});
      client.serverIP = editIP.text;
      if (await client.ServerReachable()) {
        if (await client.GetVersion() == client.Client_Version) {
          client.username = editName.text;
          client.password = editPassword.text;

          if (client.username != "" && client.password != "" &&
              client.serverIP != "") {
            int userID = await client.GetUserID(client.username);
            client.userID = userID;
            if (userID != -1) {
              if (await rsa.keyExists(client.username)) {
                rsa.loadPrivKey(client.username);
                rsa.pubKey.e =
                    BigInt.parse(await client.GetE(userID), radix: 16);
                rsa.pubKey.n =
                    BigInt.parse(await client.GetN(userID), radix: 16);

                if (await client.PasswordValid(rsa)) {
                  //Go to MailListActivity
                  client.loggedIn = true;
                  //Save Data
                  if(_saveData){
                    File saveFile = File(await getSavePath()+"/save.data");
                    saveFile.writeAsString(client.serverIP + "\n"+ client.username + "\n"+client.password);
                  }
                  Navigator.pushReplacement(context, MaterialPageRoute(
                      builder: (context) => ContactList(rsa: rsa,client: client, db: db,)));
                } else {
                  showMessageDialog(context, "Fehler",
                      "Das eingegebene Passwort ist falsch!");
                }
              } else {
                showMessageDialog(context, "Fehler",
                    "Der angegebene Benutzer wurde nicht auf diesem Gerät registriert.");
              }
            } else {
              showMessageDialog(context, "Fehler",
                  "Der angegebene Benutzer ist nicht registriert.");
            }
          } else {
            showMessageDialog(
                context, "Fehler", "Bitte füllen Sie alle Felder aus.");
          }
        } else {
          showMessageDialog(context, "Fehler",
              "Diese Version ist veraltet. Bitte laden Sie sich die neuste Version herunter!");
        }
      } else {
        showMessageDialog(context, "Fehler",
            "Es konnte keine Verbindung zum Server aufgebaut werden.");
      }
      _logging_in = false;
      setState(() {});
    }
  }

  void _handleRegisterButton() async {
    if (_registering == false) {
      _registering = true;
      setState(() {});
      client.username = editName.text;
      client.password = editPassword.text;
      client.serverIP = editIP.text;

      if (client.username != "" && client.password != "" &&
          client.serverIP != "") {
        if (await client.ServerReachable()) {
          if (await client.GetVersion() == client.Client_Version) {
            if (client.PasswordSecure(client.password)) {
              int userID = -1;
              userID = await client.GetUserID(client.username);
              if (userID == -1) {
                rsa.generateKeyPair(1024);
                if (rsa.verifyKey()) {
                  rsa.savePrivKey(client.username);
                  if (await client.CreateUser(rsa)) {
                    showMessageDialog(context, "Erfolg",
                        "Der Benutzer wurde erfolgreich registriert. Sie können sich nun anmelden.");
                    _registering = false;
                  }
                } else {
                  showMessageDialog(context, "Schlüsselfehler",
                      "Bitte probieren Sie es erneut.");
                }
              } else {
                showMessageDialog(
                    context, "Fehler", "Dieser Benutzer existiert bereits.");
              }
            } else {
              showMessageDialog(context, "Fehler",
                  "Das eingegebene Passwort ist unsicher. (8 Zeichen. Groß- und Kleinbuchstaben. 1 Ziffer)");
            }
          } else {
            showMessageDialog(context, "Fehler",
                "Diese Version ist veraltet. Bitte laden Sie die neuste Version herunter!");
          }
        } else {
          showMessageDialog(context, "Fehler",
              "Es konnte keine Verbindung zum Server aufgebaut werden.");
        }
      } else {
        showMessageDialog(
            context, "Fehler", "Bitte füllen Sie alle Felder aus.");
      }

      _registering = false;
      setState(() {});
    }
  }
}
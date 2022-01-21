import 'dart:io' show File;
import 'package:drift/drift.dart';
import 'package:drift/native.dart';
import 'package:rsamail/utils.dart';

part 'database.g.dart';

class Messages extends Table {
  IntColumn get ID => integer().autoIncrement()();
  IntColumn get MessageID => integer()();
  IntColumn get UserID => integer()();
  IntColumn get Sender => integer()();
  IntColumn get Receiver => integer()();
  TextColumn get MessageStr => text()();
  IntColumn get Read => integer()();
  TextColumn get Date => text()();
  TextColumn get ReceiveDate => text()();
}

class Contacts extends Table {
  IntColumn get ID => integer().autoIncrement()();
  IntColumn get UserID => integer()();
  IntColumn get ContactID => integer()();
  TextColumn get Name => text()();
}

@DriftDatabase(tables: [Messages, Contacts])
class MailDatabase extends _$MailDatabase{
  MailDatabase() : super(_init());

  @override
  int get schemaVersion => 1;

  Future<List<Message>> GetMessages(int userID) async {
    List<Message> list = await (select(messages)..where((tbl) => tbl.UserID.equals(userID) & (tbl.Receiver.equals(userID) | tbl.Sender.equals(userID)))).get();
    return list;
  }
  
  Future<Message> GetLastMessageWith(int userID, int otherID) async{
    List<Message> result = await (select(messages)
        ..where((tbl) =>
        (tbl.Receiver.equals(userID) & tbl.Sender.equals(otherID)) | (tbl
            .Receiver.equals(otherID) & tbl.Sender.equals(userID)))
        ..orderBy([(t) => OrderingTerm.desc(t.MessageID)])
        ..limit(1)).get();
    if(result.isNotEmpty) {
      return result[0];
    } else {
      return Message(ID: -1, MessageID: -1, UserID: -1, Sender: -1, Receiver: -1, MessageStr: "", Read: -1, Date: "", ReceiveDate: "");
    }
  }

  Future<List<Message>> GetMessagesWith(int userID, int otherID) async {
    List<Message> result = await (select(messages)..where((tbl) => (tbl.Receiver.equals(userID) &tbl.Sender.equals(otherID)) | (tbl.Receiver.equals(otherID) & tbl.Sender.equals(userID)))).get();
    return result;
  }

  Future<List<int>> GetMessageIDs(int userID) async {
    List<Message> list = await (select(messages)..where((tbl) => tbl.UserID.equals(userID) & (tbl.Receiver.equals(userID) | tbl.Sender.equals(userID)))).get();
    List<int> result = [];
    for(Message m in list){
      result.add(m.MessageID);
    }
    return result;
  }

  Future<List<Contact>> GetContacts(int userID) async {
    List<Contact> result = await (select(contacts)..where((tbl) => tbl.UserID.equals(userID))).get();
    return result;
  }

  Future<Contact> GetContact(int userID, int contactID) async {
    Contact result = await (select(contacts)..where((tbl) => tbl.UserID.equals(userID) & tbl.ContactID.equals(contactID))).getSingle();
    return result;
  }

  Future<int> GetContactID(int userID, String name) async {
    List<Contact> result = await (select(contacts)..where((tbl) => tbl.UserID.equals(userID) & tbl.Name.equals(name))).get();
    return result[0].ContactID;
  }

  void DeleteContact(int userID, int contactID) {
    (delete(contacts)..where((tbl) => tbl.UserID.equals(userID) & tbl.ContactID.equals(contactID))).go();
  }

  void DeleteMessage(int userID, int messageID) {
    (delete(messages)..where((tbl) => tbl.UserID.equals(userID) & tbl.MessageID.equals(messageID))).go();
  }

  Future<int> GetRead(int userID, int messageID) async {
    List<Message> m = await (select(messages)..where((tbl) => tbl.UserID.equals(userID) & tbl.MessageID.equals(messageID))).get();
    return m[0].Read;
  }
  
  void SetRead(int userID, int messageID, String date){
    (update(messages)..where((tbl) => tbl.UserID.equals(userID) & tbl.MessageID.equals(messageID))).write(MessagesCompanion(Read: Value(1), ReceiveDate: Value(date)));
  }

  Future<bool> ContainsMessage(int userID, int messageID) async{
    List<Message> mes = await (select(messages)..where((tbl) => tbl.MessageID.equals(messageID) & tbl.UserID.equals(userID))).get();
    if(mes.isEmpty){
      return false;
    } else {
      return true;
    }
  }

  Future<bool> ContainsContact(int userID, int contactID) async{
    List<Contact> c = await (select(contacts)..where((tbl) => tbl.ContactID.equals(contactID) & tbl.UserID.equals(userID))).get();
    if(c.isEmpty){
      return false;
    } else {
      return true;
    }
  }

  void AddContact(ContactsCompanion contact){
    into(contacts).insert(contact);
  }

  void AddMessage(MessagesCompanion message){
    into(messages).insert(message);
  }
}

LazyDatabase _init() {
  return LazyDatabase(() async{
    String path = await getSavePath()+"/mail.db";
	print(path);
    File file = File(path);
    if(!await file.exists()){
      await file.create();
    }
    return NativeDatabase(file);
  });
}

import 'dart:async';
import 'dart:convert';
import 'dart:core';
import 'dart:io';
import 'dart:math';
import 'dart:typed_data';

import 'package:drift/drift.dart';
import 'package:rsamail/net_package.dart';
import 'package:rsamail/rsa_encryptor.dart';

import 'database.dart';

enum CmdCode {
  CMD_CREATE_USER,
  CMD_DELETE_USER,
  CMD_GET_USER_ID,
  CMD_GET_USER_DATA,
  CMD_GET_COUNT_TO_USER,
  CMD_GET_COUNT_FROM_USER,
  CMD_GET_VERSION,
  CMD_GET_SERVER_KEY,
  CMD_GET_MESSAGE_IDS_OF_USER,
  CMD_GET_MESSAGE,
  CMD_DELETE_MESSAGE,
  CMD_SEND_MESSAGE,
  CMD_USER_ALREADY_EXISTS,
  CMD_CHANGE_PASSWORD,
  CMD_MESSAGE_READ,
  CMD_MESSAGE_GET_READ,
  CMD_PLACEHOLDER_16,
  CMD_NONE
}

class Network{
  late String serverIP;
  late String username;
  late String password;
  late int userID;
  late String signatureNum;
  late RSA_Pub_Key serverKey;
  late bool loggedIn;
  final int Client_Version = 23;

  Network(){
    username = "";
    password = "";
    serverKey = RSA_Pub_Key();
    loggedIn = false;
  }

  Future<NetPackage> SendPackage(NetPackage pack) async {
    Completer<NetPackage> result = new Completer<NetPackage>();
    Socket socket = await Socket.connect(serverIP, 40000);
    Uint8List byteBuf = Uint8List.fromList(pack.GetByteStreamWithLength());
    socket.add(byteBuf);

    List<int> readBuf = [];

    await socket.listen( (Uint8List data) {
      List<int> recvBuf = List<int>.from(data);
      readBuf.addAll(recvBuf);
    },
    onDone: () {
      readBuf.removeAt(0);
      readBuf.removeAt(0);
      readBuf.removeAt(0);
      readBuf.removeAt(0);
      NetPackage response = NetPackage(CmdCode.CMD_NONE.index);
      response.SetByteStream(readBuf);
      result.complete(response);
    }
    );

    /*await socket.listen( (Uint8List data) {
        List<int> ints = Uint8List.fromList(data).buffer.asInt32List();
        int length = ints[0];
        List<int> buffer = List<int>.from(data);
        buffer.removeAt(0);
        buffer.removeAt(0);
        buffer.removeAt(0);
        buffer.removeAt(0);
        NetPackage response = NetPackage(CmdCode.CMD_NONE.index);
        response.SetByteStream(buffer);
        result.complete(response);
      }
    );*/
    return result.future;
  }

  Future<bool> ServerReachable() async {
    //Check if Reachable
    NetPackage request = new NetPackage(CmdCode.CMD_NONE.index);
    NetPackage response = await SendPackage(request);
    if(response.bytestream.length > 0){
      request = new NetPackage(CmdCode.CMD_GET_SERVER_KEY.index);
      request.Append(-1);
      response = await SendPackage(request);
      if(response.GetCmdID() == CmdCode.CMD_GET_SERVER_KEY.index){
        String e = response.GetStringValue();
        String n = response.GetStringValue();
        serverKey.e = BigInt.parse(e, radix: 16);
        serverKey.n = BigInt.parse(n, radix: 16);
      } else {
        return false;
      }
      return true;
    } else {
      return false;
    }
  }

  void DeleteUser(RSAEncryptor rsa){
    NetPackage request = new NetPackage(CmdCode.CMD_DELETE_USER.index);
    request.Append(userID);
    request.AppendString(GetOwnSignature(rsa));
    request.AppendString(signatureNum);
    SendPackage(request);
  }

  Future<int> GetUserID(String username) async {
    RSAEncryptor rsa = new RSAEncryptor();
    NetPackage request = new NetPackage(CmdCode.CMD_GET_USER_ID.index);
    request.AppendString(rsa.encryptString(username, serverKey));
    NetPackage response = await SendPackage(request);
    response.GetCmdID();
    return response.GetIntValue();
  }

  String GetOwnSignature(RSAEncryptor rsa){
    RSA_Pub_Key signKey = new RSA_Pub_Key();
    signKey.e = BigInt.parse(rsa.privKey.d.toRadixString(16), radix: 16);
    signKey.n = BigInt.parse(rsa.pubKey.n.toRadixString(16), radix: 16);
    Random rng = Random();
    int signNum = rng.nextInt(1000000000)+1000000;

    signatureNum = rsa.encryptString(signNum.toString(), serverKey);
    return rsa.encryptString(signNum.toString(), signKey);
  }

  Future<int> GetCountToUser(RSAEncryptor rsa) async {
    NetPackage request = new NetPackage(CmdCode.CMD_GET_COUNT_TO_USER.index);
    request.Append(userID);
    request.AppendString(GetOwnSignature(rsa));
    request.AppendString(signatureNum);
    NetPackage response = await SendPackage(request);
    response.GetCmdID();
    return response.GetIntValue();
  }

  Future<int> GetCountFromUser(RSAEncryptor rsa) async {
    NetPackage request = new NetPackage(CmdCode.CMD_GET_COUNT_FROM_USER.index);
    request.Append(userID);
    request.AppendString(GetOwnSignature(rsa));
    request.AppendString(signatureNum);
    NetPackage response = await SendPackage(request);
    response.GetCmdID();
    return response.GetIntValue();
  }

  Future<int> GetVersion() async {
    NetPackage request = new NetPackage(CmdCode.CMD_GET_VERSION.index);
    NetPackage response = await SendPackage(request);
    response.GetCmdID();
    return response.GetIntValue();
  }

  Future<String> GetE(int ID) async {
    NetPackage request = new NetPackage(CmdCode.CMD_GET_USER_DATA.index);
    request.Append(userID);
    request.Append(ID);
    NetPackage response = await SendPackage(request);
    response.GetCmdID();
    response.GetStringValue();
    response.GetStringValue();
    return response.GetStringValue();
  }

  Future<String> GetN(int ID) async {
    NetPackage request = new NetPackage(CmdCode.CMD_GET_USER_DATA.index);
    request.Append(userID);
    request.Append(ID);
    NetPackage response = await SendPackage(request);
    response.GetCmdID();
    response.GetStringValue();
    response.GetStringValue();
    response.GetStringValue();
    return response.GetStringValue();
  }

  Future<void> SetRead(RSAEncryptor rsa, int messageID, int recvID) async {
    RSA_Pub_Key recvKey = new RSA_Pub_Key();
    recvKey.e = BigInt.parse(await GetE(recvID), radix: 16);
    recvKey.n = BigInt.parse(await GetN(recvID), radix: 16);

    DateTime now = DateTime.now();
    String date = now.year.toString()+"-"+now.month.toString()+"-"+now.day.toString()+" "+now.hour.toString()+":"+now.minute.toString()+":"+now.second.toString();

    NetPackage request = new NetPackage(CmdCode.CMD_MESSAGE_READ.index);
    request.Append(userID);
    request.AppendString(GetOwnSignature(rsa));
    request.AppendString(signatureNum);
    request.Append(messageID);
    request.AppendString(rsa.encryptString(date, rsa.pubKey));
    request.AppendString(rsa.encryptString(date, recvKey));
    SendPackage(request);
  }

  Future<List<String>> GetRead(RSAEncryptor rsa, int messageID) async {
    List<String> result = [];
    NetPackage request = new NetPackage(CmdCode.CMD_MESSAGE_GET_READ.index);
    request.Append(userID);
    request.AppendString(GetOwnSignature(rsa));
    request.AppendString(signatureNum);
    request.Append(messageID);
    NetPackage response = await SendPackage(request);
    response.GetCmdID();
    result.add(response.GetIntValue().toString());
    result.add(response.GetStringValue());
    return result;
  }

  void DeleteMessage(RSAEncryptor rsa, int messageID){
    NetPackage request = new NetPackage(CmdCode.CMD_DELETE_MESSAGE.index);
    request.Append(userID);
    request.AppendString(GetOwnSignature(rsa));
    request.AppendString(signatureNum);
    request.Append(messageID);
    SendPackage(request);
  }

  Future<List<int>> GetMessageIDs(RSAEncryptor rsa) async {
    List<int> result = [];
    NetPackage request = new NetPackage(CmdCode.CMD_GET_MESSAGE_IDS_OF_USER.index);
    request.Append(userID);
    request.AppendString(GetOwnSignature(rsa));
    request.AppendString(signatureNum);
    NetPackage response = await SendPackage(request);
    response.GetCmdID();
    int count = response.GetIntValue();
    for(int i = 0; i < count; i++){
      result.add(response.GetIntValue());
    }

    return result;
  }

  Future<MessagesCompanion> GetMessage(RSAEncryptor rsa, int ID) async {
    MessagesCompanion result;
    NetPackage request = new NetPackage(CmdCode.CMD_GET_MESSAGE.index);
    request.Append(userID);
    request.AppendString(GetOwnSignature(rsa));
    request.AppendString(signatureNum);
    request.Append(ID);
    NetPackage response = await SendPackage(request);
    response.GetCmdID();

    int Sender = response.GetIntValue();
    int Receiver = response.GetIntValue();
    String MessageStr = rsa.decryptStringUTF8(response.GetStringValue(), rsa.pubKey, rsa.privKey);
    int read = response.GetIntValue();
    String Date = rsa.decryptString(response.GetStringValue(), rsa.pubKey, rsa.privKey);
    String ReceiveDate = rsa.decryptString(response.GetStringValue(), rsa.pubKey, rsa.privKey);

    result = MessagesCompanion(UserID: Value(userID), MessageID: Value(ID), Sender: Value(Sender), Receiver: Value(Receiver), MessageStr: Value(MessageStr), Read: Value(read), Date: Value(Date), ReceiveDate: Value(ReceiveDate));
    return result;
  }

  Future<String> GetUsername(RSAEncryptor rsa, int ID) async {
    NetPackage request = new NetPackage(CmdCode.CMD_GET_USER_DATA.index);
    request.Append(userID);
    request.Append(ID);
    NetPackage response = await SendPackage(request);
    response.GetCmdID();
    return rsa.decryptString(response.GetStringValue(), rsa.pubKey, rsa.privKey);
  }

  Future<bool> PasswordValid(RSAEncryptor rsa) async {
    NetPackage request = new NetPackage(CmdCode.CMD_GET_USER_DATA.index);
    request.Append(userID);
    request.Append(userID);
    NetPackage response = await SendPackage(request);
    response.GetCmdID();
    response.GetStringValue();
    String tmp = response.GetStringValue();
    String passwd = rsa.decryptString(tmp, rsa.pubKey, rsa.privKey);

    if(passwd == password){
      return true;
    } else {
      return false;
    }
  }

  Future<bool> CreateUser(RSAEncryptor rsa) async {
    NetPackage request = new NetPackage(CmdCode.CMD_CREATE_USER.index);
    request.AppendString(rsa.encryptString(username, serverKey));
    request.AppendString(rsa.pubKey.e.toRadixString(16));
    request.AppendString(rsa.pubKey.n.toRadixString(16));
    request.AppendString(rsa.encryptString(password, rsa.pubKey));
    NetPackage result = await SendPackage(request);
    if (result.GetCmdID() != CmdCode.CMD_CREATE_USER.index){
      return false;
    } else {
      return true;
    }
  }

  Future<int> SendMessage(RSAEncryptor rsa, int recvID, String message) async{
    RSA_Pub_Key recvKey = new RSA_Pub_Key();
    recvKey.e = BigInt.parse(await GetE(recvID), radix: 16);
    recvKey.n = BigInt.parse(await GetN(recvID), radix: 16);

    DateTime now = DateTime.now();
    String date = now.year.toString()+"-"+now.month.toString().padLeft(2, '0')+"-"+now.day.toString().padLeft(2, '0')+" "+now.hour.toString().padLeft(2, '0')+":"+now.minute.toString().padLeft(2, '0')+":"+now.second.toString().padLeft(2, '0');

    NetPackage request = new NetPackage(CmdCode.CMD_SEND_MESSAGE.index);
    request.Append(userID);
    request.AppendString(GetOwnSignature(rsa));
    request.AppendString(signatureNum);
    request.Append(recvID);
    request.AppendString(rsa.encryptStringUTF8(message, rsa.pubKey));
    request.AppendString(rsa.encryptStringUTF8(message, recvKey));
    request.AppendString(rsa.encryptString(date, rsa.pubKey));
    request.AppendString(rsa.encryptString(date, recvKey));
    NetPackage response = await SendPackage(request);
    response.GetCmdID();
    return response.GetIntValue();
  }

  bool PasswordSecure(String passwd){
    bool result = true;
    if(passwd.length < 8){
      result = false;
    }
    RegExp regExp = RegExp(
      "^(?=.*[0-9])(?=.*[a-z])(?=.*[A-Z]).+",
      caseSensitive: false,
      multiLine: false,
    );
    
    result = regExp.hasMatch(passwd);
    
    return result;
  }

}
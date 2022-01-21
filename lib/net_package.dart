import 'dart:core';

import 'dart:typed_data';


class NetPackage {
  late List<int> bytestream;

  NetPackage(int cmdID){
    bytestream = [];
    bytestream.add(cmdID);

  }
  void Append(int value){
    bytestream.add((value >> 24 & 0xFF));
    bytestream.add((value >> 16 & 0xFF));
    bytestream.add((value >> 8 & 0xFF));
    bytestream.add((value & 0xFF));
  }
  void AppendString(String value){
    int length = value.length;
    bytestream.add((length >> 24 & 0xFF));
    bytestream.add((length >> 16 & 0xFF));
    bytestream.add((length >> 8 & 0xFF));
    bytestream.add((length & 0xFF));

    for(int i = 0; i < length; i++){
      bytestream.add(value.codeUnitAt(i));
    }
  }
  int GetCmdID(){
    int cmdID = bytestream[0];
    bytestream.removeAt(0);
    return cmdID;
  }
  int GetIntValue(){
    if(bytestream.isNotEmpty) {
      Int8List list = Int8List(4)
      ..[3] = bytestream[3]
      ..[2] = bytestream[2]
      ..[1] = bytestream[1]
      ..[0] = bytestream[0];
      bytestream.removeAt(0);
      bytestream.removeAt(0);
      bytestream.removeAt(0);
      bytestream.removeAt(0);
      ByteData blob = ByteData.sublistView(list);
      return blob.getInt32(0);
    } else {
      return -1;
    }
  }
  String GetStringValue(){
    if(bytestream.length > 0) {
      int length = bytestream[3] & 0xFF;
      length |= (bytestream[2] & 0xFF) << 8;
      length |= (bytestream[1] & 0xFF) << 16;
      length |= (bytestream[0] & 0xFF) << 24;
      bytestream.removeAt(0);
      bytestream.removeAt(0);
      bytestream.removeAt(0);
      bytestream.removeAt(0);
      String value = "";
      for (int i = 0; i < length; i++) {
        if (bytestream.length > 0) {
          value += String.fromCharCode(bytestream[0]);
    bytestream.removeAt(0);
    }
    }
    return value;
    } else {
    return "";
    }
  }
  List<int> GetByteStreamWithLength(){
    int length = bytestream.length;
    bytestream.insert(0, (length >> 24 & 0xFF));
    bytestream.insert(0, (length >> 16 & 0xFF));
    bytestream.insert(0, (length >> 8 & 0xFF));
    bytestream.insert(0, (length & 0xFF));
    return bytestream;
  }

  void SetByteStream(List<int> value){
    bytestream = value;
  }

}
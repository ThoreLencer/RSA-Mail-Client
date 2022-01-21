import 'dart:convert';
import 'dart:io' show Directory, File, Platform;
import 'package:path_provider/path_provider.dart';
import 'package:ninja_prime/ninja_prime.dart';
import 'package:rsamail/utils.dart';

class RSA_Priv_Key{
  late BigInt d;
}

class RSA_Pub_Key{
  late BigInt e, n;
}

class RSAEncryptor {
  BigInt generateRandomPrime(int bits){
    return randomPrimeBigInt(bits);
  }

  void generateKeyPair(int bits){
    privKey = new RSA_Priv_Key();
    pubKey = new RSA_Pub_Key();
    BigInt p, q, n, e, d, eulerP, psub1, qsub1, gcd;
    p = generateRandomPrime(bits);
    q = generateRandomPrime(bits);
    n = p * q;
    psub1 = p - BigInt.one;
    qsub1 = q - BigInt.one;
    eulerP = psub1 * qsub1;

    do {
      e = generateRandomPrime(bits);
      gcd = eulerP.gcd(e);
    } while (gcd != BigInt.one);

    d = e.modInverse(eulerP);
    
    privKey.d = d;
    pubKey.e = e;
    pubKey.n = n;
  }
  String encryptString(String value, RSA_Pub_Key key){
    List<String> blocks = [];
    String result = "";
    BigInt decrypted, encrypted, asciiCode;
    decrypted = BigInt.zero;

    //Pack Chars into one Int
    int blockID = 0;
    for (int i = 0; i < value.length; i++){
      asciiCode = BigInt.from(value.codeUnitAt(i));
      decrypted <<= 8;
      decrypted |= asciiCode;
      blockID ++;
      if(blockID == 32 || i == value.length -1){
        blocks.add(decrypted.toRadixString(16));
        blockID = 0;
        decrypted = BigInt.zero;
      }
    }

    //Encrypt Blocks
    for (int i = 0; i < blocks.length; i++){
      decrypted = BigInt.parse(blocks[i], radix: 16);
      encrypted = decrypted.modPow(key.e, key.n);
      result = result + encrypted.toRadixString(16)+ ":";
    }
    return result;
  }
  String decryptString(String value, RSA_Pub_Key key1, RSA_Priv_Key key2){
    List<String> blocks = [];
    String result = "";
    BigInt decrypted, encrypted, asciiCode, byte8;
    byte8 = BigInt.from(255);

    //Get Blocks
    String tmp = "";
    for(int i = 0; i < value.length; i++){
      if (value[i] != ':'){
        tmp = tmp + value[i];
      } else {
        blocks.add(tmp);
        tmp = "";
      }
    }

    //decrypt blocks
    String wtmp;
    for(int i = 0; i < blocks.length; i++){
      encrypted = BigInt.parse(blocks[i], radix: 16);
      decrypted = encrypted.modPow(key2.d, key1.n);
      //unpack chars
      wtmp = "";
      int blockCount = 0;
      do {
        asciiCode = decrypted & byte8;
        //Shift
        decrypted >>= 8;
        wtmp = String.fromCharCode(asciiCode.toInt()) + wtmp;
        blockCount++;
        if(blockCount == 32){
          break;
        }
      } while (decrypted!= BigInt.zero);
      result = result + wtmp;
    }

    return result;
  }
  String encryptStringUTF8(String value, RSA_Pub_Key key){
    List<String> blocks = [];
    String result = "";
    BigInt decrypted, encrypted, byteCode;
    decrypted = BigInt.zero;

    List<int> valueUTF8 = utf8.encode(value);
    //Pack Chars into one Int
    int blockID = 0;
    for (int i = 0; i < valueUTF8.length; i++){
      byteCode = BigInt.from(valueUTF8[i]);
      byteCode &= BigInt.parse("FF", radix: 16);
      decrypted <<= 8;
      decrypted |= byteCode;
      blockID ++;
      if(blockID == 32 || i == valueUTF8.length -1){
        blocks.add(decrypted.toRadixString(16));
        blockID = 0;
        decrypted = BigInt.zero;
      }
    }

    //Encrypt Blocks
    for (int i = 0; i < blocks.length; i++){
      decrypted = BigInt.parse(blocks[i], radix: 16);
      encrypted = decrypted.modPow(key.e, key.n);
      result = result + encrypted.toRadixString(16) + ":";
    }
    return result;
  }

  String decryptStringUTF8(String value, RSA_Pub_Key key1, RSA_Priv_Key key2){
    List<String> blocks = [];
    String result = "";
    BigInt decrypted, encrypted, byteCode, byte8;
    byte8 = BigInt.from(255);

    //Get Blocks
    String tmp = "";
    for(int i = 0; i < value.length; i++){
      if (value[i] != ':'){
        tmp = tmp + value[i];
      } else {
        blocks.add(tmp);
        tmp = "";
      }
    }

    //decrypt blocks
    List<int> wtmp;
    List<int> resultBytes = [];
    for(int i = 0; i < blocks.length; i++){
      encrypted = BigInt.parse(blocks[i], radix: 16);
      decrypted = encrypted.modPow(key2.d, key1.n);
      //unpack chars
      wtmp = [];
      int blockCount = 0;
      do {
        byteCode = decrypted & byte8;
        //Shift
        decrypted >>= 8;
        wtmp.insert(0, byteCode.toInt());
        blockCount++;
        if(blockCount == 32){
          break;
        }
      } while (decrypted != BigInt.zero);
      resultBytes.addAll(wtmp);
    }
    result = utf8.decode(resultBytes);
    return result;
  }

  bool verifyKey(){
    if(decryptString(encryptString("Hallo", pubKey), pubKey, privKey) == "Hallo"){
      return true;
    } else {
      return false;
    }
  }

  void savePrivKey(String username) async{
    File file = File(await getSavePath() + "/"+username+".key");
    file.writeAsString(privKey.d.toRadixString(16));
  }

  void loadPrivKey(String username) async{
    try {
      File file = File(await getSavePath()+ "/"+username+".key");

      // Read the file
      String line = await file.readAsString();
      privKey.d = BigInt.parse(line, radix: 16);

    } catch (e) {
      print(e.toString());
    }
  }

  Future<bool> keyExists(String username) async {
    try {
      File file = File(await getSavePath()+ "/"+username+".key");
      return await file.exists();
    } catch (e) {
      print(e.toString());
    }
    return false;
  }

  RSAEncryptor(){
    privKey = new RSA_Priv_Key();
    pubKey = new RSA_Pub_Key();
  }

  late RSA_Priv_Key privKey;
  late RSA_Pub_Key pubKey;
}
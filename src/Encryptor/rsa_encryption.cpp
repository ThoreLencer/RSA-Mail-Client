// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include "rsa_encryption.h"

bool RSA_Encryptor::checkPrime(mpz_ptr value) {
    bool output = true;

    mpz_t base, powmval, valsub;

    for (int i = 0; i < 20; i++){
        mpz_init(base);
        mpz_set_si(base, firstPrimes[i]);
        mpz_init(powmval);
        mpz_init(valsub);

        mpz_sub_ui(valsub, value, 1);
        mpz_powm(powmval, base, valsub, value);

        if (mpz_cmp_si(powmval, 1) != 0)
            output = false;
        
        mpz_clear(powmval);
        mpz_clear(valsub);
        mpz_clear(base);
    }

    return output;
}

void RSA_Encryptor::generateRandomPrime(mpz_ptr value, int bits) {
    gmp_randstate_t randState;
    gmp_randinit_mt(randState);
    gmp_randseed_ui(randState, time(NULL));

    mpz_t randPrime;

    do
    {
       mpz_init(randPrime);
       mpz_urandomb(randPrime, randState, bits);
    } while (checkPrime(randPrime) == false);

    mpz_init(value);
    mpz_set(value, randPrime);
    mpz_clear(randPrime);
    gmp_randclear(randState);
}

void RSA_Encryptor::generateKeyPair(wxProgressDialog* dlg, int bits) {
    mpz_t p, q, n, e, d, eulerP, psub1, qsub1, gcd;
    if(dlg != NULL)
        dlg->Update(0);
    mpz_init(q);
    mpz_init(e);
    mpz_init(n);
    mpz_init(d);
    mpz_init(eulerP);
    mpz_init(psub1);
    mpz_init(qsub1);
    mpz_init(gcd);
    if(dlg != NULL)
        dlg->Update(5);
    //Zwei zufällige Primzahlen generieren
    generateRandomPrime(p, bits);
    if(dlg != NULL)
        dlg->Update(25);
    generateRandomPrime(q, bits);
    if(dlg != NULL)
        dlg->Update(50);
    //Produkt bilden
    mpz_mul(n, p, q);
    //eulersches Produkt
    mpz_sub_ui(psub1, p, 1);
    mpz_sub_ui(qsub1, q, 1);
    mpz_mul(eulerP, psub1, qsub1);
    if(dlg != NULL)
        dlg->Update(55);
    //e muss teilerfremd zu eulerP sein:
    do
    {
        mpz_init(e);
        generateRandomPrime(e, bits);
        mpz_gcd(gcd, eulerP, e);
    } while (mpz_cmp_si(gcd, 1) != 0);
    if(dlg != NULL)
        dlg->Update(75);
    //d ist das multiplikative inverse e modulo eulerP:
    mpz_invert(d, e, eulerP);

    mpz_init_set(privKey.p, p);
    mpz_init_set(privKey.q, q);
    mpz_init_set(privKey.d, d);
    mpz_init_set(pubKey.e, e);
    mpz_init_set(pubKey.n, n);
    if(dlg != NULL)
        dlg->Update(90);

    mpz_clear(q);
    mpz_clear(e);
    mpz_clear(n);
    mpz_clear(d);
    mpz_clear(eulerP);
    mpz_clear(psub1);
    mpz_clear(qsub1);
    mpz_clear(gcd); 
    if(dlg != NULL)
        dlg->Update(100);
}

std::string RSA_Encryptor::getD(){
    char* buf = (char*)malloc(mpz_sizeinbase(privKey.d, 16)) + 2;
    mpz_get_str(buf, 16, privKey.d);

    return buf;
}

std::string RSA_Encryptor::getE(){
    char* buf = (char*)malloc(mpz_sizeinbase(pubKey.e, 16)) + 2;
    mpz_get_str(buf, 16, pubKey.e);

    return buf;
}

std::string RSA_Encryptor::getN(){
    char* buf = (char*)malloc(mpz_sizeinbase(pubKey.n, 16)) + 2;
    mpz_get_str(buf, 16, pubKey.n);

    return buf;
}

std::string RSA_Encryptor::encryptInt(int value){
    mpz_t encrypted, decrypted;
    mpz_init_set_ui(decrypted, value);
    mpz_init(encrypted);

    mpz_powm(encrypted, decrypted, pubKey.e, pubKey.n);

    char* buf = (char*)malloc(mpz_sizeinbase(encrypted, 16)) + 2;
    mpz_get_str(buf, 16, encrypted);

    mpz_clear(encrypted);
    mpz_clear(decrypted);

    return buf;
}

int RSA_Encryptor::decryptInt(std::string value){
    mpz_t encrypted, decrypted;
    mpz_init_set_str(encrypted, value.c_str(), 16);
    mpz_init(decrypted);

    mpz_powm(decrypted, encrypted, privKey.d, pubKey.n);

    int output;
    output = mpz_get_ui(decrypted);

    mpz_clear(encrypted);
    mpz_clear(decrypted);
    return output;
}

bool RSA_Encryptor::verifyKey(){
    int randInt;
    srand(time(NULL));
    randInt = rand() % 1024 + 1;
    if (decryptInt(encryptInt(randInt)) == randInt){
        return true;
    } else {
        return false;
    }
}

std::string RSA_Encryptor::encryptString(std::wstring value, RSA_Pub_Key key){
    std::vector<std::string> blocks;
    std::string result;
    mpz_t decrypted, encrypted, asciiCode;
    mpz_init(encrypted);
    mpz_init(decrypted);
    //Pack Chars into one Int
    int blockID = 0;
    for (int i = 0; i < value.length(); i++){
        mpz_init(asciiCode);
        mpz_set_si(asciiCode, value[i]);
        //shift int right
        mpz_mul_2exp(decrypted, decrypted, 8);
        //put char to last 8 bit
        mpz_ior(decrypted, decrypted, asciiCode);
        mpz_clear(asciiCode);

        blockID ++;
        if(blockID == 32 || i == value.length() - 1){
            //save block
            char* buf = (char*)malloc(mpz_sizeinbase(decrypted, 16) + 2);
            mpz_get_str(buf, 16, decrypted);
            blocks.push_back(buf);
            mpz_init(decrypted);
            blockID = 0;
        }
    }
    //encrypt blocks
    for (int i = 0; i < blocks.size(); i++){
        mpz_init_set_str(decrypted, blocks.at(i).c_str(), 16);
        mpz_init(encrypted);
        mpz_powm(encrypted, decrypted, key.e, key.n);
        char* buf = (char*)malloc(mpz_sizeinbase(encrypted, 16) + 2);
        mpz_get_str(buf, 16, encrypted);
        result = std::string(result + buf + ":");
    }


    mpz_clear(encrypted);
    mpz_clear(decrypted);
    return result;
}

std::wstring RSA_Encryptor::decryptString(std::string value, RSA_Pub_Key key1, RSA_Priv_Key key2){
    std::vector<std::string> blocks;
    std::wstring result = L"";
    mpz_t decrypted, encrypted, asciiCode, byte8;
    mpz_init(encrypted);
    mpz_init(decrypted);
    mpz_init_set_si(byte8, 255);

    //get blocks
    std::string tmp;
    for(int i = 0; i < value.length(); i++){
        if (value.at(i) != ':'){
            tmp = std::string(tmp + value.at(i));
        } else {
            blocks.push_back(tmp);
            tmp = "";
        }
    }
    std::wstring wtmp;
    //decrypt blocks
    for(int i = 0; i < blocks.size();i++){
        mpz_init(decrypted);
        mpz_init_set_str(encrypted, blocks.at(i).c_str(), 16);
        mpz_powm(decrypted, encrypted, key2.d, key1.n);
        //unpack chars
        wtmp = L"";
        int blockCount = 0;
        do
        {
            mpz_init(asciiCode);
            mpz_and(asciiCode, decrypted, byte8);
            //shift 8 bits
            mpz_tdiv_q_2exp(decrypted, decrypted, 8);
            wtmp = std::wstring((wchar_t)mpz_get_ui(asciiCode) + wtmp);
            mpz_clear(asciiCode);
            blockCount ++;
            if(blockCount == 32){
                break;
            }
        } while (mpz_cmp_si(decrypted, 0) != 0);
        result = std::wstring(result + wtmp);
    }
    mpz_clear(encrypted);
    mpz_clear(decrypted);
    mpz_clear(byte8);
    return result;
}

RSA_Priv_Key RSA_Encryptor::getPrivKey(){
    return privKey;
}

RSA_Pub_Key RSA_Encryptor::getPubKey(){
    return pubKey;
}

void RSA_Encryptor::savePrivKey(std::string filename){
    std::ofstream output_file(filename, std::ios::out);
    output_file << getD();
    output_file.close();
}

void RSA_Encryptor::loadPrivKey(std::string filename){
    std::ifstream input_file(filename, std::ios::in);
    std::string d;
    char c = input_file.get();
    while(input_file.good()){
        d = d + c;
        c = input_file.get();
    }

    mpz_set_str(privKey.d, d.c_str(), 16);
    input_file.close();
}

void RSA_Encryptor::setE(std::string value){
    mpz_set_str(pubKey.e, value.c_str(), 16);
}

void RSA_Encryptor::setN(std::string value){
    mpz_set_str(pubKey.n, value.c_str(), 16);
}

void RSA_Encryptor::init(){
    mpz_init(privKey.d);
    mpz_init(privKey.p);
    mpz_init(privKey.q);
    mpz_init(pubKey.e);
    mpz_init(pubKey.n);
}
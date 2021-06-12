// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include "file_encryption.h"

std::string FILE_Encryptor::generateRandomKey(){
    std::string s = "";
    srand(time(NULL));
    std::string c = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!\"%&/()=?";
    for(int i = 0; i < 128; i++){
        int random = rand() % c.length();
        s += c[random];
    }
    return s;
}

int FILE_Encryptor::getFileLength(std::filesystem::path filename){
    std::filesystem::path p = std::filesystem::current_path() / filename;
    return std::filesystem::file_size(p);
}

void FILE_Encryptor::fileToBuffer(std::string filename, unsigned char* value){
    std:ifstream is(filename, std::ios::binary);
    if(is){
        is.read(reinterpret_cast<char*>(value), getFileLength(filename));
        is.close();
    }
}

void FILE_Encryptor::saveFileFromBuffer(std::string filename, unsigned char* value, int length){
    std::ofstream os(filename, std::ios::binary);
    os.write(reinterpret_cast<char*>(value), length);
 
    os.close();
}

void FILE_Encryptor::encryptFile(std::string srcFilename, std::string destFilename, std::string key){
    AES aes(128);
    //Read in srcFile
    int srcFileLength = getFileLength(srcFilename);
    unsigned char* srcFileContent = (unsigned char*)malloc(srcFileLength);
    fileToBuffer(srcFilename, srcFileContent);

    //Encrypt File
    unsigned int outLen;
    unsigned char *encryptedFileContent = aes.EncryptECB((unsigned char*)srcFileContent, srcFileLength, (unsigned char*)key.c_str(), outLen);

    //Save to Dest
    saveFileFromBuffer(destFilename, encryptedFileContent, outLen);

    delete srcFileContent;
    delete encryptedFileContent;
}

void FILE_Encryptor::decryptFile(std::string srcFilename, std::string destFilename, std::string key){
    AES aes(128);
    //Read in srcFile
    int srcFileLength = getFileLength(srcFilename);
    unsigned char* srcFileContent = (unsigned char*)malloc(srcFileLength);
    fileToBuffer(srcFilename, srcFileContent);

    //Decrypt File
    unsigned char *decryptedFileContent = aes.DecryptECB((unsigned char*)srcFileContent, srcFileLength, (unsigned char*)key.c_str());

    //Save to Dest
    saveFileFromBuffer(destFilename, decryptedFileContent, srcFileLength);

    delete srcFileContent;
    delete decryptedFileContent;
}
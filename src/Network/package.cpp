// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include "package.h"

Package::Package(byte_t cmdID){
    bytestream.push_back(cmdID);
}

Package::Package(){
    
}

void Package::Append(int value){
    //value 4 Bytes
    bytestream.push_back(value >> 24 & 0xFF);
    bytestream.push_back(value >> 16 & 0xFF);
    bytestream.push_back(value >> 8 & 0xFF);
    bytestream.push_back(value & 0xFF);
}

void Package::Append(std::string value){
    //length
    int length = value.length();
    bytestream.push_back(length >> 24 & 0xFF);
    bytestream.push_back(length >> 16 & 0xFF);
    bytestream.push_back(length >> 8 & 0xFF);
    bytestream.push_back(length & 0xFF);
    //value
    for(int i = 0; i < length; i++){
        bytestream.push_back(value.at(i));
    }
}

byte_t Package::GetCmdID(){
    byte_t cmdID = bytestream.at(0);
    bytestream.erase(bytestream.begin());
    return cmdID;
}

int Package::GetIntValue(){
    //value
    int value = int(bytestream.at(0) << 24 |
                    bytestream.at(1) << 16 |
                    bytestream.at(2) << 8  |
                    bytestream.at(3));
    bytestream.erase(bytestream.begin());
    bytestream.erase(bytestream.begin());
    bytestream.erase(bytestream.begin());
    bytestream.erase(bytestream.begin());
    return value;
}

std::string Package::GetStrValue(){
    //length
    int length = int(bytestream.at(0) << 24 |
                    bytestream.at(1) << 16 |
                    bytestream.at(2) << 8  |
                    bytestream.at(3));
    bytestream.erase(bytestream.begin());
    bytestream.erase(bytestream.begin());
    bytestream.erase(bytestream.begin());
    bytestream.erase(bytestream.begin());
    std::string value;
    for(int i = 0; i < length; i++){
        value += bytestream.at(0);
        bytestream.erase(bytestream.begin());
    }
    return value;
}

std::vector<byte_t> Package::GetByteStream(){
    return bytestream;
}

std::vector<byte_t> Package::GetByteStreamWithLength(){
    //Länge anfügen
    int length = bytestream.size();
    bytestream.insert(bytestream.begin(), length >> 24 & 0xFF);
    bytestream.insert(bytestream.begin(), length >> 16 & 0xFF);
    bytestream.insert(bytestream.begin(), length >> 8 & 0xFF);
    bytestream.insert(bytestream.begin(), length & 0xFF);
    return bytestream;
}

void Package::SetByteStream(byte_t* buffer, int length){
    bytestream.clear();
    for(int i = 0; i < length; i++){
        bytestream.push_back(buffer[i]);
    }
}
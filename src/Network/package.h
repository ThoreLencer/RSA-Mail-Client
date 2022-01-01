// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include <vector>
#include <string>

typedef unsigned char byte_t;

class Package{
    private:
        std::vector<byte_t> bytestream;
    public:
        Package(byte_t cmdID);
        Package();
        void Append(int value);
        void Append(std::string value);

        //Getters (do pop args)
        byte_t GetCmdID();
        int GetIntValue();
        std::string GetStrValue();

        void SetByteStream(byte_t* buffer, int length);

        std::vector<byte_t> GetByteStreamWithLength();
        std::vector<byte_t> GetByteStream();
};
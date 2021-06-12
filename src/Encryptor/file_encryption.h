// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include <string>
#include "aes_encryption.h"
#include <filesystem>
#include <fstream>

class FILE_Encryptor {
    public:
        std::string generateRandomKey();
        void encryptFile(std::string srcFilename, std::string destFilename, std::string key);
        void decryptFile(std::string srcFilename, std::string destFilename, std::string key);

        void fileToBuffer(std::string filename, unsigned char* value);
        int getFileLength(std::filesystem::path filename);
        void saveFileFromBuffer(std::string filename, unsigned char* value, int length);
    private:
};
// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include <gmp.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <wx/progdlg.h>

struct RSA_Priv_Key {
    mpz_t p, q, d;
};

struct RSA_Pub_Key {
    mpz_t e, n;
};

class RSA_Encryptor {
    private:
        void generateRandomPrime(mpz_ptr value, int bits);
        bool checkPrime(mpz_ptr value);

        RSA_Priv_Key privKey;
        RSA_Pub_Key pubKey;

        const int firstPrimes[20] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 73, 79, 83, 89, 97};
    public:
        void init();
        void generateKeyPair(wxProgressDialog* dlg, int bits);
        std::string encryptInt(int value);
        int decryptInt(std::string value);

        std::string encryptString(std::wstring value, RSA_Pub_Key key);
        std::wstring decryptString(std::string value, RSA_Pub_Key key1, RSA_Priv_Key key2);

        bool verifyKey();

        RSA_Priv_Key getPrivKey();
        RSA_Pub_Key getPubKey();

        std::string getD();
        std::string getE();
        std::string getN();

        void setE(std::string value);
        void setN(std::string value);

        void savePrivKey(std::string filename);
        void loadPrivKey(std::string filename);
};

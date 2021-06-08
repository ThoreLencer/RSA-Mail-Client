// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include <string>
#include <mysql/mysql.h>
#include <iostream>
#include "../Encryptor/rsa_encryption.h"
#include <fstream>
#include <vector>
#include <ctime>
#include <quickmail.h>

/*
    secure.h:
    const std::string namekey_d = "private rsa key for name decryption";
    const std::string namekey_e = "public rsa key (e) for name encryption";
    const std::string namekey_n = "public rsa key (n) for name encryption";
        
    const std::string db_username = "database username";
    const std::string db_password = "database password";

    const std::string verify_adress = "verify email adress";
    const std::string verify_password = "verify email password";

*/
#include "../Secure/secure.h"


#define CLIENT_VERSION 19

struct Mail_Caption{
    int ID;
    int From;
    int To;
    bool Read;
    std::wstring date;
    std::wstring receiveDate;
    std::wstring caption;
};

class Mail_Database {
    private:
        MYSQL* mysql;
        MYSQL_RES* res;
        int account_id;
        std::string account_username;
        std::wstring account_password;
        std::wstring account_email;
        std::vector<Mail_Caption> mail_captions;
        std::vector<Mail_Caption> sent_captions;
        bool connected;

        RSA_Pub_Key pubNameKey;
        RSA_Priv_Key privNameKey;
    public:
        void init();
        void setLoginInfo(std::string username, std::wstring password);
        bool connect(std::string ip);
        bool versionValid();
        bool isConnected();
        bool messagesAvailable();
        int getCurrentCaptionCount();
        int getCurrentUnreadCaptionCount();
        bool sentMessagesAvailable();
        std::vector<Mail_Caption> receiveCaptions(RSA_Encryptor* rsa);
        std::vector<Mail_Caption> receiveSentCaptions(RSA_Encryptor* rsa);
        std::wstring receiveMail(RSA_Encryptor* rsa, int ID);
        std::wstring receiveSentMail(RSA_Encryptor* rsa, int ID);
        void sendMail(RSA_Encryptor* rsa, int To, std::wstring Caption, std::wstring Message, RSA_Pub_Key key);
        bool userExists(RSA_Encryptor* rsa);
        int userExists(RSA_Encryptor* rsa, std::string value);
        RSA_Pub_Key userKey(RSA_Encryptor* rsa, std::string value);
        RSA_Pub_Key userKey(RSA_Encryptor* rsa, int value);
        void createUser(RSA_Encryptor* rsa);
        void deleteUser();
        bool passwordValid(RSA_Encryptor* rsa);
        bool passwordSecure(std::wstring passwd);
        void changePassword(RSA_Encryptor* rsa, std::wstring passwd);
        std::string getE(RSA_Encryptor* rsa);
        std::string getN(RSA_Encryptor* rsa);
        std::string getUsername();
        std::string getUsername(RSA_Encryptor* rsa, int ID);
        std::string getSender(RSA_Encryptor* rsa, int ID);
        std::string getReceiver(RSA_Encryptor* rsa, int ID);
        std::wstring getCaption(int ID);
        std::vector<Mail_Caption> getMailCaptions();
        std::vector<Mail_Caption> getSentCaptions();
        int getRead(int ID);
        int getSentRead(int ID);
        std::wstring getDate(int ID);
        std::wstring getReceiveDate(int ID);
        std::wstring getSentDate(int ID);
        std::wstring getSentReceiveDate(int ID);
        void deleteMail(int ID);
        void deleteSentMail(int ID);
        bool updateSentRead(RSA_Encryptor* rsa);
        void setRead(RSA_Encryptor* rsa, int ID);
        void updateUnderWindows();
        void updateUnderLinux();
        bool mailSignatureValid(RSA_Encryptor* rsa, int ID);
        void close();
        void exportData(std::string filename);
        bool hasEmail();
        int sendPasswordResetEmail(RSA_Encryptor* rsa);
        int sendVerificationEmail(RSA_Encryptor* rsa, std::string emailAdress);
        void setEmail(std::wstring email);
};

#pragma once
#include <boost/asio.hpp>
#include "package.h"
#include <iostream>
#include <string>
#include <vector>
#include "../Encryptor/rsa_encryption.h"
#include "../Database/database.h"
#include "../Secure/secure.h"

enum {
   CMD_CREATE_USER = 0,
   CMD_DELETE_USER,
   CMD_GET_USER_ID,
   CMD_GET_USER_DATA,
   CMD_GET_COUNT_TO_USER,
   CMD_GET_COUNT_FROM_USER,
   CMD_GET_VERSION,
   CMD_GET_SERVER_KEY,
   CMD_GET_MESSAGE_CAPTIONS_TO_USER,
   CMD_GET_MESSAGE_CAPTIONS_FROM_USER,
   CMD_GET_MESSAGE,
   CMD_DELETE_MESSAGE,
   CMD_SEND_MESSAGE,
   CMD_USER_ALREADY_EXISTS,
   CMD_CHANGE_PASSWORD,
   CMD_MESSAGE_READ,
   CMD_MESSAGE_GET_READ,
   CMD_NONE
};

class Client{
    private:
        RSA_Pub_Key pubNameKey;
        RSA_Priv_Key privNameKey;
    public:
        Client();
        Package SendPackage(Package* package);
        std::string GetUsername(RSA_Encryptor* rsa, int ID);
        int GetUserID(RSA_Encryptor* rsa, std::string userName);
        int GetVersion();

        std::tuple<std::string, std::string> GetOwnSignature(RSA_Encryptor* rsa);

        std::string GetUserE(RSA_Encryptor* rsa, int ID);
        std::string GetUserN(RSA_Encryptor* rsa, int ID);

        int GetCountToUser(RSA_Encryptor* rsa);

        int GetCountFromUser(RSA_Encryptor* rsa);

        int GetRead(RSA_Encryptor* rsa, int messageID);

        std::vector<Caption> GetCaptionsToUser(RSA_Encryptor* rsa);

        std::vector<Caption> GetCaptionsFromUser(RSA_Encryptor* rsa);

        std::wstring GetMessage(RSA_Encryptor* rsa, int ID);

        bool ChangeUserPassword(RSA_Encryptor* rsa, std::wstring value);

        bool PasswordValid(RSA_Encryptor* rsa);

        bool ServerReachable();

        void CreateUser(RSA_Encryptor* rsa);

        void SendMessage(RSA_Encryptor* rsa, int recvID, std::wstring caption, std::wstring message);

        void DeleteMessage(RSA_Encryptor* rsa, int messageID);

        void SetRead(Database* database, RSA_Encryptor* rsa, int ID);

        bool passwordSecure(std::wstring passwd);
        std::string GetDateString();

        std::string userName;
        std::string serverAddress;
        std::wstring password;
        RSA_Pub_Key serverKey;
        bool loggedIn;
        int userID;
};
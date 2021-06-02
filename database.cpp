// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include "database.h"

bool Mail_Database::connect(std::string ip){

    mysql = mysql_init(NULL);
    if (!mysql_real_connect(mysql, ip.c_str(), db_username.c_str(), db_password.c_str(), "RSA_Mail", 80, NULL, 0)) {
        std::cout << mysql_error(mysql) << std::endl;
        return false;
    } else {
        connected = true;
        return true;
    }
}

void Mail_Database::setLoginInfo(std::string username, std::wstring password){
    account_username = username;
    account_password = password;
}

bool Mail_Database::userExists(RSA_Encryptor* rsa){
    if (mysql_query(mysql, std::string("SELECT COUNT(ID) FROM `Users` WHERE `Name`=\"" + rsa->encryptString(std::wstring(account_username.begin(), account_username.end()), pubNameKey) + "\"").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    int count;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        count = atoi(row[0]);
    }

    // Release memories
    mysql_free_result(res);
    if (count == 0){
        return false;
    } else {
        //get ID
        if (mysql_query(mysql, std::string("SELECT `ID` FROM `Users` WHERE `Name`=\"" + rsa->encryptString(std::wstring(account_username.begin(), account_username.end()), pubNameKey) + "\"").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
        }
        res = mysql_use_result(mysql);
        int id;
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != NULL){
            id = atoi(row[0]);
        }
        account_id = id;
        // Release memories
        mysql_free_result(res);
        return true;
    }
}

int Mail_Database::userExists(RSA_Encryptor* rsa, std::string value) {
    int result = -1;
    if (mysql_query(mysql, std::string("SELECT `ID` FROM `Users` WHERE `Name`=\"" + rsa->encryptString(std::wstring(value.begin(), value.end()), pubNameKey) + "\"").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        result = atoi(row[0]);
    }

    // Release memories
    mysql_free_result(res);

    return result;
}

void Mail_Database::createUser(RSA_Encryptor* rsa, std::wstring email){
    if(email != "None"){
        if(mysql_query(mysql, std::string("INSERT INTO `Users`(`Name`, `E`, `N`, `Password`, `Email`) VALUES (\"" + rsa->encryptString(std::wstring(account_username.begin(), account_username.end()), pubNameKey) + "\",\"" + rsa->getE() + "\", \"" + rsa->getN() + "\", \"" + rsa->encryptString(account_password, rsa->getPubKey()) + "\", \"" + rsa->encryptString(email, rsa->getPubKey()) + "\")").c_str())){
            std::cout << mysql_error(mysql) << std::endl;
        }
    } else {
        if(mysql_query(mysql, std::string("INSERT INTO `Users`(`Name`, `E`, `N`, `Password`) VALUES (\"" + rsa->encryptString(std::wstring(account_username.begin(), account_username.end()), pubNameKey) + "\",\"" + rsa->getE() + "\", \"" + rsa->getN() + "\", \"" + rsa->encryptString(account_password, rsa->getPubKey()) + "\")").c_str())){
            std::cout << mysql_error(mysql) << std::endl;
        }
    }
    //get ID
    if (mysql_query(mysql, std::string("SELECT `ID` FROM `Users` WHERE `Name`=\"" + rsa->encryptString(std::wstring(account_username.begin(), account_username.end()), pubNameKey) + "\"").c_str())) {
    std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    int id;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        id = atoi(row[0]);
    }
    account_id = id;
    // Release memories
    mysql_free_result(res);
}

void Mail_Database::deleteUser(){
    //Delete Mails
    if (mysql_query(mysql, std::string("DELETE FROM `Messages` WHERE `From`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    if (mysql_query(mysql, std::string("DELETE FROM `Messages` WHERE `To`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    if (mysql_query(mysql, std::string("DELETE FROM `Users` WHERE `ID`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    
}

bool Mail_Database::passwordValid(RSA_Encryptor* rsa){
    if (mysql_query(mysql, std::string("SELECT `Password` FROM `Users` WHERE `Name`=\"" + rsa->encryptString(std::wstring(account_username.begin(), account_username.end()), pubNameKey) + "\"").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::string passwd;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        passwd = row[0];
    }

    // Release memories
    mysql_free_result(res);
    if (rsa->decryptString(passwd, rsa->getPubKey(), rsa->getPrivKey()) == account_password) {
        return true;
    } else {
        return false;
    }
}

std::string Mail_Database::getE(RSA_Encryptor* rsa){
    if (mysql_query(mysql, std::string("SELECT `E` FROM `Users` WHERE `Name`=\"" + rsa->encryptString(std::wstring(account_username.begin(), account_username.end()), pubNameKey) + "\"").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::string E;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        E = row[0];
    }

    // Release memories
    mysql_free_result(res);
    return E;
}

std::string Mail_Database::getN(RSA_Encryptor* rsa){
    if (mysql_query(mysql, std::string("SELECT `N` FROM `Users` WHERE `Name`=\"" + rsa->encryptString(std::wstring(account_username.begin(), account_username.end()), pubNameKey) + "\"").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::string N;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        N = row[0];
    }

    // Release memories
    mysql_free_result(res);
    return N;
}

std::string Mail_Database::getUsername(){
    return account_username;
}

std::string Mail_Database::getUsername(RSA_Encryptor* rsa, int ID){
    if (mysql_query(mysql, std::string("SELECT `Name` FROM `Users` WHERE `ID`=\"" + std::to_string(ID) + "\"").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::string Name;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        Name = row[0];
    }

    // Release memories
    mysql_free_result(res);
    std::wstring decrypt = rsa->decryptString(Name, pubNameKey, privNameKey);
    return std::string(decrypt.begin(), decrypt.end());
}

std::string Mail_Database::getSender(RSA_Encryptor* rsa, int ID){
    return getUsername(rsa, mail_captions.at(ID).From);
}

std::string Mail_Database::getReceiver(RSA_Encryptor* rsa, int ID){
    return getUsername(rsa, sent_captions.at(ID).To);
}

void Mail_Database::init() {
    connected = false;
    mpz_init_set_str(pubNameKey.e, namekey_e.c_str(), 16);
    mpz_init_set_str(pubNameKey.n, namekey_n.c_str(), 16);
    mpz_init_set_str(privNameKey.d, namekey_d.c_str(), 16);
    mpz_init(privNameKey.p);
    mpz_init(privNameKey.q);
}

bool Mail_Database::isConnected() {
    return connected;
}

void Mail_Database::close(){
    connected = false;
    mail_captions.clear();
    // Close a MySQL connection
    mysql_close(mysql);
}

bool Mail_Database::messagesAvailable() {
    if (mysql_query(mysql, std::string("SELECT COUNT(ID) FROM `Messages` WHERE `To`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    int n;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        n = atoi(row[0]);
    }

    // Release memories
    mysql_free_result(res);
    if(mail_captions.size() != n){
        return true;
    } else {
        return false;
    }
}

bool Mail_Database::sentMessagesAvailable(){
    if (mysql_query(mysql, std::string("SELECT COUNT(ID) FROM `Messages` WHERE `From`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    int n;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        n = atoi(row[0]);
    }

    // Release memories
    mysql_free_result(res);
    if(sent_captions.size() != n){
        return true;
    } else {
        return false;
    }
}

bool Mail_Database::versionValid(){
    if (mysql_query(mysql, std::string("SELECT `Version` FROM `Versioninfo` WHERE `ID`=1").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    int n;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        n = atoi(row[0]);
    }

    // Release memories
    mysql_free_result(res);
    if (n == CLIENT_VERSION){
        return true;
    } else {
        return false;
    }
}

std::vector<Mail_Caption> Mail_Database::receiveCaptions(RSA_Encryptor* rsa){
    std::vector<Mail_Caption> result;
    if (mysql_query(mysql, std::string("SELECT `ID`, `From`, `Caption`, `Read`, `Date`, `ReceiveDate` FROM `Messages` WHERE `To`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        Mail_Caption tmp;
        tmp.ID = atoi(row[0]);
        tmp.From = atoi(row[1]);
        tmp.caption = rsa->decryptString(row[2], rsa->getPubKey(), rsa->getPrivKey());
        tmp.Read = atoi(row[3]);
        tmp.date = rsa->decryptString(row[4], rsa->getPubKey(), rsa->getPrivKey());
        tmp.receiveDate = rsa->decryptString(row[5], rsa->getPubKey(), rsa->getPrivKey());
        result.push_back(tmp);
    }

    // Release memories
    mysql_free_result(res);
    mail_captions = result;
    return result;
}

std::vector<Mail_Caption> Mail_Database::receiveSentCaptions(RSA_Encryptor* rsa){
    std::vector<Mail_Caption> result;
    if (mysql_query(mysql, std::string("SELECT `ID`, `From`, `SenderCaption`, `Read`, `SenderDate`, `SenderReceiveDate`, `To` FROM `Messages` WHERE `From`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        Mail_Caption tmp;
        tmp.ID = atoi(row[0]);
        tmp.From = atoi(row[1]);
        tmp.caption = rsa->decryptString(row[2], rsa->getPubKey(), rsa->getPrivKey());
        tmp.Read = atoi(row[3]);
        tmp.date = rsa->decryptString(row[4], rsa->getPubKey(), rsa->getPrivKey());
        tmp.receiveDate = rsa->decryptString(row[5], rsa->getPubKey(), rsa->getPrivKey());
        tmp.To = atoi(row[6]);
        result.push_back(tmp);
    }

    // Release memories
    mysql_free_result(res);
    sent_captions = result;
    return result;
}

std::wstring Mail_Database::receiveMail(RSA_Encryptor* rsa, int ID){
    std::wstring result;
    if (mysql_query(mysql, std::string("SELECT `Message` FROM `Messages` WHERE `ID`=" + std::to_string(mail_captions.at(ID).ID)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        result = rsa->decryptString(row[0], rsa->getPubKey(), rsa->getPrivKey());
    }

    // Release memories
    mysql_free_result(res);
    return result;
}

std::wstring Mail_Database::receiveSentMail(RSA_Encryptor* rsa, int ID){
    std::wstring result;
    if (mysql_query(mysql, std::string("SELECT `SenderMessage` FROM `Messages` WHERE `ID`=" + std::to_string(sent_captions.at(ID).ID)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        result = rsa->decryptString(row[0], rsa->getPubKey(), rsa->getPrivKey());
    }

    // Release memories
    mysql_free_result(res);
    return result;
}

void Mail_Database::deleteMail(int ID){
    if (mysql_query(mysql, std::string("DELETE FROM `Messages` WHERE `ID`=" + std::to_string(mail_captions.at(ID).ID)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
}
void Mail_Database::deleteSentMail(int ID){
    if (mysql_query(mysql, std::string("DELETE FROM `Messages` WHERE `ID`=" + std::to_string(sent_captions.at(ID).ID)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
}

bool Mail_Database::mailSignatureValid(RSA_Encryptor* rsa, int ID){
    RSA_Pub_Key key = userKey(rsa, getUsername(rsa, mail_captions.at(ID).From));
    if (mysql_query(mysql, std::string("SELECT `Signature` FROM `Messages` WHERE `ID`=\"" + std::to_string(mail_captions.at(ID).ID) + "\"").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::string signature;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        signature = row[0];
    }

    // Release memories
    mysql_free_result(res);

    RSA_Priv_Key signKey;
    mpz_init_set(signKey.d, key.e);
    mpz_init(signKey.p);
    mpz_init(signKey.q);

    if (rsa->decryptString(signature, key, signKey) == L"Signature OK"){
        return true;
    } else {
        return false;
    }
}

void Mail_Database::changePassword(RSA_Encryptor* rsa, std::wstring passwd){
    if (mysql_query(mysql, std::string("UPDATE `Users` SET `Password`=\"" + rsa->encryptString(passwd, rsa->getPubKey()) +"\" WHERE `ID`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
}

void Mail_Database::setRead(RSA_Encryptor* rsa, int ID){
    if (mysql_query(mysql, std::string("UPDATE `Messages` SET `Read`=1 WHERE `ID`=" + std::to_string(mail_captions.at(ID).ID)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    mail_captions.at(ID).Read = true;
    //ReceiveDate
    std::time_t t = std::time(0);
    std::tm* time = std::localtime(&t);
    std::string datestring = "";
    datestring = std::to_string(time->tm_year + 1900) + '-' + std::to_string(time->tm_mon + 1) + '-' + std::to_string(time->tm_mday) + ' ' + std::to_string(time->tm_hour) + ':' + std::to_string(time->tm_min) + ':' + std::to_string(time->tm_sec);

    RSA_Pub_Key key = userKey(rsa, mail_captions.at(ID).From);

    if (mysql_query(mysql, std::string("UPDATE `Messages` SET `ReceiveDate`= \""+ rsa->encryptString(std::wstring(datestring.begin(), datestring.end()), rsa->getPubKey()) +"\" WHERE `ID`=" + std::to_string(mail_captions.at(ID).ID)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }

    if (mysql_query(mysql, std::string("UPDATE `Messages` SET `SenderReceiveDate`= \""+ rsa->encryptString(std::wstring(datestring.begin(), datestring.end()), key) +"\" WHERE `ID`=" + std::to_string(mail_captions.at(ID).ID)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }

    if (mysql_query(mysql, std::string("SELECT `ReceiveDate` FROM `Messages` WHERE `ID`=\"" + std::to_string(mail_captions.at(ID).ID) + "\"").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::wstring recvDate;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        recvDate = rsa->decryptString(row[0], rsa->getPubKey(), rsa->getPrivKey());
    }

    // Release memories
    mysql_free_result(res);
    mail_captions.at(ID).receiveDate = recvDate;
}

bool Mail_Database::updateSentRead(RSA_Encryptor* rsa){
    bool changed = false;
    if (mysql_query(mysql, std::string("SELECT `ID`, `Read`, `SenderReceiveDate` FROM `Messages` WHERE `From`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        for(int i = 0; i < sent_captions.size(); i++){
            if (sent_captions.at(i).ID == atoi(row[0])){
                if (sent_captions.at(i).Read != atoi(row[1])){
                    sent_captions.at(i).Read = atoi(row[1]);
                    sent_captions.at(i).receiveDate = rsa->decryptString(row[2], rsa->getPubKey(), rsa->getPrivKey());
                    changed = true;
                }
                break;
            }
        }
    }

    // Release memories
    mysql_free_result(res);

    return changed;
}

int Mail_Database::getRead(int ID){
    return mail_captions.at(ID).Read;
}

int Mail_Database::getSentRead(int ID){
    return sent_captions.at(ID).Read;
}

std::vector<Mail_Caption> Mail_Database::getMailCaptions(){
    return mail_captions;
}

std::vector<Mail_Caption> Mail_Database::getSentCaptions(){
    return sent_captions;
}

std::wstring Mail_Database::getDate(int ID){
    return mail_captions.at(ID).date;
}

std::wstring Mail_Database::getReceiveDate(int ID){
    return mail_captions.at(ID).receiveDate;
}

std::wstring Mail_Database::getSentDate(int ID){
    return sent_captions.at(ID).date;
}

std::wstring Mail_Database::getSentReceiveDate(int ID){
    return sent_captions.at(ID).receiveDate;
}

void Mail_Database::sendMail(RSA_Encryptor* rsa, int To, std::wstring Caption, std::wstring Message, RSA_Pub_Key key) {
    RSA_Pub_Key signKey;
    mpz_init_set(signKey.e, rsa->getPrivKey().d);
    mpz_init_set(signKey.n, rsa->getPubKey().n);
    std::time_t now = std::time(0);
    std::tm* time = std::localtime(&now);
    std::string datestring = "";
    datestring = std::to_string(time->tm_year + 1900) + '-' + std::to_string(time->tm_mon + 1) + '-' + std::to_string(time->tm_mday) + ' ' + std::to_string(time->tm_hour) + ':' + std::to_string(time->tm_min) + ':' + std::to_string(time->tm_sec);

    if(mysql_query(mysql, std::string("INSERT INTO `Messages`(`From`, `To`, `Caption`, `Message`, `Signature`, `SenderCaption`, `SenderMessage`, `Date`, `SenderDate`) VALUES (\"" + std::to_string(account_id) + "\",\"" + std::to_string(To) + "\", \"" + rsa->encryptString(Caption, key) + "\", \"" + rsa->encryptString(Message, key) + "\", \"" + rsa->encryptString(L"Signature OK", signKey) + "\", \"" + rsa->encryptString(Caption, rsa->getPubKey()) + "\", \"" + rsa->encryptString(Message, rsa->getPubKey())+ "\", \"" + rsa->encryptString(std::wstring(datestring.begin(), datestring.end()), key) + "\", \"" + rsa->encryptString(std::wstring(datestring.begin(), datestring.end()), rsa->getPubKey()) + "\")").c_str())){
        std::cout << mysql_error(mysql) << std::endl;
    }
}

RSA_Pub_Key Mail_Database::userKey(RSA_Encryptor* rsa, int value){
    RSA_Pub_Key result;
    if (mysql_query(mysql, std::string("SELECT `E` FROM `Users` WHERE `ID`=" + std::to_string(value)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::string E;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        E = row[0];
    }

    // Release memories
    mysql_free_result(res);

    if (mysql_query(mysql, std::string("SELECT `N` FROM `Users` WHERE `ID`=" + std::to_string(value)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::string N;
    while ((row = mysql_fetch_row(res)) != NULL){
        N = row[0];
    }

    // Release memories
    mysql_free_result(res);

    mpz_init_set_str(result.e, E.c_str(), 16);
    mpz_init_set_str(result.n, N.c_str(), 16);

    return result;
}

RSA_Pub_Key Mail_Database::userKey(RSA_Encryptor* rsa, std::string value) {
    RSA_Pub_Key result;
    if (mysql_query(mysql, std::string("SELECT `E` FROM `Users` WHERE `Name`=\"" + rsa->encryptString(std::wstring(value.begin(), value.end()), pubNameKey) + "\"").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::string E;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        E = row[0];
    }

    // Release memories
    mysql_free_result(res);

    if (mysql_query(mysql, std::string("SELECT `N` FROM `Users` WHERE `Name`=\"" + rsa->encryptString(std::wstring(value.begin(), value.end()), pubNameKey) + "\"").c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::string N;
    while ((row = mysql_fetch_row(res)) != NULL){
        N = row[0];
    }

    // Release memories
    mysql_free_result(res);

    mpz_init_set_str(result.e, E.c_str(), 16);
    mpz_init_set_str(result.n, N.c_str(), 16);

    return result;
}

void Mail_Database::updateUnderWindows(){
    char cmd[] = "SELECT `BinWin`, LENGTH(`BinWin`) FROM `Versioninfo` WHERE `ID`=1";
    if (mysql_real_query(mysql, cmd, strlen(cmd))) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_store_result(mysql);
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    //save file

    std::ofstream os;
    os.open ("RSA_Client_New.exe", std::ios::binary);
    os.seekp(0, std::ios::beg);

    os.write(row[0], atoi(row[1]));

    os.close();

    // Release memories
    mysql_free_result(res);

    //Rename own Binary
    rename("RSA_Client.exe", "RSA_Client_Old.exe"); //old file will be deleted on next start
    rename("RSA_Client_New.exe", "RSA_Client.exe");
}

void Mail_Database::updateUnderLinux(){
     char cmd[] = "SELECT `BinLinux`, LENGTH(`BinLinux`) FROM `Versioninfo` WHERE `ID`=1";
    if (mysql_real_query(mysql, cmd, strlen(cmd))) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_store_result(mysql);
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    //save file

    std::ofstream os;
    os.open ("RSA_Client_New", std::ios::binary);
    os.seekp(0, std::ios::beg);

    os.write(row[0], atoi(row[1]));

    os.close();

    // Release memories
    mysql_free_result(res);

    //Rename own Binary
    rename("RSA_Client", "RSA_Client_Old"); //old file will be deleted on next start
    rename("RSA_Client_New", "RSA_Client");
    //Make executable
    system("chmod +x RSA_Client");
}

std::wstring Mail_Database::getCaption(int ID){
    return mail_captions.at(ID).caption;
}

bool Mail_Database::passwordSecure(std::wstring passwd){
    bool result = true;
    //Check Security
    if (passwd.length() < 8){
        result = false;
    }
    bool hasLower = false; bool hasUpper = false; bool hasNumber = false;
    for (int i = 0; i < passwd.length(); i++) {
        if (islower(passwd.at(i))){
            hasLower = true;
        }
        if (isupper(passwd.at(i))){
            hasUpper = true;
        }
        if (isdigit(passwd.at(i))){
            hasNumber = true;
        }
    }
    if(!(hasLower && hasUpper && hasNumber)){
        result = false;
    }

    return result;
}

void Mail_Database::exportData(std::string filename){
    std::ofstream of(filename, std::ios::out);

    if (mysql_query(mysql, std::string("SELECT * FROM `Users` WHERE `ID`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        of << "ID" << std::endl << row[0] << std::endl << "Name" << std::endl << row[1] << std::endl << "E" << std::endl << row[2] << std::endl;
        of << "N" << std::endl << row[3] << std::endl << "Password" << std::endl << row[4] << std::endl<< "Email" << std::endl << row[5] << std::endl;
    }

    // Release memories
    mysql_free_result(res);

    if (mysql_query(mysql, std::string("SELECT * FROM `Messages` WHERE `From`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    while ((row = mysql_fetch_row(res)) != NULL){
        of << "ID" << std::endl << row[0] << std::endl << "From" << std::endl << row[1] << std::endl << "To" << std::endl << row[2] << std::endl;
        of << "Caption" << std::endl << row[3] << std::endl << "Message" << std::endl << row[4] << std::endl << "Signature" << std::endl << row[5] << std::endl;
        of << "Read" << std::endl << row[6] << std::endl << "SenderCaption" << std::endl << row[7] << std::endl << "SenderMessage" << std::endl << row[8] << std::endl;
        of << "Date" << std::endl << row[9] << std::endl << "ReceiveDate" << std::endl << row[10] << std::endl << "SenderDate" << std::endl << row[11] << std::endl;
        of << "SenderReceiveDate" << std::endl << row[12] << std::endl;
    }

    // Release memories
    mysql_free_result(res);


    of.close();
}

bool Mail_Database::hasEmail(){
    if (mysql_query(mysql, std::string("SELECT `Email` FROM `Users` WHERE `ID`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::string E;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        E = row[0];
    }

    // Release memories
    mysql_free_result(res);

    if (E != "None"){
        return true;
    } else {
        return false;
    }
}

int Mail_Database::sendPasswordResetEmail(RSA_Encryptor* rsa){
    int randomCode;
    randomCode = rand() % 9999999 + 1000000;
    //Get Email Adress
    if (mysql_query(mysql, std::string("SELECT `Email` FROM `Users` WHERE `ID`=" + std::to_string(account_id)).c_str())) {
        std::cout << mysql_error(mysql) << std::endl;
    }
    res = mysql_use_result(mysql);
    std::wstring Adress;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL){
        Adress = rsa->decryptString(row[0], rsa->getPubKey(), rsa->getPrivKey());
    }

    // Release memories
    mysql_free_result(res);
    //Send Email
    quickmail_initialize();
    quickmail mailobj = quickmail_create(verify_adress.c_str(), "RSA Mail - Passwort Reset");
	
    quickmail_set_body(mailobj, std::string("Hallo " + getUsername() + ", \n\nhier ist der Code zum Resetten deines Passworts: " + std::to_string(randomCode) + ".\n\n\nRSA Mail Verifikation").c_str());
	quickmail_add_to(mailobj, std::string(Adress.begin(), Adress.end()).c_str());
	const char *errmsg = quickmail_send_secure(mailobj, "smtp.gmail.com", 465, verify_adress.c_str(), verify_password.c_str());
    if (errmsg != NULL) {
       std::cout << "Fehler beim Sender der Email: " << errmsg << "\r\n";
    }

    quickmail_destroy(mailobj);

    return randomCode;
}

int Mail_Database::sendVerificationEmail(RSA_Encryptor* rsa, std::string emailAdress){
    int randomCode;
    randomCode = rand() % 9999999 + 1000000;
    //Send Email
    quickmail_initialize();
    quickmail mailobj = quickmail_create(verify_adress.c_str(), "RSA Mail - Email Verifikation");
	
    quickmail_set_body(mailobj, std::string("Hallo " + getUsername() + ", \n\nhier ist der Code zum Verifizieren deiner Email-Adresse: " + std::to_string(randomCode) + ".\n\nFalls Sie sich nicht registriert haben, können Sie diese Email ignorieren.\n\n\nRSA Mail Verifikation").c_str());
	quickmail_add_to(mailobj, emailAdress.c_str());
	const char *errmsg = quickmail_send_secure(mailobj, "smtp.gmail.com", 465, verify_adress.c_str(), verify_password.c_str());
    if (errmsg != NULL) {
       std::cout << "Fehler beim Sender der Email: " << errmsg << "\r\n";
    }

    quickmail_destroy(mailobj);

    return randomCode;
    
}
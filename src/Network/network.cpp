#include "network.h"

Client::Client(){
    mpz_init_set_str(pubNameKey.e, namekey_e.c_str(), 16);
    mpz_init_set_str(pubNameKey.n, namekey_n.c_str(), 16);
    mpz_init_set_str(privNameKey.d, namekey_d.c_str(), 16);
    mpz_init(privNameKey.p);
    mpz_init(privNameKey.q);
    loggedIn = false;
}

Package Client::SendPackage(Package* package){
    Package result(CMD_NONE);
    boost::asio::io_service io_service;
    //socket creation
    boost::asio::ip::tcp::socket socket(io_service);

    boost::system::error_code error;
    //connection
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(serverAddress, "40000");
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
    socket.connect(iter->endpoint(),error);
    if(!error){
        //write package
        std::vector<byte_t> outputBuf = package->GetByteStreamWithLength();
        boost::asio::write(socket, boost::asio::buffer(outputBuf, outputBuf.size()), error);
        if(error) {
            std::cout << "send failed: " << error.message() << std::endl;
        }
        //get length
        uint32_t count = 0;
        //Länge lesen
        socket.read_some(boost::asio::buffer(reinterpret_cast<char*>(&count), 4), error);
        //getting response from server
        byte_t* readdata = (byte_t*)malloc(count);
        boost::asio::read(socket, boost::asio::buffer(readdata, count), error);
        if(error) {
            std::cout << "receive failed: " << error.message() << "! Skipping ..." << std::endl;
        }
        else {
            result.SetByteStream(readdata, count);
        }
    } else {
        std::cout << "connection failed: " << error.message() << std::endl;
    }

    return result;
}

bool Client::ServerReachable(){
    Package request(CMD_GET_VERSION);
    request.Append(-1);
    Package response = SendPackage(&request);
    if(response.GetCmdID() == CMD_NONE){
        return false;
    } else {
        //ServerKey empfangen
        Package request(CMD_GET_SERVER_KEY);
        request.Append(-1);
        Package response = SendPackage(&request);
        if(response.GetCmdID() == CMD_GET_SERVER_KEY){
            std::string key_e = response.GetStrValue();
            mpz_init_set_str(serverKey.e, key_e.c_str(), 16);
            std::string key_n = response.GetStrValue();
            mpz_init_set_str(serverKey.n, key_n.c_str(), 16);
        }

        return true;
    }
}

std::string Client::GetUsername(RSA_Encryptor* rsa, int ID){
    Package request(CMD_GET_USER_DATA);
    request.Append(userID);//UserID
    request.Append(ID);
    Package response = SendPackage(&request);
    response.GetCmdID();
    std::wstring result = rsa->decryptString(response.GetStrValue(), pubNameKey, privNameKey);
    return std::string(result.begin(), result.end());
}

int Client::GetUserID(RSA_Encryptor* rsa, std::string userName){
    Package request(CMD_GET_USER_ID);
    request.Append(rsa->encryptString(std::wstring(userName.begin(), userName.end()), pubNameKey));
    Package response = SendPackage(&request);
    response.GetCmdID();
    return response.GetIntValue();
}

std::string Client::GetUserE(RSA_Encryptor* rsa, int ID){
    Package request(CMD_GET_USER_DATA);
    request.Append(userID);//UserID
    request.Append(ID);
    Package response = SendPackage(&request);
    response.GetCmdID();
    response.GetStrValue();
    response.GetStrValue();
    return response.GetStrValue();
}

std::string Client::GetUserN(RSA_Encryptor* rsa, int ID){
    Package request(CMD_GET_USER_DATA);
    request.Append(userID);
    request.Append(ID);
    Package response = SendPackage(&request);
    response.GetCmdID();
    response.GetStrValue();
    response.GetStrValue();
    response.GetStrValue();
    return response.GetStrValue();
}

bool Client::PasswordValid(RSA_Encryptor* rsa){
    //Get Password
    Package request(CMD_GET_USER_DATA);
    request.Append(userID);//UserID
    request.Append(userID);//UserID: Get Own Data
    Package response = SendPackage(&request);
    response.GetCmdID();
    response.GetStrValue();
    std::string passwd = response.GetStrValue();

    if(rsa->decryptString(passwd, rsa->getPubKey(), rsa->getPrivKey()) == password){
        return true;
    } else {
        return false;
    }
    
}

int Client::GetVersion(){
    Package request(CMD_GET_VERSION);
    Package response = SendPackage(&request);
    response.GetCmdID();
    return response.GetIntValue();
}

void Client::CreateUser(RSA_Encryptor* rsa){
    Package request(CMD_CREATE_USER);
    request.Append(rsa->encryptString(std::wstring(userName.begin(), userName.end()), pubNameKey));
    request.Append(rsa->getE());
    request.Append(rsa->getN());
    request.Append(rsa->encryptString(password, rsa->getPubKey()));
    SendPackage(&request);
}

std::string Client::GetDateString(){
    //time
    std::time_t now = std::time(0);
    std::tm* time = std::localtime(&now);
    std::string datestring = "";
    datestring = std::to_string(time->tm_year + 1900) + '-' + std::to_string(time->tm_mon + 1) + '-' + std::to_string(time->tm_mday) + ' ' + std::to_string(time->tm_hour) + ':' + std::to_string(time->tm_min) + ':' + std::to_string(time->tm_sec);
    return datestring;
}

void Client::SendMessage(RSA_Encryptor* rsa, int recvID, std::wstring caption, std::wstring message){
    //keys
    RSA_Pub_Key recvKey;
    mpz_init_set_str(recvKey.e, GetUserE(rsa, recvID).c_str(), 16);
    mpz_init_set_str(recvKey.n, GetUserN(rsa, recvID).c_str(), 16);
    //time
    std::string datestring = GetDateString();
    
    //send
    Package request(CMD_SEND_MESSAGE);
    request.Append(userID);
    std::string signature, signatureNum;
    std::tie(signature, signatureNum) = GetOwnSignature(rsa);
    request.Append(signature); //Signieren
    request.Append(signatureNum); //Signieren
    request.Append(recvID);
    request.Append(rsa->encryptString(caption, rsa->getPubKey()));
    request.Append(rsa->encryptString(message, rsa->getPubKey()));
    request.Append(rsa->encryptString(caption, recvKey));
    request.Append(rsa->encryptString(message, recvKey));
    RSA_Pub_Key signKey;
    mpz_init_set(signKey.e, rsa->getPrivKey().d);
    mpz_init_set(signKey.n, rsa->getPubKey().n);
    request.Append(rsa->encryptString(L"Signature OK", signKey)); //Signieren
    request.Append(rsa->encryptString(std::wstring(datestring.begin(), datestring.end()), rsa->getPubKey()));
    request.Append(rsa->encryptString(std::wstring(datestring.begin(), datestring.end()), recvKey));
    SendPackage(&request);
}

void Client::DeleteMessage(RSA_Encryptor* rsa, int messageID){
    Package request(CMD_DELETE_MESSAGE);
    request.Append(userID);
    std::string signature, signatureNum;
    std::tie(signature, signatureNum) = GetOwnSignature(rsa);
    request.Append(signature); //Signieren
    request.Append(signatureNum); //Signieren
    request.Append(messageID);
    SendPackage(&request);
}

std::tuple<std::string, std::string> Client::GetOwnSignature(RSA_Encryptor* rsa){
    RSA_Pub_Key signKey;
    mpz_init_set(signKey.e, rsa->getPrivKey().d);
    mpz_init_set(signKey.n, rsa->getPubKey().n);
    int signNum = rand() % 999999999 + 1000000;
    return std::make_tuple(rsa->encryptString(std::to_wstring(signNum), signKey), rsa->encryptString(std::to_wstring(signNum), serverKey));
}

bool Client::ChangeUserPassword(RSA_Encryptor* rsa, std::wstring value){
    Package request(CMD_CHANGE_PASSWORD);
    request.Append(userID);
    std::string signature, signatureNum;
    std::tie(signature, signatureNum) = GetOwnSignature(rsa);
    request.Append(signature); //Signieren
    request.Append(signatureNum); //Signieren
    request.Append(rsa->encryptString(value, rsa->getPubKey()));
    Package response = SendPackage(&request);
    if(response.GetCmdID() != CMD_CHANGE_PASSWORD){
        return false;
    } else {
        return true;
    }
}

int Client::GetCountToUser(RSA_Encryptor* rsa){
    Package request(CMD_GET_COUNT_TO_USER);
    request.Append(userID);
    std::string signature, signatureNum;
    std::tie(signature, signatureNum) = GetOwnSignature(rsa);
    request.Append(signature); //Signieren
    request.Append(signatureNum); //Signieren
    Package response = SendPackage(&request);
    response.GetCmdID();
    return response.GetIntValue();
}

int Client::GetCountFromUser(RSA_Encryptor* rsa){
    Package request(CMD_GET_COUNT_FROM_USER);
    request.Append(userID);
    std::string signature, signatureNum;
    std::tie(signature, signatureNum) = GetOwnSignature(rsa);
    request.Append(signature); //Signieren
    request.Append(signatureNum); //Signieren
    Package response = SendPackage(&request);
    response.GetCmdID();
    return response.GetIntValue();
}

std::wstring Client::GetMessage(RSA_Encryptor* rsa, int ID){
    Package request(CMD_GET_MESSAGE);
    request.Append(userID);
    std::string signature, signatureNum;
    std::tie(signature, signatureNum) = GetOwnSignature(rsa);
    request.Append(signature); //Signieren
    request.Append(signatureNum); //Signieren
    request.Append(ID);
    Package response = SendPackage(&request);
    response.GetCmdID();
    return rsa->decryptString(response.GetStrValue(), rsa->getPubKey(), rsa->getPrivKey());
}

void Client::SetRead(Database* database, RSA_Encryptor* rsa, int ID){
    database->RecvCaptions.at(ID).read=1;
    //key
    RSA_Pub_Key recvKey;
    mpz_init_set_str(recvKey.e, GetUserE(rsa, database->RecvCaptions.at(ID).Person).c_str(), 16);
    mpz_init_set_str(recvKey.n, GetUserN(rsa, database->RecvCaptions.at(ID).Person).c_str(), 16);
    //time
    std::string datestring = GetDateString();
    Package request(CMD_MESSAGE_READ);
    request.Append(userID);
    std::string signature, signatureNum;
    std::tie(signature, signatureNum) = GetOwnSignature(rsa);
    request.Append(signature); //Signieren
    request.Append(signatureNum); //Signieren
    request.Append(database->RecvCaptions.at(ID).ID);
    request.Append(rsa->encryptString(std::wstring(datestring.begin(), datestring.end()), rsa->getPubKey()));
    request.Append(rsa->encryptString(std::wstring(datestring.begin(), datestring.end()), recvKey));
    SendPackage(&request);
}  

int Client::GetRead(RSA_Encryptor* rsa, int messageID){
    Package request(CMD_MESSAGE_GET_READ);
    request.Append(userID);
    std::string signature, signatureNum;
    std::tie(signature, signatureNum) = GetOwnSignature(rsa);
    request.Append(signature); //Signieren
    request.Append(signatureNum); //Signieren
    request.Append(messageID);
    Package response = SendPackage(&request);
    response.GetCmdID();
    return response.GetIntValue();
}

std::vector<Caption> Client::GetCaptionsToUser(RSA_Encryptor* rsa){
    Package request(CMD_GET_MESSAGE_CAPTIONS_TO_USER);
    request.Append(userID);
    std::string signature, signatureNum;
    std::tie(signature, signatureNum) = GetOwnSignature(rsa);
    request.Append(signature); //Signieren
    request.Append(signatureNum); //Signieren
    Package response = SendPackage(&request);
    response.GetCmdID();
    std::vector<Caption> result;
    int size = response.GetIntValue();
    for(int i = 0; i < size; i++){
        Caption cap;
        cap.ID = response.GetIntValue();
        cap.Person = response.GetIntValue();
        cap.Caption = rsa->decryptString(response.GetStrValue(), rsa->getPubKey(), rsa->getPrivKey());
        cap.read = response.GetIntValue();
        cap.Date = rsa->decryptString(response.GetStrValue(), rsa->getPubKey(), rsa->getPrivKey());
        cap.ReceiveDate = rsa->decryptString(response.GetStrValue(), rsa->getPubKey(), rsa->getPrivKey());
        result.push_back(cap);
    }
    return result;
}

std::vector<Caption> Client::GetCaptionsFromUser(RSA_Encryptor* rsa){
    Package request(CMD_GET_MESSAGE_CAPTIONS_FROM_USER);
    request.Append(userID);
    std::string signature, signatureNum;
    std::tie(signature, signatureNum) = GetOwnSignature(rsa);
    request.Append(signature); //Signieren
    request.Append(signatureNum); //Signieren
    Package response = SendPackage(&request);
    response.GetCmdID();
    std::vector<Caption> result;
    int size = response.GetIntValue();
    for(int i = 0; i < size; i++){
        Caption cap;
        cap.ID = response.GetIntValue();
        cap.Person = response.GetIntValue();
        cap.Caption = rsa->decryptString(response.GetStrValue(), rsa->getPubKey(), rsa->getPrivKey());
        cap.read = response.GetIntValue();
        cap.Date = rsa->decryptString(response.GetStrValue(), rsa->getPubKey(), rsa->getPrivKey());
        cap.ReceiveDate = rsa->decryptString(response.GetStrValue(), rsa->getPubKey(), rsa->getPrivKey());
        result.push_back(cap);
    }
    return result;
}

bool Client::passwordSecure(std::wstring passwd){
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
#pragma once
#include <string>
#include <vector>

struct Caption{
    int ID;
    int Person;
    std::wstring Caption;
    int read;
    std::wstring Date;
    std::wstring ReceiveDate;
};

class Database{
    public:
        std::vector<Caption> RecvCaptions;
        std::vector<std::wstring> RecvMessages;
        std::vector<Caption> SentCaptions;
        std::vector<std::wstring> SentMessages;

    private:
};
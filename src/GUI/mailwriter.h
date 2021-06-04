#pragma once
#include "../Database/database.h"
#include "../Encryptor/rsa_encryption.h"
#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include "../define.h"
#include "../Icon.xpm.h"

class MailWriterFrame : public wxFrame {
public:
    MailWriterFrame(Mail_Database* database, RSA_Encryptor* rsa);
private:
    Mail_Database* database;
    RSA_Encryptor* rsa;

    wxTextCtrl* toEdit;
    wxTextCtrl* captionEdit;
    wxRichTextCtrl* mailEdit;
    void OnSend(wxCommandEvent& event);
};
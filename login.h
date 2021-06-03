#pragma once
#include <wx/wx.h>
#include "database.h"
#include "rsa_encryption.h"
#include "define.h"

wxDECLARE_EVENT(EVT_COMMAND_CLEARMAILVIEW, wxCommandEvent);
wxDECLARE_EVENT(EVT_COMMAND_LOGGEDIN, wxCommandEvent);
wxDECLARE_EVENT(EVT_COMMAND_PERFORMUPDATE, wxCommandEvent);

class LoginFrame : public wxFrame {
public:
    LoginFrame(Mail_Database* database, RSA_Encryptor* rsa, wxEvtHandler* mainFrame);
private:
    Mail_Database* database;
    RSA_Encryptor* rsa;
    wxEvtHandler* mainFrameHandler;

    wxTextCtrl* ipEdit;
    wxTextCtrl* usernameEdit;
    wxTextCtrl* passwordEdit;
    void OnLogin(wxCommandEvent& event);
    void OnPasswordReset(wxCommandEvent& event);
};
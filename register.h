#pragma once
#include <wx/wx.h>
#include <wx/thread.h>
#include "define.h"
#include "database.h"
#include "rsa_encryption.h"

wxDECLARE_EVENT(EVT_COMMAND_PERFORMUPDATE, wxCommandEvent);
wxDECLARE_EVENT(EVT_COMMAND_CLEARMAILVIEW, wxCommandEvent);
wxDECLARE_EVENT(EVT_COMMAND_REGISTER, wxCommandEvent);

class RegisterFrame : public wxFrame {
public:
    RegisterFrame(Mail_Database* database, RSA_Encryptor* rsa, wxEvtHandler* mainFrame);
private:
    Mail_Database* database;
    RSA_Encryptor* rsa;
    wxEvtHandler* mainFrameHandler;

    wxTextCtrl* ipEdit;
    wxTextCtrl* usernameEdit;
    wxTextCtrl* emailEdit;
    wxTextCtrl* passwordEdit;
    wxTextCtrl* passwordRepeatEdit;
    wxDialog* tmpDlg;
    void OnRegister(wxCommandEvent& event);
};
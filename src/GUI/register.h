#pragma once
#include <wx/wx.h>
#include <wx/thread.h>
#include "../define.h"
#include "../Database/database.h"
#include "../Encryptor/rsa_encryption.h"
#include "../Icon.xpm.h"

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
    void OnClose(wxCloseEvent& event);
};
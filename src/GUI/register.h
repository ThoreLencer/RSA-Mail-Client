// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include <wx/wx.h>
#include <wx/thread.h>
#include "../define.h"
#include "../Encryptor/rsa_encryption.h"
#include "../Network/network.h"
#include "../Icon.xpm.h"

wxDECLARE_EVENT(EVT_COMMAND_PERFORMUPDATE, wxCommandEvent);
wxDECLARE_EVENT(EVT_COMMAND_CLEARMAILVIEW, wxCommandEvent);
wxDECLARE_EVENT(EVT_COMMAND_REGISTER, wxCommandEvent);

class RegisterFrame : public wxFrame {
public:
    RegisterFrame(RSA_Encryptor* rsa, Client* client, wxEvtHandler* mainFrame);
private:
    RSA_Encryptor* rsa;
    Client* client;
    wxEvtHandler* mainFrameHandler;

    wxTextCtrl* ipEdit;
    wxTextCtrl* usernameEdit;
    wxTextCtrl* passwordEdit;
    wxTextCtrl* passwordRepeatEdit;
    wxDialog* tmpDlg;
    void OnRegister(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
};
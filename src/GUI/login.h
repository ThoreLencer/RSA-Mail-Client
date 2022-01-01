// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include <wx/wx.h>
#include "../Encryptor/rsa_encryption.h"
#include "../define.h"
#include "../Icon.xpm.h"
#include "../Network/network.h"
#include <wx/busyinfo.h>
#include "popupDialog.h"
#include <thread>

class LoginThread : public wxThread
{
public:
    LoginThread(RSA_Encryptor* rsa, Client* client, wxEvtHandler *frame, std::string ip){
        this->evtHandler = frame;
        this->ip = ip;
        this->rsa = rsa;
        this->client = client;
    }

    virtual ExitCode Entry();

private:
    wxEvtHandler *evtHandler;
    RSA_Encryptor* rsa;
    Client* client;
    std::string ip;
};

class LoginFrame : public wxFrame {
public:
    LoginFrame(RSA_Encryptor* rsa, Client* client, wxEvtHandler* mainFrame);
private:
    Client* client;
    RSA_Encryptor* rsa;
    wxEvtHandler* mainFrameHandler;
    wxProgressDialog* progress;
    PopupDialog* loginPopup;

    wxTextCtrl* ipEdit;
    wxTextCtrl* usernameEdit;
    wxTextCtrl* passwordEdit;
    void OnLogin(wxCommandEvent& event);
    void OnLoginFinished(wxCommandEvent& event);
    void OnPasswordReset(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
};
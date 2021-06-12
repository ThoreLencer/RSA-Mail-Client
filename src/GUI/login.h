// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include <wx/wx.h>
#include "../Database/database.h"
#include "../Encryptor/rsa_encryption.h"
#include "../define.h"
#include "../Icon.xpm.h"
#include <wx/busyinfo.h>
#include "popupDialog.h"
#include <thread>

class LoginThread : public wxThread
{
public:
    LoginThread(RSA_Encryptor* rsa, wxEvtHandler *frame, Mail_Database* database, std::string ip){
        this->evtHandler = frame;
        this->database = database;
        this->ip = ip;
        this->rsa = rsa;
    }

    virtual ExitCode Entry();

private:
    wxEvtHandler *evtHandler;
    Mail_Database* database;
    RSA_Encryptor* rsa;
    std::string ip;
};

class LoginFrame : public wxFrame {
public:
    LoginFrame(Mail_Database* database, RSA_Encryptor* rsa, wxEvtHandler* mainFrame);
private:
    Mail_Database* database;
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
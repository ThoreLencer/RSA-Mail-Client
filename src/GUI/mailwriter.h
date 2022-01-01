// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include "../Encryptor/rsa_encryption.h"
#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/filedlg.h>
#include "../define.h"
#include "../Icon.xpm.h"
#include "popupDialog.h"
#include "../Network/network.h"

class MailWriterFrame : public wxFrame {
public:
    MailWriterFrame(RSA_Encryptor* rsa, Client* client, std::wstring caption, std::string user);
private:
    RSA_Encryptor* rsa;
    Client* client;
    wxTextCtrl* toEdit;
    wxTextCtrl* captionEdit;
    wxRichTextCtrl* mailEdit;
    wxListBox* attachmentListBox;
    std::vector<Attachment> attachmentList;
    void OnSend(wxCommandEvent& event);
    void OnAttach(wxCommandEvent& event);
    void OnDetach(wxCommandEvent& event);
};
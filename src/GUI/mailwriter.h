// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include "../Database/database.h"
#include "../Encryptor/rsa_encryption.h"
#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/filedlg.h>
#include "../define.h"
#include "../Icon.xpm.h"
#include "popupDialog.h"

class MailWriterFrame : public wxFrame {
public:
    MailWriterFrame(Mail_Database* database, RSA_Encryptor* rsa);
private:
    Mail_Database* database;
    RSA_Encryptor* rsa;

    wxTextCtrl* toEdit;
    wxTextCtrl* captionEdit;
    wxRichTextCtrl* mailEdit;
    wxListBox* attachmentListBox;
    std::vector<Attachment> attachmentList;
    void OnSend(wxCommandEvent& event);
    void OnAttach(wxCommandEvent& event);
    void OnDetach(wxCommandEvent& event);
};
// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include "mailwriter.h"

MailWriterFrame::MailWriterFrame(Mail_Database* database, RSA_Encryptor* rsa): wxFrame(NULL, wxID_ANY, "Nachricht schreiben"){
    this->database = database;
    this->rsa = rsa;
    this->SetIcon(icon_xmp);
    
    wxFont myFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    //Panel for TAB Order working
    wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* toText = new wxStaticText(panel, wxID_ANY, L"An:");
    toEdit = new wxTextCtrl(panel, ID_Edit_To, "");
    toEdit->SetFont(myFont);
    mainSizer->Add(toText, 0, wxEXPAND);
    mainSizer->Add(toEdit, 0, wxEXPAND);

    wxStaticText* captionText = new wxStaticText(panel, wxID_ANY, L"Betreff:");
    captionEdit = new wxTextCtrl(panel, ID_Edit_Caption, "");
    captionEdit->SetFont(myFont);
    mainSizer->Add(captionText, 0, wxEXPAND);
    mainSizer->Add(captionEdit, 0, wxEXPAND);

    wxStaticText* mailboxText = new wxStaticText(panel, wxID_ANY, L"Nachricht:");
    mainSizer->Add(mailboxText, 0, wxEXPAND);

    mailEdit = new wxRichTextCtrl(panel, ID_Edit_Mail, "");
    mailEdit->SetFont(myFont);
    mainSizer->Add(mailEdit, 2, wxEXPAND);

    wxBoxSizer* hBox1 = new wxBoxSizer(wxHORIZONTAL);
    wxButton* attachFileButton = new wxButton(panel, ID_Button_AttachFile, L"Anhang hinzufügen");
    hBox1->Add(attachFileButton);

    wxButton* detachFileButton = new wxButton(panel, ID_Button_DetachFile, L"Anhang entfernen");
    hBox1->Add(detachFileButton);
    mainSizer->Add(hBox1, 0, wxCENTER);

    attachmentListBox = new wxListBox(panel, ID_ListBox_Attachment);
    mainSizer->Add(attachmentListBox, 1, wxEXPAND);


    wxButton* sendButton = new wxButton(panel, ID_Button_Send, L"Senden");
    mainSizer->Add(sendButton, 0, wxCENTER);

    Bind(wxEVT_BUTTON, &MailWriterFrame::OnSend, this, ID_Button_Send);
    Bind(wxEVT_BUTTON, &MailWriterFrame::OnAttach, this, ID_Button_AttachFile);
    Bind(wxEVT_BUTTON, &MailWriterFrame::OnDetach, this, ID_Button_DetachFile);

    panel->SetSizer(mainSizer);
}

void MailWriterFrame::OnSend(wxCommandEvent& event) {
    // Check if Boxes are filled
    if (captionEdit->GetValue() != "" && toEdit->GetValue() != ""){
        PopupDialog* serverPopup = new PopupDialog(L"Nachricht Senden", L"Sende Nachricht... Bitte warten.", this);
        wxYield(); 
        //Check if Receiver exists
        if (std::wstring(mailEdit->GetValue().c_str()).length() < 50000){
            std::string recv = std::string(toEdit->GetValue().c_str());
            int recvID = database->userExists(rsa, recv);
            if(recvID > -1){
                database->sendMail(rsa, recvID, std::wstring(captionEdit->GetValue().c_str()), std::wstring(mailEdit->GetValue().c_str()), database->userKey(rsa, recv), attachmentList);
                serverPopup->Close();
                wxMessageBox(L"Die Nachricht wurde versandt!", "Nachricht schreiben", wxOK, this);
                this->Close();
            } else {
                serverPopup->Close();
                wxMessageBox(L"Der angegebene Empfänger konnte nicht gefunden werden!", "Fehler", wxOK, this);
            }
        } else {
            serverPopup->Close();
            wxMessageBox(L"Sie haben die maximale Zeichenanzahl von 50000 überschritten. Bitte halten Sie sich etwas kürzer.", "Fehler", wxOK, this);
        }

    } else {
        if (captionEdit->GetValue() == ""){
            wxMessageBox(L"Bitte gib einen Betreff ein!", "Fehler", wxOK, this);
        } 
        if (toEdit->GetValue() == ""){
            wxMessageBox(L"Bitte gib einen Empfänger ein!", "Fehler", wxOK, this);
        }
    }
}

void MailWriterFrame::OnAttach(wxCommandEvent& event){
    wxFileDialog openFileDialog(this, _("Wähle Datei aus"), "", "", "All files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;     

    Attachment attachment;
    attachment.filepath = openFileDialog.GetPath();
    attachment.filename = openFileDialog.GetFilename();

    attachmentList.push_back(attachment);
    attachmentListBox->Append(attachment.filename);
}

void MailWriterFrame::OnDetach(wxCommandEvent& event){
    if (attachmentListBox->GetSelection() > -1){
        attachmentList.erase(attachmentList.begin() + attachmentListBox->GetSelection());
        attachmentListBox->Delete(attachmentListBox->GetSelection());
    } else {
        wxMessageBox(L"Sie haben keinen Anhang ausgewählt.", "Anhang entfernen", wxOK);
    }
}
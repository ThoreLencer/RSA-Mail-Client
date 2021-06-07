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
    mainSizer->Add(mailEdit, 1, wxEXPAND);

    wxButton* sendButton = new wxButton(panel, ID_Button_Send, L"Senden");
    mainSizer->Add(sendButton, 0, wxCENTER);

    Bind(wxEVT_BUTTON, &MailWriterFrame::OnSend, this, ID_Button_Send);

    panel->SetSizer(mainSizer);
}

void MailWriterFrame::OnSend(wxCommandEvent& event) {
    // Check if Boxes are filled
    if (captionEdit->GetValue() != "" && toEdit->GetValue() != ""){
        //Check if Receiver exists
        if (std::wstring(mailEdit->GetValue().c_str()).length() < 50000){
            std::string recv = std::string(toEdit->GetValue().c_str());
            int recvID = database->userExists(rsa, recv);
            if(recvID > -1){
                database->sendMail(rsa, recvID, std::wstring(captionEdit->GetValue().c_str()), std::wstring(mailEdit->GetValue().c_str()), database->userKey(rsa, recv));
                wxMessageBox(L"Die Nachricht wurde versandt!", "Nachricht schreiben", wxOK, this);
                this->Close();
            } else {
                wxMessageBox(L"Der angegebene Empfänger konnte nicht gefunden werden!", "Fehler", wxOK, this);
            }
        } else {
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
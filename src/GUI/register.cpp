// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include "register.h"

RegisterFrame::RegisterFrame(RSA_Encryptor* rsa, Client* client, wxEvtHandler* mainFrame): wxFrame(NULL, wxID_ANY, "Registrierung") {
    this->rsa = rsa;
    this->mainFrameHandler = mainFrame;
    this->client = client;
    this->SetIcon(icon_xmp);
    wxFont myFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    //Description of the GUI
    //Panel for TAB Order working
    wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(5, 2, 5, 5);

    wxStaticText* ipText = new wxStaticText(panel, wxID_ANY, L"IP-Adresse:", wxPoint(10, 15));
    ipText->SetFont(myFont);
    gridSizer->Add(ipText, 1, wxEXPAND);
    ipEdit = new wxTextCtrl(panel, ID_Edit_IP, L"rsamail.ddns.net", wxPoint(100, 10), wxSize(100, 30));
    gridSizer->Add(ipEdit, 1, wxEXPAND);
    wxStaticText* usernameText = new wxStaticText(panel, wxID_ANY, L"Benutzername:", wxPoint(10, 65));
    usernameText->SetFont(myFont);
    gridSizer->Add(usernameText, 1, wxEXPAND);
    usernameEdit = new wxTextCtrl(panel, ID_Edit_Username, L"", wxPoint(100, 60), wxSize(100, 30));
    gridSizer->Add(usernameEdit, 1, wxEXPAND);
    wxStaticText* passwordText = new wxStaticText(panel, wxID_ANY, L"Passwort:", wxPoint(10, 125));
    passwordText->SetFont(myFont);
    gridSizer->Add(passwordText, 1, wxEXPAND);
    passwordEdit = new wxTextCtrl(panel, ID_Edit_Password, L"", wxPoint(100, 110), wxSize(100, 30), wxTE_PASSWORD);
    gridSizer->Add(passwordEdit, 1, wxEXPAND);
    wxStaticText* passwordRepeatText = new wxStaticText(panel, wxID_ANY, L"wiederholen:", wxPoint(10, 125));
    passwordRepeatText->SetFont(myFont);
    gridSizer->Add(passwordRepeatText, 1, wxEXPAND);
    passwordRepeatEdit = new wxTextCtrl(panel, ID_Edit_Password_Repeat, L"", wxPoint(100, 110), wxSize(100, 30), wxTE_PASSWORD);
    gridSizer->Add(passwordRepeatEdit, 1, wxEXPAND);
    gridSizer->AddGrowableCol(1, 1);

    wxButton* registerButton = new wxButton(panel, ID_Button_Register, L"Registrieren", wxPoint(100, 150), wxSize(100, 50));

    mainSizer->Add(gridSizer, 1, wxEXPAND);
    mainSizer->Add(registerButton, 0, wxEXPAND);
    panel->SetSizer(mainSizer);
    Bind(wxEVT_BUTTON, &RegisterFrame::OnRegister, this, ID_Button_Register);
    Bind(wxEVT_CLOSE_WINDOW, &RegisterFrame::OnClose, this);
}

void RegisterFrame::OnRegister(wxCommandEvent& event){
    client->serverAddress = this->ipEdit->GetValue();
    if (client->loggedIn){
        wxThreadEvent event(EVT_COMMAND_CLEARMAILVIEW, ID_EVT_CLEARMAILVIEW);
        wxQueueEvent(mainFrameHandler, event.Clone());
        client->loggedIn = false;
    }

    if(client->ServerReachable()){
    if (this->ipEdit->GetValue() != "" && this->usernameEdit->GetValue() != "" && this->passwordEdit->GetValue() != "" && this->passwordRepeatEdit->GetValue() != ""){
        client->userName = this->usernameEdit->GetValue();
        if (this->passwordEdit->GetValue() == this->passwordRepeatEdit->GetValue()){
            if (client->passwordSecure(std::wstring(this->passwordEdit->GetValue().c_str()))){
                client->password = std::wstring(this->passwordEdit->GetValue());
                //Check Version
                if(client->GetVersion() == CLIENT_VERSION){
                    client->userID = client->GetUserID(rsa, std::string(this->usernameEdit->GetValue()));
                    if(client->userID == -1){
                        //Create New User
                        wxThreadEvent event(EVT_COMMAND_REGISTER, ID_EVT_ONREGISTER);
                        wxQueueEvent(mainFrameHandler, event.Clone());
                    } else {
                        wxMessageBox("Es gibt schon einen Benutzer unter diesem Namen!", "Fehler", wxOK);
                    }
                } else {
                    wxMessageBox(L"Die Version dieses Clients stimmt nicht mit der des Servers überein. Bitte laden Sie die neuste Version herunter.", "Versionsfehler", wxOK);
                    wxThreadEvent event(EVT_COMMAND_PERFORMUPDATE, ID_EVT_PERFORMUPDATE);
                    wxQueueEvent(mainFrameHandler, event.Clone());
                }
            } else {
                wxMessageBox(L"Das gewählte Passwort ist unsicher. Das Passwort muss Groß - und Kleinbuchstaben und eine Zahl enthalten. Außerdem sollte es mindestens 8 Zeichen lang sein.", "Fehler", wxOK, this);
            }
        } else {
            wxMessageBox(L"Die Passwörter stimmen nicht überein!", "Fehler", wxOK, this);
        }
    } else {
        wxMessageBox("Sie haben nicht alle Eingabefelder ausgefüllt!", "Fehler", wxOK, this);
    }
    } else {
        wxMessageBox("Es konnte keine Verbindung zum Server aufgebaut werden!", "Fehler", wxOK, this);
    }
}

void RegisterFrame::OnClose(wxCloseEvent& event){
    wxCommandEvent evt(EVT_COMMAND_MAINFRAME_ENABLE, ID_EVT_MAINFRAME_ENABLE);
    wxQueueEvent(mainFrameHandler, evt.Clone());
    this->Destroy();
}
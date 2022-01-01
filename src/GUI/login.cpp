// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include "login.h"

enum
{
    ID_Version_Invalid,
    ID_User_Nonexist,
    ID_Server_Unreachable,
    ID_Password_Wrong,
    ID_Login_Successful,
    ID_Update_Required
};

wxDEFINE_EVENT(EVT_COMMAND_LOGIN_FINISH, wxCommandEvent);

wxThread::ExitCode LoginThread::Entry()
{
    if (client->ServerReachable())
    {
        if (client->GetVersion() == CLIENT_VERSION)
        {
            int userID = client->GetUserID(rsa, client->userName);
            client->userID = userID;
            if (userID != -1)
            {
                // Load Keys
                rsa->loadPrivKey(std::string(client->userName + ".key"));
                rsa->setE(client->GetUserE(rsa, userID));
                rsa->setN(client->GetUserN(rsa, userID));
                //Check Password
                if (!client->PasswordValid(rsa))
                {
                    wxCommandEvent event(EVT_COMMAND_LOGIN_FINISH, ID_Password_Wrong);
                    wxQueueEvent(evtHandler, event.Clone());
                } else {
                    wxCommandEvent event(EVT_COMMAND_LOGIN_FINISH, ID_Login_Successful);
                    wxQueueEvent(evtHandler, event.Clone());
                }
            }
            else
            {
                wxCommandEvent event(EVT_COMMAND_LOGIN_FINISH, ID_User_Nonexist);
                wxQueueEvent(evtHandler, event.Clone());
            }
        }
        else
        {
            wxCommandEvent event(EVT_COMMAND_LOGIN_FINISH, ID_Update_Required);
            wxQueueEvent(evtHandler, event.Clone());
        }
    }
    else
    {
        wxCommandEvent event(EVT_COMMAND_LOGIN_FINISH, ID_Server_Unreachable);
        wxQueueEvent(evtHandler, event.Clone());
    }
    return static_cast<wxThread::ExitCode>(NULL);
}

void LoginFrame::OnLoginFinished(wxCommandEvent &event)
{
    if (event.GetId() == ID_Login_Successful)
    {
        client->loggedIn = true;
        loginPopup->Close();
        wxMessageBox("Die Anmeldung ist erfolgreich.", "Anmeldung", wxOK);
        wxThreadEvent event(EVT_COMMAND_LOGGEDIN, ID_EVT_LOGGEDIN);
        wxQueueEvent(mainFrameHandler, event.Clone());

        this->Close();
    }
    if (event.GetId() == ID_Password_Wrong)
    {
        loginPopup->Close();
        wxMessageBox("Das eingegebene Passwort ist falsch. Probieren Sie es nocheinmal.", "Anmeldung", wxOK);
    }
    if (event.GetId() == ID_User_Nonexist)
    {
        loginPopup->Close();
        // User does not exist
        wxMessageBox(L"Der angegebene Benutzer konnte nicht gefunden werden. Nutzen Sie die Registrierung um ein Benutzerkonto anzulegen.", "Anmeldung", wxOK);
    }

    if (event.GetId() == ID_Update_Required)
    {
        loginPopup->Close();
        if (wxMessageBox(L"Die Version dieses Clients stimmt nicht mit der des Servers überein. Soll die neueste Version jetzt heruntergeladen werden?", "Versionsfehler", wxYES_NO) == wxYES)
        {
            wxThreadEvent event(EVT_COMMAND_PERFORMUPDATE, ID_EVT_PERFORMUPDATE);
            wxQueueEvent(mainFrameHandler, event.Clone());
        }
    }

    if (event.GetId() == ID_Server_Unreachable)
    {
        loginPopup->Close();
        wxMessageBox("Es konnte keine Verbindung zum Server hergestellt werden!", "Fehler", wxOK);
    }
    Enable();
}

void LoginFrame::OnLogin(wxCommandEvent &event)
{
    client->serverAddress = this->ipEdit->GetValue();
    if (this->ipEdit->GetValue() != "" && this->usernameEdit->GetValue() != "" && this->passwordEdit->GetValue() != "")
    {
        client->userName = this->usernameEdit->GetValue();
        client->password = this->passwordEdit->GetValue();
        loginPopup = new PopupDialog(L"Anmeldung", L"Melde an... Bitte warten.", NULL);

        if (client->loggedIn)
        {
            wxThreadEvent event(EVT_COMMAND_CLEARMAILVIEW, ID_EVT_CLEARMAILVIEW);
            wxQueueEvent(mainFrameHandler, event.Clone());
            client->loggedIn = false;
        }

        LoginThread *thread = new LoginThread(rsa, client, this, std::string(this->ipEdit->GetValue()));
        thread->Create();
        thread->Run();
        Disable();
    }
    else
    {
        if (this->ipEdit->GetValue() == "")
        {
            wxMessageBox("Bitte gib eine IP ein!", "Fehler", wxOK, this);
        }
        if (this->usernameEdit->GetValue() == "")
        {
            wxMessageBox("Bitte gib einen Benutzername ein!", "Fehler", wxOK, this);
        }
        if (this->passwordEdit->GetValue() == "")
        {
            wxMessageBox("Bitte gib ein Passwort ein!", "Fehler", wxOK, this);
        }
    }
}

void LoginFrame::OnPasswordReset(wxCommandEvent &event)
{
    client->serverAddress = this->ipEdit->GetValue();
    PopupDialog *serverPopup = new PopupDialog(L"Passwort zurücksetzen", L"Verbinde zum Server...", this);
    wxYield();
    if (client->ServerReachable())
    {
        if (this->ipEdit->GetValue() != "" && this->usernameEdit->GetValue() != "")
        {

            if (client->loggedIn)
            {
                wxThreadEvent event(EVT_COMMAND_CLEARMAILVIEW, ID_EVT_CLEARMAILVIEW);
                wxQueueEvent(mainFrameHandler, event.Clone());
                client->loggedIn = false;
            }
            client->userName = std::string(this->usernameEdit->GetValue().c_str());
            client->userID = client->GetUserID(rsa, client->userName);

            if (client->GetVersion() == CLIENT_VERSION)
            {
                if (client->userID != -1)
                {
                    serverPopup->Close();
                    // Load Keys
                    rsa->loadPrivKey(std::string(client->userName + ".key"));
                    rsa->setE(client->GetUserE(rsa, client->userID));
                    rsa->setN(client->GetUserN(rsa, client->userID));
                    // Change Password
                    // Ask for new password
                    std::wstring passwd1, passwd2;
                    wxTextEntryDialog dlg1(this, L"Bitte geben Sie ein neues Passwort ein.", L"Passwort ändern", "", wxOK | wxCANCEL | wxTE_PASSWORD);
                    if (dlg1.ShowModal() == wxID_OK)
                    {
                        passwd1 = dlg1.GetValue();
                        wxTextEntryDialog dlg2(this, L"Bitte geben Sie das Passwort zur Bestätigung nochmals ein.", L"Passwort ändern", "", wxOK | wxCANCEL | wxTE_PASSWORD);
                        if (dlg2.ShowModal() == wxID_OK)
                        {
                            passwd2 = dlg2.GetValue();
                            if (passwd1 == passwd2)
                            {
                                if (client->passwordSecure(passwd1))
                                {
                                    // Change Password
                                    if(client->ChangeUserPassword(rsa, passwd1)){
                                        wxMessageBox(L"Das Passwort wurde erfolgreich geändert. Sie könnten sich nun anmelden.", L"Passwort ändern", wxOK);
                                        if (client->loggedIn)
                                        {
                                            wxThreadEvent event(EVT_COMMAND_CLEARMAILVIEW, ID_EVT_CLEARMAILVIEW);
                                            wxQueueEvent(mainFrameHandler, event.Clone());
                                            client->loggedIn = false;
                                        }
                                    } else {
                                        wxMessageBox(L"Das Passwort dieses Benutzers können Sie nicht ändern.", L"Passwort ändern", wxOK);
                                    }
                                }
                                else
                                {
                                    wxMessageBox(L"Das gewählte Passwort ist unsicher. Das Passwort muss Groß - und Kleinbuchstaben und eine Zahl enthalten. Außerdem sollte es mindestens 8 Zeichen lang sein.", "Fehler", wxOK, this);
                                }
                            }
                            else
                            {
                                wxMessageBox(L"Die eingegebenen Passwörter stimmen nicht überein.", L"Passwort ändern", wxOK);
                            }
                        }
                    }

                    this->Close();
                }
                else
                {
                    serverPopup->Close();
                    // User does not exist
                    wxMessageBox(L"Der angegebene Benutzer konnte nicht gefunden werden. Nutzen Sie die Registrierung um ein Benutzerkonto anzulegen.", "Passwort zurücksetzen", wxOK);
                }
            }
            else
            {
                serverPopup->Close();
                if (wxMessageBox(L"Die Version dieses Clients stimmt nicht mit der des Servers überein. Soll die neueste Version jetzt heruntergeladen werden?", "Versionsfehler", wxYES_NO) == wxYES)
                {
                    wxThreadEvent event(EVT_COMMAND_PERFORMUPDATE, ID_EVT_PERFORMUPDATE);
                    wxQueueEvent(mainFrameHandler, event.Clone());
                }
            }
        }
        else
        {
            serverPopup->Close();
            if (this->ipEdit->GetValue() == "")
            {
                wxMessageBox("Bitte gib eine IP ein!", "Passwort zurücksetzen", wxOK, this);
            }
            if (this->usernameEdit->GetValue() == "")
            {
                wxMessageBox("Bitte gib einen Benutzername ein!", "Passwort zurücksetzen", wxOK, this);
            }
        }
    }
    else
    {
        serverPopup->Close();
        wxMessageBox("Es konnte keine Verbindung zum Server hergestellt werden!", "Passwort zurücksetzen", wxOK);
    }
}

void LoginFrame::OnClose(wxCloseEvent &event)
{
    wxCommandEvent evt(EVT_COMMAND_MAINFRAME_ENABLE, ID_EVT_MAINFRAME_ENABLE);
    wxQueueEvent(mainFrameHandler, evt.Clone());
    this->Destroy();
}

LoginFrame::LoginFrame(RSA_Encryptor *rsa, Client *client, wxEvtHandler *mainFrame) : wxFrame(NULL, wxID_ANY, "Anmelden")
{
    this->rsa = rsa;
    this->mainFrameHandler = mainFrame;
    this->client = client;

    this->SetIcon(icon_xmp);
    wxFont myFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    // Description of the GUI
    // Panel for TAB Order working
    wxPanel *panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *gridSizer = new wxFlexGridSizer(3, 2, 5, 5);

    wxStaticText *ipText = new wxStaticText(panel, wxID_ANY, L"IP-Adresse:");
    ipText->SetFont(myFont);
    gridSizer->Add(ipText, 1, wxEXPAND);
    ipEdit = new wxTextCtrl(panel, ID_Edit_IP, L"rsamail.ddns.net");
    gridSizer->Add(ipEdit, 1, wxEXPAND);
    wxStaticText *usernameText = new wxStaticText(panel, wxID_ANY, L"Benutzername:");
    usernameText->SetFont(myFont);
    gridSizer->Add(usernameText, 1, wxEXPAND);
    usernameEdit = new wxTextCtrl(panel, ID_Edit_Username, L"");
    gridSizer->Add(usernameEdit, 1, wxEXPAND);
    wxStaticText *passwordText = new wxStaticText(panel, wxID_ANY, L"Passwort:");
    passwordText->SetFont(myFont);
    gridSizer->Add(passwordText, 1, wxEXPAND);
    passwordEdit = new wxTextCtrl(panel, ID_Edit_Password, L"", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    gridSizer->Add(passwordEdit, 1, wxEXPAND);
    gridSizer->AddGrowableCol(1, 1);
    wxButton *login = new wxButton(panel, ID_Button_Login, L"Anmelden");
    wxButton *passwdReset = new wxButton(panel, ID_Button_PasswdReset, L"Passwort vergessen");

    mainSizer->Add(gridSizer, 1, wxEXPAND);
    mainSizer->Add(login, 0, wxEXPAND);
    mainSizer->Add(passwdReset, 0, wxEXPAND);
    panel->SetSizer(mainSizer);
    Bind(wxEVT_BUTTON, &LoginFrame::OnLogin, this, ID_Button_Login);
    Bind(wxEVT_BUTTON, &LoginFrame::OnPasswordReset, this, ID_Button_PasswdReset);
    Bind(EVT_COMMAND_LOGIN_FINISH, &LoginFrame::OnLoginFinished, this);
    Bind(wxEVT_CLOSE_WINDOW, &LoginFrame::OnClose, this);
}
#include "login.h"

void LoginFrame::OnLogin(wxCommandEvent& event) {  
    if (this->ipEdit->GetValue() != "" && this->usernameEdit->GetValue() != "" && this->passwordEdit->GetValue() != ""){
        if (database->isConnected()){
            wxThreadEvent event(EVT_COMMAND_CLEARMAILVIEW, ID_EVT_CLEARMAILVIEW);
            wxQueueEvent(mainFrameHandler, event.Clone());
            database->close();
        }
        database->setLoginInfo(std::string(this->usernameEdit->GetValue().c_str()), std::wstring(this->passwordEdit->GetValue().c_str()));
        
        if (database->connect(std::string(ipEdit->GetValue().c_str()))){
            if(database->versionValid()){
                if (database->userExists(rsa)){
                    // Load Keys
                    rsa->loadPrivKey(std::string(database->getUsername() + ".key"));
                    rsa->setE(database->getE(rsa));
                    rsa->setN(database->getN(rsa));
                    //Check password
                    if(database->passwordValid(rsa)){
                        wxMessageBox("Die Anmeldung ist erfolgreich.", "Anmeldung", wxOK);
                        wxThreadEvent event(EVT_COMMAND_LOGGEDIN, ID_EVT_LOGGEDIN);
                        wxQueueEvent(mainFrameHandler, event.Clone());

                        this->Close();
                    } else {
                        wxMessageBox("Das eingegebene Passwort ist falsch. Probieren Sie es nocheinmal.", "Anmeldung", wxOK);
                        if (database->isConnected()){
                            database->close();
                        }
                    }
                } else {
                    // User does not exist
                    wxMessageBox(L"Der angegebene Benutzer konnte nicht gefunden werden. Nutzen Sie die Registrierung um ein Benutzerkonto anzulegen.", "Anmeldung", wxOK);
                }
            } else {
                if(wxMessageBox(L"Die Version dieses Clients stimmt nicht mit der des Servers überein. Soll die neueste Version jetzt heruntergeladen werden?", "Versionsfehler", wxYES_NO) == wxYES){
                    wxThreadEvent event(EVT_COMMAND_PERFORMUPDATE, ID_EVT_PERFORMUPDATE);
                    wxQueueEvent(mainFrameHandler, event.Clone());
                }
            }
        } else {
            wxMessageBox("Es konnte keine Verbindung zum Server hergestellt werden!", "Fehler", wxOK);
        }
    } else {
        if (this->ipEdit->GetValue() == ""){
            wxMessageBox("Bitte gib eine IP ein!", "Fehler", wxOK, this);
        } 
        if(this->usernameEdit->GetValue() == ""){
            wxMessageBox("Bitte gib einen Benutzername ein!", "Fehler", wxOK, this);
        } 
        if(this->passwordEdit->GetValue() == ""){
            wxMessageBox("Bitte gib ein Passwort ein!", "Fehler", wxOK, this);
        }
    }
}

void LoginFrame::OnPasswordReset(wxCommandEvent& event){
    if (this->ipEdit->GetValue() != "" && this->usernameEdit->GetValue() != ""){
        if (database->isConnected()){
            wxThreadEvent event(EVT_COMMAND_CLEARMAILVIEW, ID_EVT_CLEARMAILVIEW);
            wxQueueEvent(mainFrameHandler, event.Clone());
            database->close();
        }
        database->setLoginInfo(std::string(this->usernameEdit->GetValue().c_str()), std::wstring(this->passwordEdit->GetValue().c_str()));
        
        if (database->connect(std::string(ipEdit->GetValue().c_str()))){
            if(database->versionValid()){
                if (database->userExists(rsa)){
                    // Load Keys
                    rsa->loadPrivKey(std::string(database->getUsername() + ".key"));
                    rsa->setE(database->getE(rsa));
                    rsa->setN(database->getN(rsa));
                    //Change Password
                    if (database->hasEmail()){
                        //Email Request
                        int code = database->sendPasswordResetEmail(rsa);
                        wxMessageBox(L"Es wurde eine Email zur Bestätigung Ihrer Identität verschickt. Bitte geben Sie den dort enthaltenen Bestätigungscode im folgenden Fenster ein.", "Passwort zurücksetzen", wxOK);
                        //Ask for Code
                        wxTextEntryDialog dlg(this, L"Bitte geben Sie den Bestätigungscode ein.", "Email Verifizierung", "", wxOK | wxCANCEL);
                        if(dlg.ShowModal() == wxID_OK){
                            if(dlg.GetValue() == std::to_string(code)){
                                //Code correct; Ask for new password
                                std::wstring passwd1, passwd2;
                                wxTextEntryDialog dlg1(this, L"Bitte geben Sie ein neues Passwort ein.", L"Passwort ändern", "", wxOK | wxCANCEL | wxTE_PASSWORD);
                                if(dlg1.ShowModal() == wxID_OK){
                                    passwd1 = dlg1.GetValue();
                                    wxTextEntryDialog dlg2(this, L"Bitte geben Sie das Passwort zur Bestätigung nochmals ein.", L"Passwort ändern", "", wxOK | wxCANCEL | wxTE_PASSWORD);
                                    if(dlg2.ShowModal() == wxID_OK){
                                        passwd2 = dlg2.GetValue();
                                        if(passwd1 == passwd2){
                                            if (database->passwordSecure(passwd1)){
                                                //Change Password
                                                database->changePassword(rsa, passwd1);
                                                wxMessageBox(L"Das Passwort wurde erfolgreich geändert. Sie könnten sich nun anmelden.", L"Passwort ändern", wxOK);
                                                database->close();
                                            } else {
                                                wxMessageBox(L"Das gewählte Passwort ist unsicher. Das Passwort muss Groß - und Kleinbuchstaben und eine Zahl enthalten. Außerdem sollte es mindestens 8 Zeichen lang sein.", "Fehler", wxOK, this);
                                            }
                                        } else {
                                            wxMessageBox(L"Die eingegebenen Passwörter stimmen nicht überein.", L"Passwort ändern", wxOK);
                                        }
                                    }
                                }
                            } else {
                                wxMessageBox("Der eingegebene Code ist falsch.", "Email Verifizierung", wxOK);
                            }
                        }
                        this->Close();
                    } else {
                        if(wxMessageBox(L"In Ihrem Account ist keine Email-Adresse hinterlegt. Wollen Sie das Benutzerkonto löschen? Sie können sich anschließend neu registieren.", "Passwort zurücksetzen", wxYES_NO) == wxYES){
                            database->deleteUser();
                            wxCommandEvent event(wxEVT_MENU, ID_Logout);
                            wxQueueEvent(mainFrameHandler, event.Clone());
                            remove(std::string(database->getUsername() + ".key").c_str());
                            wxMessageBox(L"Ihr Account wurde erfolgreich gelöscht.", L"Account löschen", wxOK);
                            this->Close();
                        }
                    }
                } else {
                    // User does not exist
                    wxMessageBox(L"Der angegebene Benutzer konnte nicht gefunden werden. Nutzen Sie die Registrierung um ein Benutzerkonto anzulegen.", "Passwort zurücksetzen", wxOK);
                }
            } else {
                if(wxMessageBox(L"Die Version dieses Clients stimmt nicht mit der des Servers überein. Soll die neueste Version jetzt heruntergeladen werden?", "Versionsfehler", wxYES_NO) == wxYES){
                    wxThreadEvent event(EVT_COMMAND_PERFORMUPDATE, ID_EVT_PERFORMUPDATE);
                    wxQueueEvent(mainFrameHandler, event.Clone());
                }
            }
        } else {
            wxMessageBox("Es konnte keine Verbindung zum Server hergestellt werden!", "Passwort zurücksetzen", wxOK);
        }
    } else {
        if (this->ipEdit->GetValue() == ""){
            wxMessageBox("Bitte gib eine IP ein!", "Passwort zurücksetzen", wxOK, this);
        } 
        if(this->usernameEdit->GetValue() == ""){
            wxMessageBox("Bitte gib einen Benutzername ein!", "Passwort zurücksetzen", wxOK, this);
        } 
    }
}

LoginFrame::LoginFrame(Mail_Database* database, RSA_Encryptor* rsa, wxEvtHandler* mainFrame): wxFrame(NULL, wxID_ANY, "Anmelden") {
    this->database = database;
    this->rsa = rsa;
    this->mainFrameHandler = mainFrame;
   
    this->SetIcon(icon_xmp);
    wxFont myFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    //Description of the GUI
    //Panel for TAB Order working
    wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(3, 2, 5, 5);

    wxStaticText* ipText = new wxStaticText(panel, wxID_ANY, L"IP-Adresse:");
    ipText->SetFont(myFont);
    gridSizer->Add(ipText, 1, wxEXPAND);
    ipEdit = new wxTextCtrl(panel, ID_Edit_IP, L"rsamail.ddns.net");
    gridSizer->Add(ipEdit, 1, wxEXPAND);
    wxStaticText* usernameText = new wxStaticText(panel, wxID_ANY, L"Benutzername:");
    usernameText->SetFont(myFont);
    gridSizer->Add(usernameText, 1, wxEXPAND);
    usernameEdit = new wxTextCtrl(panel, ID_Edit_Username, L"");
    gridSizer->Add(usernameEdit, 1, wxEXPAND);
    wxStaticText* passwordText = new wxStaticText(panel, wxID_ANY, L"Passwort:");
    passwordText->SetFont(myFont);
    gridSizer->Add(passwordText, 1, wxEXPAND);
    passwordEdit = new wxTextCtrl(panel, ID_Edit_Password, L"", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    gridSizer->Add(passwordEdit, 1, wxEXPAND);
    gridSizer->AddGrowableCol(1, 1);
    wxButton* login = new wxButton(panel, ID_Button_Login, L"Anmelden");
    wxButton* passwdReset = new wxButton(panel, ID_Button_PasswdReset, L"Passwort vergessen");

    mainSizer->Add(gridSizer, 1, wxEXPAND);
    mainSizer->Add(login, 0, wxEXPAND);
    mainSizer->Add(passwdReset, 0, wxEXPAND);
    panel->SetSizer(mainSizer);
    Bind(wxEVT_BUTTON, &LoginFrame::OnLogin, this, ID_Button_Login);
    Bind(wxEVT_BUTTON, &LoginFrame::OnPasswordReset, this, ID_Button_PasswdReset);
}
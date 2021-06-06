#include "register.h"

RegisterFrame::RegisterFrame(Mail_Database* database, RSA_Encryptor* rsa, wxEvtHandler* mainFrame): wxFrame(NULL, wxID_ANY, "Registrierung") {
    this->database = database;
    this->rsa = rsa;
    this->mainFrameHandler = mainFrame;
    #if defined(__WXMSW__)
        this->SetIcon(wxICON(progicon));
    #else
        this->SetIcon(icon_xmp);
    #endif
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
    ipEdit->SetFont(myFont);
    gridSizer->Add(ipEdit, 1, wxEXPAND);
    wxStaticText* usernameText = new wxStaticText(panel, wxID_ANY, L"Benutzername:", wxPoint(10, 65));
    usernameText->SetFont(myFont);
    gridSizer->Add(usernameText, 1, wxEXPAND);
    usernameEdit = new wxTextCtrl(panel, ID_Edit_Username, L"", wxPoint(100, 60), wxSize(100, 30));
    usernameEdit->SetFont(myFont);
    gridSizer->Add(usernameEdit, 1, wxEXPAND);
    wxStaticText* emailText = new wxStaticText(panel, wxID_ANY, L"Email:", wxPoint(10, 65));
    emailText->SetFont(myFont);
    gridSizer->Add(emailText, 1, wxEXPAND);
    emailEdit = new wxTextCtrl(panel, ID_Edit_Email, L"", wxPoint(100, 60), wxSize(100, 30));
    emailEdit->SetFont(myFont);
    gridSizer->Add(emailEdit, 1, wxEXPAND);
    wxStaticText* passwordText = new wxStaticText(panel, wxID_ANY, L"Passwort:", wxPoint(10, 125));
    passwordText->SetFont(myFont);
    gridSizer->Add(passwordText, 1, wxEXPAND);
    passwordEdit = new wxTextCtrl(panel, ID_Edit_Password, L"", wxPoint(100, 110), wxSize(100, 30), wxTE_PASSWORD);
    passwordEdit->SetFont(myFont);
    gridSizer->Add(passwordEdit, 1, wxEXPAND);
    wxStaticText* passwordRepeatText = new wxStaticText(panel, wxID_ANY, L"wiederholen:", wxPoint(10, 125));
    passwordRepeatText->SetFont(myFont);
    gridSizer->Add(passwordRepeatText, 1, wxEXPAND);
    passwordRepeatEdit = new wxTextCtrl(panel, ID_Edit_Password_Repeat, L"", wxPoint(100, 110), wxSize(100, 30), wxTE_PASSWORD);
    passwordRepeatEdit->SetFont(myFont);
    gridSizer->Add(passwordRepeatEdit, 1, wxEXPAND);
    gridSizer->AddGrowableCol(1, 1);

    wxButton* registerButton = new wxButton(panel, ID_Button_Register, L"Registrieren", wxPoint(100, 150), wxSize(100, 50));

    mainSizer->Add(gridSizer, 1, wxEXPAND);
    mainSizer->Add(registerButton, 0, wxEXPAND);
    panel->SetSizer(mainSizer);
    Bind(wxEVT_BUTTON, &RegisterFrame::OnRegister, this, ID_Button_Register);
}

void RegisterFrame::OnRegister(wxCommandEvent& event){
    std::wstring email = L"None";
    bool continueProcess = true;
    bool emailValid = true;
    if (this->ipEdit->GetValue() != "" && this->usernameEdit->GetValue() != "" && this->passwordEdit->GetValue() != "" && this->passwordRepeatEdit->GetValue() != ""){
        if(this->emailEdit->GetValue() == ""){
            if(!wxMessageBox(L"Sie haben keine Email-Adresse eingegeben. Diese wird benötigt, falls Sie Ihr Passwort vergessen. Falls Sie keine Email-Adresse angeben möchten, können Sie Ihr Benutzerkonto bei Verlust des Passwortes nur noch löschen. Wollen Sie fortfahren?", "Email-Adresse", wxYES_NO) == wxYES){
                continueProcess = false; 
            } 
        }
        if (continueProcess){
            if(this->emailEdit->GetValue() != ""){
                email = this->emailEdit->GetValue();
            }
            if (database->isConnected()){
                wxThreadEvent event(EVT_COMMAND_CLEARMAILVIEW, ID_EVT_CLEARMAILVIEW);
                wxQueueEvent(mainFrameHandler, event.Clone());
                database->close();
            }
            database->setLoginInfo(std::string(this->usernameEdit->GetValue().c_str()), std::wstring(this->passwordEdit->GetValue().c_str()));
            
            if (this->passwordEdit->GetValue() == this->passwordRepeatEdit->GetValue()){
                if (database->passwordSecure(std::wstring(this->passwordEdit->GetValue().c_str()))){

                if (database->connect(std::string(ipEdit->GetValue().c_str()))){
                    if(database->versionValid()){
                        if (database->userExists(rsa)){
                            wxMessageBox("Es gibt schon einen Benutzer unter diesem Namen!", "Fehler", wxOK);
                        } else {
                            if (email != L"None"){
                                //Verify Email
                                int code = database->sendVerificationEmail(rsa, std::string(email.begin(), email.end()));
                                wxMessageBox(L"Es wurde eine Email zur Bestätigung Ihrer Email-Adresse verschickt. Bitte geben Sie den dort enthaltenen Bestätigungscode im folgenden Fenster ein.", "Benutzerkonto erstellen", wxOK);
                                wxTextEntryDialog dlg(this, L"Bitte geben Sie den Bestätigungscode ein.", "Email Verifizierung", "", wxOK | wxCANCEL);
                                if(dlg.ShowModal() == wxID_OK){
                                    if(dlg.GetValue() == std::to_string(code)){
                                        wxMessageBox("Der eingegebene Code ist richtig. Das Benutzerkonto kann erstellt werden.", "Email Verifizierung", wxOK);
                                        emailValid = true;
                                    } else {
                                        wxMessageBox("Der eingegebene Code ist falsch. Das Benutzerkonto kann nicht erstellt werden.", "Email Verifizierung", wxOK);
                                        emailValid = false;
                                        this->Close();
                                    }
                                }
                            } 
                            if(emailValid){
                                //Create New User
                                database->setEmail(email);
                                wxThreadEvent event(EVT_COMMAND_REGISTER, ID_EVT_ONREGISTER);
                                wxQueueEvent(mainFrameHandler, event.Clone());

                                this->Close();
                            }
                        }
                    } else {
                        wxMessageBox(L"Die Version dieses Clients stimmt nicht mit der des Servers überein. Bitte laden Sie die neuste Version herunter.", "Versionsfehler", wxOK);
                        wxThreadEvent event(EVT_COMMAND_PERFORMUPDATE, ID_EVT_PERFORMUPDATE);
                        wxQueueEvent(mainFrameHandler, event.Clone());
                    }
                } else {
                    wxMessageBox("Es konnte keine Verbindung zum Server hergestellt werden!", "Fehler", wxOK);
                }
                } else {
                    wxMessageBox(L"Das gewählte Passwort ist unsicher. Das Passwort muss Groß - und Kleinbuchstaben und eine Zahl enthalten. Außerdem sollte es mindestens 8 Zeichen lang sein.", "Fehler", wxOK, this);
                }
            } else {
                wxMessageBox(L"Die Passwörter stimmen nicht überein!", "Fehler", wxOK, this);
            }
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
        if(this->passwordRepeatEdit->GetValue() == ""){
            wxMessageBox("Bitte gib das Passwort zur Bestätigung nochmals ein!", "Fehler", wxOK, this);
        }
    }
}
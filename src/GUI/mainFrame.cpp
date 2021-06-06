#include "mainFrame.h"

wxDEFINE_EVENT(EVT_COMMAND_CLEARMAILVIEW, wxCommandEvent);
wxDEFINE_EVENT(EVT_COMMAND_LOGGEDIN, wxCommandEvent);
wxDEFINE_EVENT(EVT_COMMAND_PERFORMUPDATE, wxCommandEvent);
wxDEFINE_EVENT(EVT_COMMAND_REGISTER, wxCommandEvent);

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_THREAD(UPDATE_THREAD_FINISH, MyFrame::OnUpdateFinish)
    EVT_THREAD(REGISTER_THREAD_FINISH, MyFrame::OnRegisterFinish)
    EVT_THREAD(PULSE_PROGRESS, MyFrame::OnPulseProgress)
wxEND_EVENT_TABLE()

wxThread::ExitCode UpdateThread::Entry(){
        #ifdef LINUX
            database->updateUnderLinux();
        #endif
        #ifdef WINDOWS
            database->updateUnderWindows();
        #endif

        wxThreadEvent event(wxEVT_THREAD, UPDATE_THREAD_FINISH);
        wxQueueEvent(evtHandler, event.Clone());

        return static_cast<wxThread::ExitCode>(NULL); 
    }

UpdateThread::~UpdateThread(){}

wxThread::ExitCode RegisterThread::Entry(){

    rsa->generateKeyPair(NULL);
        
    wxThreadEvent event(wxEVT_THREAD, REGISTER_THREAD_FINISH);
    wxQueueEvent(evtHandler, event.Clone());

    return static_cast<wxThread::ExitCode>(NULL); 
}

RegisterThread::~RegisterThread(){}

WaitThread::~WaitThread(){}

MyFrame::MyFrame(Mail_Database* database, RSA_Encryptor* rsa): wxFrame(NULL, wxID_ANY, "RSA Mail Client") {
    this->database = database;
    this->rsa = rsa;


    #if defined(__WXMSW__)
        this->SetIcon(wxICON(progicon));
    #else
        this->SetIcon(icon_xmp);
    #endif

    wxFont myFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    //Description of the GUI
    wxMenu *menuKey = new wxMenu;
    menuKey->Append(ID_GenKeys, L"rsa-Test", L"Führt den RSA-Test aus!");
    wxMenu *menuConnect = new wxMenu;
    menuConnect->Append(ID_Connect, L"&Anmelden", "Stellt die Verbindung zum Mail-Server her.");
    menuConnect->Append(ID_Registration, L"&Registrieren", "Erstellt ein neues Benutzerkonto.");
    menuConnect->Append(ID_Disconnect, L"&Abmelden", "Trennt die Verbindung zum Mail-Server.");
    menuConnect->Append(ID_Delete, L"&Löschen", L"Löscht das Benutzerkonto.");
    menuConnect->Append(ID_Data_Get, L"&Datenauszug", L"Lädt alle Einträge des Benutzerkontos herunter.");
    wxMenu *menuMessage = new wxMenu;
    menuMessage->Append(ID_Write_Mail, L"&Schreiben", "Eine Nachricht schreiben.");
    menuMessage->Append(ID_Delete_Mail, L"&Löschen", "Aktuelle Nachricht löschen.");
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuConnect, L"&Account");
    menuBar->Append(menuMessage, L"&Nachricht");
    menuBar->Append(menuKey, L"&Schlüssel");
    SetMenuBar( menuBar );

    //wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainBox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hBox1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* hBox3 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* hBox2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* vBox1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vBox2 = new wxBoxSizer(wxVERTICAL);
    //Caption Listbox
    mailFolder = new wxChoice(this, ID_Mail_Folder_Change);
    mailFolder->AppendString("Posteingang");
    mailFolder->AppendString("Gesendete");
    mailFolder->SetSelection(0);
    hBox1->Add(mailFolder, 1);


    inboxListBox = new wxSimpleHtmlListBox(this, ID_ListBox_Inbox);
    vBox1->Add(inboxListBox, 1, wxEXPAND);
    this->Connect(ID_ListBox_Inbox, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(MyFrame::OnMailSelected), NULL, this);
    inboxListBox->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(MyFrame::OnInboxContextMenu), this);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnContextMenuSelected, this, ID_MENU_CONTEXT_INBOX_1);

    //Rich Text Control
    wxStaticText* mailboxText = new wxStaticText(this, wxID_ANY, L"Nachricht:", wxPoint(320, 15));
    hBox1->Add(mailboxText, 3);
    mailText = new wxHtmlWindow(this, ID_Text_Mail);
    wxSystemSettings settings;
    wxColour color = settings.GetColour(wxSYS_COLOUR_WINDOW);
    mailText->SetBackgroundColour(color);
    vBox2->Add(mailText, 1, wxEXPAND);

    senderText = new wxStaticText(this, wxID_ANY, "Sender: ");
    accountText = new wxStaticText(this, wxID_ANY, "Benutzer: ");
    hBox3->Add(accountText, 1, wxEXPAND);
    hBox3->Add(senderText, 3, wxEXPAND);
    
    hBox2->Add(vBox1, 1, wxEXPAND);
    hBox2->Add(vBox2, 3, wxEXPAND);

    mainBox->Add(hBox1, 0, wxEXPAND);
    mainBox->Add(hBox3, 0, wxEXPAND);
    mainBox->Add(hBox2, 1, wxEXPAND);

    this->SetSizer(mainBox);

    CreateStatusBar();
    SetStatusText("RSA Mail Client!");
    Bind(wxEVT_MENU, &MyFrame::OnGenKeys, this, ID_GenKeys);
    Bind(wxEVT_MENU, &MyFrame::OnConnect, this, ID_Connect);
    Bind(wxEVT_MENU, &MyFrame::OnRegister, this, ID_Registration);
    Bind(wxEVT_MENU, &MyFrame::OnDisconnect, this, ID_Disconnect);
    Bind(wxEVT_MENU, &MyFrame::OnWriteMail, this, ID_Write_Mail);
    Bind(wxEVT_MENU, &MyFrame::OnDeleteMail, this, ID_Delete_Mail);
    Bind(wxEVT_MENU, &MyFrame::OnDeleteAccount, this, ID_Delete);
    Bind(wxEVT_MENU, &MyFrame::OnGetData, this, ID_Data_Get);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

    Bind(EVT_COMMAND_PERFORMUPDATE, &MyFrame::performUpdate, this);
    Bind(EVT_COMMAND_CLEARMAILVIEW, &MyFrame::OnClearMailView, this);
    Bind(EVT_COMMAND_LOGGEDIN, &MyFrame::OnLoggedIn, this);
    Bind(EVT_COMMAND_REGISTER, &MyFrame::OnPerformRegister, this);
    //Create Timer
    mailTimer = new wxTimer(this, ID_Timer_Mail_Sync);
    
    this->Connect(ID_Timer_Mail_Sync, wxEVT_TIMER, wxTimerEventHandler(MyFrame::OnMailSyncTimer), NULL, this);
    this->Connect(ID_Mail_Folder_Change, wxEVT_CHOICE, wxCommandEventHandler(MyFrame::OnMailFolderChanged), NULL, this);
}

void MyFrame::OnExit(wxCommandEvent& event){
    Close(true);
}

MyFrame::~MyFrame(){
    delete waitThread;
    delete loginframe;
    delete registerframe;
    delete writerframe;
}

WaitThread *MyFrame::CreateWaitThread()
{
    WaitThread *thread = new WaitThread(this);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
    }

    return thread;
}

UpdateThread *MyFrame::CreateUpdateThread()
{
    UpdateThread *thread = new UpdateThread(this, database);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
    }

    return thread;
}

RegisterThread *MyFrame::CreateRegisterThread(){
    RegisterThread *thread = new RegisterThread(this, rsa);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
    }

    return thread;
}

void MyFrame::performUpdate(wxCommandEvent& event){
    progress = new wxProgressDialog("Update", "Lade Update herunter... Bitte warten.");
    progress->CenterOnParent();
    progress->Show();
    UpdateThread* thread = CreateUpdateThread();
    thread->Run();
    waitThread = CreateWaitThread();
    waitThread->Run();
}

void MyFrame::OnPerformRegister(wxCommandEvent& event){
    progress = new wxProgressDialog("Benutzer erstellen", "Erstelle Benutzer... Bitte warten.", 100, this);
    progress->CenterOnParent();
    progress->Show();
    wxYield();
    RegisterThread* thread = CreateRegisterThread();
    thread->Run();
    waitThread = CreateWaitThread();
    waitThread->Run();
}

void MyFrame::OnPulseProgress(wxThreadEvent& event){
    progress->Pulse();
}

void MyFrame::OnInboxContextMenu(wxContextMenuEvent& event){
    wxMenu menu;
    menu.Append(ID_MENU_CONTEXT_INBOX_1, L"Löschen");
    menu.Append(ID_MENU_CONTEXT_INBOX_2, "Antworten");
    menu.Append(ID_MENU_CONTEXT_INBOX_3, "Weiterleiten");
        
    PopupMenu(&menu); 
}

void MyFrame::OnContextMenuSelected(wxCommandEvent& event){
    switch (event.GetId()) {
            case ID_MENU_CONTEXT_INBOX_1:
            {
                wxThreadEvent event(wxEVT_MENU, ID_Delete_Mail);
                wxQueueEvent(this, event.Clone());
                break;
            }
            case ID_MENU_CONTEXT_INBOX_2:
                std::cout << "Context Menu command 2";
                break;
            case ID_MENU_CONTEXT_INBOX_3:
                std::cout << "Context Menu command 3";
                break;
        }
}

void MyFrame::OnClearMailView(wxCommandEvent& event){
    inboxListBox->Clear();
    mailText->SetPage("");
    wxSystemSettings settings;
    wxColour color = settings.GetColour(wxSYS_COLOUR_WINDOW);
    mailText->SetBackgroundColour(color);
}

void MyFrame::OnLoggedIn(wxCommandEvent& event){
    accountText->SetLabelText(std::string("Benutzer: " + database->getUsername()));
    mailTimer->Start(5000, false);
    mailTimer->Notify();
}

void MyFrame::OnUpdateFinish(wxThreadEvent& event){
    waitThread->Delete();
    progress->Close();
    wxYield();
    wxMessageBox(L"Das Update wurde erfolgreich durchgeführt. Das Programm muss nun neu gestartet werden.", "Update", wxOK);
    this->Close();
}

void MyFrame::OnGenKeys(wxCommandEvent& event) {
    if (wxMessageBox(L"Dies ist eine Entwickleroption zum Test der RSA-Verschlüsselung. Möchten Sie fortfahren?", L"Entwickleroption", wxYES_NO) == wxYES){
        rsa->generateKeyPair(NULL);
        if(rsa->verifyKey()){
            if (rsa->decryptString(rsa->encryptString(L"Hallo", rsa->getPubKey()), rsa->getPubKey(), rsa->getPrivKey()) == "Hallo"){
                wxMessageBox("Der Verschlüsselungstest war erfolgreich.", "RSATest", wxOK);
                std::string tmp = rsa->getD();
                rsa->savePrivKey("priv.key");
                rsa->loadPrivKey("priv.key");
                if (rsa->getD() == tmp){
                    wxMessageBox("Der Speichertest war erfolgreich.", "RSATest", wxOK);
                    if(database->isConnected()){
                        wxCommandEvent event(wxEVT_MENU, ID_Disconnect);
                        wxQueueEvent(this, event.Clone());
                        wxMessageBox("Sie wurden abgemeldet. Bitte melden Sie sich neu an.", "RSATest", wxOK);
                    }
                } else {
                    wxMessageBox(L"Der Speichertest war fehlerhaft.", "Fehler", wxOK);
                }
            } else {
                wxMessageBox(L"Der Verschlüsselungstest war fehlerhaft.", "Fehler", wxOK);
            }
        } else {
            wxMessageBox("Schlüssel ungültig!", "Fehler", wxOK);
        }
    }
}

void MyFrame::OnConnect(wxCommandEvent& event) {
    // Login Dialog
    loginframe = new LoginFrame(database, rsa, this);
    wxSize size;
    size.x = 500;
    size.y = 230;
    loginframe->SetMinSize(size);
    loginframe->SetSize(500, 230);
    loginframe->Show(true);
}

void MyFrame::OnRegister(wxCommandEvent& event) {
    //Register Dialog
    registerframe = new RegisterFrame(database, rsa, this);
    wxSize size;
    size.x = 500;
    size.y = 250;
    registerframe->SetMinSize(size);
    registerframe->SetSize(500, 250);
    registerframe->Show(true);
}

void MyFrame::OnDisconnect(wxCommandEvent& event){
    inboxListBox->Clear();
    mailText->SetPage("");
    wxSystemSettings settings;
    wxColour color = settings.GetColour(wxSYS_COLOUR_WINDOW);
    mailText->SetBackgroundColour(color);
    accountText->SetLabelText("Benutzer: ");
    senderText->SetLabelText("Sender: ");
    if(database->isConnected()){
        database->close();
    }
}

void MyFrame::OnWriteMail(wxCommandEvent& event){
    if(database->isConnected()){
        //Mail Writer Dialog
        writerframe = new MailWriterFrame(database, rsa);
        wxSize size;
        size.x = 800;
        size.y = 500;
        writerframe->SetMinSize(size);
        writerframe->SetSize(800, 500);
        writerframe->Show(true);
    } else {
        wxMessageBox(L"Bitte melden Sie sich erst an.", "Anmeldung erforderlich", wxOK);
    }
}

void MyFrame::OnDeleteMail(wxCommandEvent& event){
    if(inboxListBox->GetSelection() > -1){
        if(wxMessageBox(L"Wollen Sie die Nachricht wirklich löschen?", L"Nachricht löschen", wxYES_NO) == wxYES){
            if(mailFolder->GetSelection() == 0){
                //Posteingang
                database->deleteMail(inboxListBox->GetSelection());
            } else if (mailFolder->GetSelection() == 1){
                //Gesendete
                database->deleteSentMail(inboxListBox->GetSelection());
            }
            mailTimer->Notify();
            wxYield();
            wxMessageBox(L"Die Nachricht wurde für alle gelöscht.", L"Nachricht löschen", wxOK);
        }
    } else {
        wxMessageBox(L"Sie haben keine Nachricht ausgewählt.", L"Nachricht löschen", wxOK);
    }
}

void MyFrame::OnGetData(wxCommandEvent& event){
    if(database->isConnected()){
        database->exportData(std::string(database->getUsername() + ".export"));
        wxMessageBox(std::wstring(L"Die Daten wurden in der Datei " + database->getUsername() + ".export abgelegt."), L"Datenauszug", wxOK);
    } else {
        wxMessageBox(L"Sie sind nicht angemeldet.", L"Datenauszug", wxOK);
    }
}

void MyFrame::OnDeleteAccount(wxCommandEvent& event){
    if (database->isConnected()){
        if(wxMessageBox(L"Möchten Sie wirklich Ihren Account inklusive aller gesendeten Nachrichten löschen?", L"Account löschen", wxYES_NO) == wxYES){
            database->deleteUser();
            wxCommandEvent event(wxEVT_MENU, ID_Disconnect);
            wxQueueEvent(this, event.Clone());
            remove(std::string(database->getUsername() + ".key").c_str());
            wxMessageBox(L"Ihr Account wurde erfolgreich gelöscht.", L"Account löschen", wxOK);
        }
    } else {
        wxMessageBox(L"Sie sind nicht angemeldet.", L"Account löschen", wxOK);
    }
}

void MyFrame::UpdateInboxListBox(int selection){
    inboxListBox->Clear();
    wxSystemSettings settings;
    wxColour color;
    std::vector<Mail_Caption> tmp;
    if(mailFolder->GetSelection() == 0) {
        tmp = database->getMailCaptions();
        for (int i = 0; i < tmp.size(); i++){
            if(i != inboxListBox->GetSelection()){
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXTEXT);
            } else {
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
            }
            if(tmp.at(i).Read == 0){
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).caption + " (Ungelesen)</b> <br>Von: " + database->getSender(rsa, i) + "</font>");
            } else {
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).caption + "</b> <br>Von: " + database->getSender(rsa, i)+ "</font>");
            }
        }
    } else if(mailFolder->GetSelection() == 1) {
        tmp = database->getSentCaptions();
        for (int i = 0; i < tmp.size(); i++){
            if(i != inboxListBox->GetSelection()){
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXTEXT);
            } else {
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
            }
            if(tmp.at(i).Read == 0){
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).caption + " (Ungelesen)</b> <br>An: " + database->getReceiver(rsa, i)+ "</font>");
            } else {
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).caption + "</b> <br>An: " + database->getReceiver(rsa, i)+ "</font>");
            }
        }
    }
    inboxListBox->SetSelection(selection);
}

void MyFrame::OnMailSyncTimer(wxTimerEvent& event) {
    if (database->isConnected()){
        //Get Messages
        if(mailFolder->GetSelection() == 0) {
            if (database->messagesAvailable()){
                database->receiveCaptions(rsa);
                UpdateInboxListBox(-1);
            }
        } else if (mailFolder->GetSelection() == 1){
            if(database->sentMessagesAvailable()){
                database->receiveSentCaptions(rsa);
                UpdateInboxListBox(-1);
            }
            //Check read states
            if (database->updateSentRead(rsa)){
                //Update ListBox
                UpdateInboxListBox(-1);
            }
        }
    } else {
        this->mailTimer->Stop();
    }
    if(inboxListBox->GetSelection() == -1){
        mailText->SetPage("");
        wxSystemSettings settings;
        wxColour color = settings.GetColour(wxSYS_COLOUR_WINDOW);
        mailText->SetBackgroundColour(color);
        senderText->SetLabelText(L"Sender: ");
    }
}

void MyFrame::OnMailFolderChanged(wxCommandEvent& event){
    mailTimer->Notify();
    mailText->SetPage("");
    wxSystemSettings settings;
    wxColour color = settings.GetColour(wxSYS_COLOUR_WINDOW);
    mailText->SetBackgroundColour(color);
    inboxListBox->Clear();
    
    UpdateInboxListBox(-1);
}


void MyFrame::OnMailSelected(wxCommandEvent& event){
    if (mailFolder->GetSelection() == 0){
    if (inboxListBox->GetSelection() > -1){
        //Display selected Mail
        wxSystemSettings settings;
        wxColour color = settings.GetColour(wxSYS_COLOUR_LISTBOXTEXT);
        mailText->SetPage("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+">"+database->receiveMail(rsa, inboxListBox->GetSelection())+"</font>");
        color = settings.GetColour(wxSYS_COLOUR_WINDOW);
        mailText->SetBackgroundColour(color);
        //Gelesen markieren
        if (!database->getRead(inboxListBox->GetSelection())){
            database->setRead(rsa, inboxListBox->GetSelection());
            UpdateInboxListBox(inboxListBox->GetSelection());
        }
        if(database->mailSignatureValid(rsa, inboxListBox->GetSelection())){
            senderText->SetLabelText(std::wstring("Sender: " + database->getSender(rsa, inboxListBox->GetSelection()) + L" (ID OK) Gesendet: " + database->getDate(inboxListBox->GetSelection()) + L" Empfangen: " + database->getReceiveDate(inboxListBox->GetSelection())));
        } else {
            senderText->SetLabelText(std::wstring("Sender: " + database->getSender(rsa, inboxListBox->GetSelection()) + L" (ACHTUNG: ID FALSCH) Gesendet: " + database->getDate(inboxListBox->GetSelection()) + L" Empfangen: " + database->getReceiveDate(inboxListBox->GetSelection())));
        }
    }
    } else if (mailFolder->GetSelection() == 1){
        if (inboxListBox->GetSelection() > -1){
            //Display Mail
            wxSystemSettings settings;
            wxColour color = settings.GetColour(wxSYS_COLOUR_LISTBOXTEXT);
            mailText->SetPage("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+">"+database->receiveSentMail(rsa, inboxListBox->GetSelection())+"</font>");
            color = settings.GetColour(wxSYS_COLOUR_WINDOW);
            mailText->SetBackgroundColour(color);
            if(database->getSentRead(inboxListBox->GetSelection())){
                senderText->SetLabelText(std::wstring(L"Empfänger: "+database->getReceiver(rsa, inboxListBox->GetSelection())+" Gesendet: " + database->getSentDate(inboxListBox->GetSelection()) + L" Empfangen: " + database->getSentReceiveDate(inboxListBox->GetSelection())));
            } else {
                senderText->SetLabelText(std::wstring(L"Empfänger: "+database->getReceiver(rsa, inboxListBox->GetSelection())+" Gesendet: " + database->getSentDate(inboxListBox->GetSelection()) + L" Empfangen: noch nicht"));
            }
        }
    }
}

void MyFrame::OnRegisterFinish(wxThreadEvent& event){
    waitThread->Delete();
    progress->Close();
    progress->Destroy();
    if(!rsa->verifyKey()){
        wxMessageBox("Der Benutzer konnte nicht erstellt werden. Probieren Sie es nocheinmal!", "Fehler", wxOK);
    } else {
        rsa->savePrivKey(std::string(database->getUsername() + ".key"));
        // Save User To Database
        database->createUser(rsa);
        accountText->SetLabelText(std::string("Benutzer: " + database->getUsername()));
        wxMessageBox("Der Benutzer wurde erfolgreich registriert.", "Registrierung", wxOK);
        mailTimer->Start(5000, false);
    }
}
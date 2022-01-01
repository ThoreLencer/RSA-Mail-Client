// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include "mainFrame.h"

//MainFrame

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(ID_Minimize, MainFrame::OnMinimize)
    EVT_MENU(ID_Login, MainFrame::OnLogin)
    EVT_COMMAND(ID_EVT_LOGGEDIN, EVT_COMMAND_LOGGEDIN, MainFrame::OnLoggedIn)
    EVT_COMMAND(ID_EVT_MAINFRAME_ENABLE, EVT_COMMAND_MAINFRAME_ENABLE, MainFrame::OnEnableFrame)
    EVT_TIMER(ID_Timer_Mail_Sync, MainFrame::OnSyncMails)
    EVT_CHOICE(ID_Mail_Folder_Change, MainFrame::OnMailFolderChanged)
    EVT_COMMAND(ID_ListBox_Inbox, wxEVT_COMMAND_LISTBOX_SELECTED, MainFrame::OnMailSelected)
    EVT_MENU(ID_Registration, MainFrame::OnRegister)
    EVT_COMMAND(ID_EVT_ONREGISTER, EVT_COMMAND_REGISTER, MainFrame::OnPerformRegister)
    EVT_THREAD(REGISTER_THREAD_FINISH, MainFrame::OnRegisterFinish)
    EVT_THREAD(PULSE_PROGRESS, MainFrame::OnPulseProgress)
    EVT_MENU(ID_Logout, MainFrame::OnLogout)
    EVT_MENU(ID_Delete_Account, MainFrame::OnDeleteAccount)
    EVT_MENU(ID_Data_Get, MainFrame::OnGetData)
    EVT_MENU(ID_Write_Mail, MainFrame::OnWriteMail)
    EVT_MENU(ID_Delete_Mail, MainFrame::OnDeleteMail)
    EVT_COMMAND(ID_MENU_CONTEXT_INBOX_1, wxEVT_COMMAND_MENU_SELECTED, MainFrame::OnContextMenuSelected)
    EVT_COMMAND(ID_MENU_CONTEXT_INBOX_2, wxEVT_COMMAND_MENU_SELECTED, MainFrame::OnContextMenuSelected)
    EVT_COMMAND(ID_MENU_CONTEXT_INBOX_3, wxEVT_COMMAND_MENU_SELECTED, MainFrame::OnContextMenuSelected)
    EVT_COMMAND(ID_EVT_CLEARMAILVIEW, EVT_COMMAND_CLEARMAILVIEW, MainFrame::OnClearMailView)
    EVT_COMMAND(ID_EVT_PERFORMUPDATE, EVT_COMMAND_PERFORMUPDATE, MainFrame::OnPerformUpdate)
    EVT_THREAD(UPDATE_THREAD_FINISH, MainFrame::OnUpdateFinish)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_MENU(ID_Update, MainFrame::OnCheckUpdate)
    EVT_COMMAND(ID_Button_Download_Attachment, wxEVT_COMMAND_BUTTON_CLICKED, MainFrame::OnDownloadAttachment)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(): wxFrame(NULL, wxID_ANY, "RSA Mail Client"){
    rsa = new RSA_Encryptor;
    rsa->init();
    client = new Client();
    database = new Database();
    this->SetIcon(icon_xmp);
    //Menubar
    wxMenu* menuProgram = new wxMenu;
    menuProgram->Append(ID_Minimize, L"&Minimieren", "Minimiert das Programm und fügt es der Taskleiste an.");
    menuProgram->Append(ID_Update, L"&Update", "Sucht nach Updates.");
    menuProgram->Append(wxID_EXIT, L"&Beenden", "Beendet das Programm.");
    wxMenu *menuConnect = new wxMenu;
    menuConnect->Append(ID_Login, L"&Anmelden", "Stellt die Verbindung zum Mail-Server her.");
    menuConnect->Append(ID_Registration, L"&Registrieren", "Erstellt ein neues Benutzerkonto.");
    menuConnect->Append(ID_Logout, L"&Abmelden", "Trennt die Verbindung zum Mail-Server.");
    menuConnect->Append(ID_Delete_Account, L"&Löschen", L"Löscht das Benutzerkonto.");
    menuConnect->Append(ID_Data_Get, L"&Datenauszug", L"Lädt alle Einträge des Benutzerkontos herunter.");
    wxMenu *menuMessage = new wxMenu;
    menuMessage->Append(ID_Write_Mail, L"&Schreiben", "Eine Nachricht schreiben.");
    menuMessage->Append(ID_Delete_Mail, L"&Löschen", "Aktuelle Nachricht löschen.");
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuProgram, L"&Programm");
    menuBar->Append(menuConnect, L"&Account");
    menuBar->Append(menuMessage, L"&Nachricht");
    SetMenuBar( menuBar );
    //GUI Design
    //Panel for TAB Order working
    wxPanel* panel = new wxPanel(this, ID_Main_Panel, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    wxBoxSizer* mainBox = new wxBoxSizer(wxVERTICAL);
    //Posteingang/Gesendete - Nachricht
    wxBoxSizer* hBox1 = new wxBoxSizer(wxHORIZONTAL);
    mailFolder = new wxChoice(panel, ID_Mail_Folder_Change);
    mailFolder->AppendString("Posteingang");
    mailFolder->AppendString("Gesendete");
    mailFolder->SetSelection(0);
    hBox1->Add(mailFolder, 1, wxEXPAND);
    wxStaticText* mailboxText = new wxStaticText(panel, wxID_ANY, L"Nachricht:");
    hBox1->Add(mailboxText, 3, wxEXPAND);
    mainBox->Add(hBox1, 0, wxEXPAND);

    //Benutzer - Sender
    wxBoxSizer* hBox2 = new wxBoxSizer(wxHORIZONTAL);
    accountText = new wxStaticText(panel, ID_Account_Text, "Benutzer: ");
    accountText->SetLabelText(std::string("Benutzer: "));
    

    hBox2->Add(accountText, 1, wxEXPAND);
    senderText = new wxStaticText(panel, ID_Sender_Text, "Sender: ");
    hBox2->Add(senderText, 3, wxEXPAND);
    mainBox->Add(hBox2, 0, wxEXPAND);

    //InBox - MailText
    wxBoxSizer* hBox3 = new wxBoxSizer(wxHORIZONTAL);
    inboxListBox = new InboxWidget(panel, ID_ListBox_Inbox);
    hBox3->Add(inboxListBox, 1, wxEXPAND);
    wxBoxSizer* vBox1 = new wxBoxSizer(wxVERTICAL);
    mailText = new MailViewWidget(panel);
    vBox1->Add(mailText, 8, wxEXPAND);
    wxStaticText* attachText = new wxStaticText(panel, wxID_ANY, L"Anhang:");
    vBox1->Add(attachText);
    attachListBox = new wxListBox(panel, ID_ListBox_Inbox_Attachment);
    vBox1->Add(attachListBox, 2, wxEXPAND);
    wxButton* downloadAttachment = new wxButton(panel, ID_Button_Download_Attachment, "Anhang herunterladen");
    vBox1->Add(downloadAttachment, 0, wxCENTER);

    hBox3->Add(vBox1, 3, wxEXPAND);
    mainBox->Add(hBox3, 1, wxEXPAND);


    panel->SetSizer(mainBox);

    //Create Timer
    mailTimer = new wxTimer(this, ID_Timer_Mail_Sync);
}

void MainFrame::OnExit(wxCommandEvent& event){
    Close(true);
}

void MainFrame::OnClose(wxCloseEvent& event){
    //Ask if minimize
    if(event.CanVeto()){
        if(wxMessageBox(L"Sie sind dabei das Programm zu schließen. Soll es stattdessen minimiert werden?", L"Schließen", wxYES_NO) == wxYES){
            event.Veto();
            TBIcon* tbIcon = new TBIcon(this);
            //Hide Frame
            this->Show(false);
            return;
        } 
    }
    Destroy();
}

void MainFrame::OnMinimize(wxCommandEvent& event){
    TBIcon* tbIcon = new TBIcon(this);
    //Hide Frame
    this->Show(false);
}
    
void MainFrame::OnLogin(wxCommandEvent& event){
    // Login Dialog
    LoginFrame* loginframe = new LoginFrame(rsa, client, this);
    wxSize size;
    size.x = 500;
    size.y = 230;
    loginframe->SetMinSize(size);
    loginframe->SetSize(500, 230);
    loginframe->Show(true);

    Disable();
}

void MainFrame::OnLoggedIn(wxCommandEvent& event){
    accountText->SetLabelText(std::string("Benutzer: " + client->userName));
    mailTimer->Start(10000, false);
    mailTimer->Notify();

    Enable();
}

void MainFrame::OnEnableFrame(wxCommandEvent& event){
    Enable();
}

void MainFrame::OnSyncMails(wxTimerEvent& event){
    //Old Non_Read Message Count
    int oldcount = inboxListBox->GetItemCount();     
    if (client->ServerReachable()){
        //Get Messages
        if(mailFolder->GetSelection() == 0) {
            if (client->GetCountToUser(rsa) > oldcount){
                database->RecvCaptions = client->GetCaptionsToUser(rsa);
                //Send Notification and Receive Messages
                for(int i = oldcount; i < database->RecvCaptions.size(); i++){
                    database->RecvMessages.push_back(client->GetMessage(rsa, database->RecvCaptions.at(i).ID));
                    if(database->RecvCaptions.at(i).read == 0){
                        wxNotificationMessage notif(L"Nachricht erhalten", std::wstring(L"Sie haben eine Nachricht von " + client->GetUsername(rsa, database->RecvCaptions.at(i).Person) + L" erhalten."));
                        notif.Show(0);
                    }
                }
                UpdateInboxListBox(-1);
            }
        } else if (mailFolder->GetSelection() == 1){
            if(client->GetCountFromUser(rsa) > oldcount){
                database->SentCaptions = client->GetCaptionsFromUser(rsa);
                //Receive Messages
                for(int i = oldcount; i < database->SentCaptions.size(); i++){
                    database->SentMessages.push_back(client->GetMessage(rsa, database->SentCaptions.at(i).ID));
                }
                UpdateInboxListBox(-1);
            }
            //Check read states
            for (int i = 0; i < database->SentCaptions.size(); i++){
                if (database->SentCaptions.at(i).read == 0){
                    if(database->SentCaptions.at(i).read != client->GetRead(rsa, database->SentCaptions.at(i).ID)){
                        database->SentCaptions.at(i).read = 1;
                        //Update ListBox
                        UpdateInboxListBox(-1);
                    }
                }
            }
        }
    } else {
        mailTimer->Stop();
        wxMessageBox(L"Sie wurden abgemeldet.", L"Verbindungsabbruch", wxOK);
        wxCommandEvent event(wxEVT_MENU, ID_Logout);
        wxQueueEvent(this, event.Clone());
    }
    if(inboxListBox->GetSelection() == -1){
        mailText->SetPage("");
        senderText->SetLabelText(L"Sender: ");
    }
}

void MainFrame::UpdateInboxListBox(int selection){
    inboxListBox->Clear();
    wxSystemSettings settings;
    wxColour color;
    std::vector<Caption> tmp;
    if(mailFolder->GetSelection() == 0) {
        tmp = database->RecvCaptions;
        for (int i = 0; i < tmp.size(); i++){
            if(i != inboxListBox->GetSelection()){
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXTEXT);
            } else {
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
            }
            if(tmp.at(i).read == 0){
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).Caption + " (Ungelesen)</b> <br>Von: " + client->GetUsername(rsa, tmp.at(i).Person) + "</font>");
            } else {
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).Caption + "</b> <br>Von: " + client->GetUsername(rsa, tmp.at(i).Person) + "</font>");
            }
        }
    } else if(mailFolder->GetSelection() == 1) {
        tmp = database->SentCaptions;
        for (int i = 0; i < tmp.size(); i++){
            if(i != inboxListBox->GetSelection()){
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXTEXT);
            } else {
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
            }
            if(tmp.at(i).read == 0){
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).Caption + " (Ungelesen)</b> <br>An: " + client->GetUsername(rsa, tmp.at(i).Person)+ "</font>");
            } else {
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).Caption + "</b> <br>An: " + client->GetUsername(rsa, tmp.at(i).Person)+ "</font>");
            }
        }
    }
    inboxListBox->SetSelection(selection);
}

void MainFrame::OnMailFolderChanged(wxCommandEvent& event){
    mailTimer->Notify();
    inboxListBox->Clear();
    attachListBox->Clear();
    mailText->SetPage("");
    
    UpdateInboxListBox(-1);
}

std::wstring addHTMLBrNewLn(std::wstring input){
    std::wstring output = std::regex_replace(input, std::wregex(L"\n"), L"<br>");

    return output;
}

void MainFrame::OnMailSelected(wxCommandEvent& event){
    if (mailFolder->GetSelection() == 0){
    if (inboxListBox->GetSelection() > -1){
        //Display selected Mail
        wxSystemSettings settings;
        wxColour color = settings.GetColour(wxSYS_COLOUR_LISTBOXTEXT);
        mailText->SetPage("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+">"+addHTMLBrNewLn(database->RecvMessages.at(inboxListBox->GetSelection()))+"</font>");
        //Load Attachments
        //std::vector<std::string> attachNames = database->getAttachedFilenames(rsa, inboxListBox->GetSelection());
        //attachListBox->Clear();
        //for(int i = 0; i < attachNames.size(); i++) {attachListBox->Append(attachNames.at(i));}
        //Gelesen markieren
        if (database->RecvCaptions.at(inboxListBox->GetSelection()).read == 0){
            client->SetRead(database, rsa, inboxListBox->GetSelection());
            UpdateInboxListBox(inboxListBox->GetSelection());
        }
        senderText->SetLabelText(std::wstring("Sender: " + client->GetUsername(rsa, database->RecvCaptions.at(inboxListBox->GetSelection()).Person) + L" (ID OK) Gesendet: " + database->RecvCaptions.at(inboxListBox->GetSelection()).Date + L" Empfangen: " + database->RecvCaptions.at(inboxListBox->GetSelection()).ReceiveDate));
       
    }
    } else if (mailFolder->GetSelection() == 1){
        if (inboxListBox->GetSelection() > -1){
            //Display Mail
            wxSystemSettings settings;
            wxColour color = settings.GetColour(wxSYS_COLOUR_LISTBOXTEXT);
            mailText->SetPage("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+">"+addHTMLBrNewLn(database->SentMessages.at(inboxListBox->GetSelection()))+"</font>");
            //Load Attachments
            //std::vector<std::string> attachNames = database->getSentAttachedFilenames(rsa, inboxListBox->GetSelection());
            //attachListBox->Clear();
            //for(int i = 0; i < attachNames.size(); i++) {attachListBox->Append(attachNames.at(i));}
            if(database->SentCaptions.at(inboxListBox->GetSelection()).read == 1){
                senderText->SetLabelText(std::wstring(L"Empfänger: "+client->GetUsername(rsa, database->SentCaptions.at(inboxListBox->GetSelection()).Person)+" Gesendet: " + database->SentCaptions.at(inboxListBox->GetSelection()).Date + L" Empfangen: " + database->SentCaptions.at(inboxListBox->GetSelection()).ReceiveDate));
            } else {
                senderText->SetLabelText(std::wstring(L"Empfänger: "+client->GetUsername(rsa, database->SentCaptions.at(inboxListBox->GetSelection()).Person)+" Gesendet: " + database->SentCaptions.at(inboxListBox->GetSelection()).Date + L" Empfangen: noch nicht"));
            }
        }
    }
}

void MainFrame::OnRegister(wxCommandEvent& event) {
    //Register Dialog
    RegisterFrame* registerframe = new RegisterFrame(rsa, client, this);
    wxSize size;
    size.x = 500;
    size.y = 250;
    registerframe->SetMinSize(size);
    registerframe->SetSize(500, 250);
    registerframe->Show(true);

    Disable();
}

void MainFrame::OnPerformRegister(wxCommandEvent& event){
    progress = new wxProgressDialog("Benutzer erstellen", "Erstelle Benutzer... Bitte warten.", 100, this);
    progress->CenterOnParent();
    progress->Show();
    wxYield();
    RegisterThread* thread = CreateRegisterThread();
    thread->Run();
    pulseThread = CreatePulseThread();
    pulseThread->Run();
}

void MainFrame::OnRegisterFinish(wxThreadEvent& event){
    pulseThread->Delete();
    progress->Close();
    progress->Destroy();
    if(!rsa->verifyKey()){
        wxMessageBox("Der Benutzer konnte nicht erstellt werden. Probieren Sie es nocheinmal!", "Fehler", wxOK);
    } else {
        rsa->savePrivKey(std::string(client->userName + ".key"));
        // Save User To Database
        client->CreateUser(rsa);
        accountText->SetLabelText(std::string("Benutzer: " + client->userName));
        wxMessageBox("Der Benutzer wurde erfolgreich registriert.", "Registrierung", wxOK);
        mailTimer->Start(5000, false);
    }
    Enable();
}

void MainFrame::OnPulseProgress(wxThreadEvent& event){
    progress->Pulse();
}

void MainFrame::OnLogout(wxCommandEvent& event){
    inboxListBox->Clear();
    mailText->SetPage("");
    accountText->SetLabelText("Benutzer: ");
    senderText->SetLabelText("Sender: ");
    client->loggedIn = false;
}

void MainFrame::OnDeleteAccount(wxCommandEvent& event){
    /*if (database->isConnected()){
        if(wxMessageBox(L"Möchten Sie wirklich Ihren Account inklusive aller gesendeten Nachrichten löschen?", L"Account löschen", wxYES_NO) == wxYES){
            database->deleteUser();
            wxCommandEvent event(wxEVT_MENU, ID_Logout);
            wxQueueEvent(this, event.Clone());
            remove(std::string(database->getUsername() + ".key").c_str());
            wxMessageBox(L"Ihr Account wurde erfolgreich gelöscht.", L"Account löschen", wxOK);
        }
    } else {
        wxMessageBox(L"Sie sind nicht angemeldet.", L"Account löschen", wxOK);
    }*/
}

void MainFrame::OnGetData(wxCommandEvent& event){
    /*if(database->isConnected()){
        database->exportData(std::string(database->getUsername() + ".export"));
        wxMessageBox(std::wstring(L"Die Daten wurden in der Datei " + database->getUsername() + ".export abgelegt."), L"Datenauszug", wxOK);
    } else {
        wxMessageBox(L"Sie sind nicht angemeldet.", L"Datenauszug", wxOK);
    }*/
}

void MainFrame::OnWriteMail(wxCommandEvent& event){
    if(client->loggedIn){
        //Mail Writer Dialog
        MailWriterFrame* writerframe = new MailWriterFrame(rsa, client, L"", "");
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

void MainFrame::OnDeleteMail(wxCommandEvent& event){
    if(inboxListBox->GetSelection() > -1){
        if(wxMessageBox(L"Wollen Sie die Nachricht wirklich löschen?", L"Nachricht löschen", wxYES_NO) == wxYES){
            if(mailFolder->GetSelection() == 0){
                //Posteingang
                client->DeleteMessage(rsa, database->RecvCaptions.at(inboxListBox->GetSelection()).ID);
                inboxListBox->Delete(inboxListBox->GetSelection());
            } else if (mailFolder->GetSelection() == 1){
                //Gesendete
                client->DeleteMessage(rsa, database->SentCaptions.at(inboxListBox->GetSelection()).ID);
                inboxListBox->Delete(inboxListBox->GetSelection());
            }
            mailTimer->Notify();
            wxYield();
            wxMessageBox(L"Die Nachricht wurde für alle gelöscht.", L"Nachricht löschen", wxOK);
        }
    } else {
        wxMessageBox(L"Sie haben keine Nachricht ausgewählt.", L"Nachricht löschen", wxOK);
    }
}

void MainFrame::OnContextMenuSelected(wxCommandEvent& event){
    switch (event.GetId()) {
            case ID_MENU_CONTEXT_INBOX_1:
            {
                wxThreadEvent event(wxEVT_MENU, ID_Delete_Mail);
                wxQueueEvent(this, event.Clone());
                break;
            }
            case ID_MENU_CONTEXT_INBOX_2:
                //Reply
                if(mailFolder->GetSelection() == 0){
                    if(inboxListBox->GetSelection() > -1){
                        //Mail Writer Dialog
                        //MailWriterFrame* writerframe = new MailWriterFrame(rsa, std::wstring(L"RE: " + database->getCaption(inboxListBox->GetSelection())), database->getSender(rsa, inboxListBox->GetSelection()));
                        wxSize size;
                        size.x = 800;
                        size.y = 500;
                        //writerframe->SetMinSize(size);
                        //writerframe->SetSize(800, 500);
                        //writerframe->Show(true);
                    } else {
                        wxMessageBox(L"Sie haben keine Nachricht ausgewählt.", "Fehler", wxOK);
                    }
                } else {
                    wxMessageBox(L"Es können nur für empfangene Nachrichten Antworten verfasst werden.", "Fehler", wxOK);
                }
                break;
            case ID_MENU_CONTEXT_INBOX_3:
                //forward
                if(inboxListBox->GetSelection() > -1){
                    wxTextEntryDialog dlg(this, L"Bitte geben Sie den Empfänger ein.", "Weiterleiten", "", wxOK | wxCANCEL);
                    if(dlg.ShowModal() == wxID_OK){
                        /*int recvId = database->userExists(rsa, std::string(dlg.GetValue().c_str()));
                        if(recvId > -1){
                            
                        } else {
                            wxMessageBox(L"Der eingegebene Empfänger konnte nicht gefunden werden.", "Fehler", wxOK);
                        }*/
                    }
                
                } else {
                    wxMessageBox(L"Sie haben keine Nachricht ausgewählt.", "Fehler", wxOK);
                }
                break;
        }
}

void MainFrame::OnClearMailView(wxCommandEvent& event){
    inboxListBox->Clear();
    attachListBox->Clear();
    mailText->SetPage("");
}

void MainFrame::OnPerformUpdate(wxCommandEvent& event){
    progress = new wxProgressDialog("Update", "Lade Update herunter... Bitte warten.");
    progress->CenterOnParent();
    progress->Show();
    UpdateThread* thread = CreateUpdateThread();
    thread->Run();
    pulseThread = CreatePulseThread();
    pulseThread->Run();
}

void MainFrame::OnUpdateFinish(wxThreadEvent& event){
    pulseThread->Delete();
    wxYield();
    progress->Close();
    progress->Destroy();
    wxYield();
    wxMessageBox(L"Das Update wurde erfolgreich durchgeführt. Das Programm muss nun neu gestartet werden.", "Update", wxOK);
    wxYield();
    Close(true);
}

void MainFrame::OnCheckUpdate(wxCommandEvent& event){
    /*if (!database->isConnected()){
        wxTextEntryDialog dlg(this, L"IP-Adresse:", "Datenbankserver", "rsamail.ddns.net", wxOK | wxCANCEL);
        if(dlg.ShowModal() == wxID_OK){
            if (!database->connect(std::string(dlg.GetValue().c_str()))){
                wxMessageBox(L"Es konnte keine Verbindung zum Server aufgebaut werden.", "Update", wxOK);
                return;
            }
        } else {
            return;
        }
    }

    if(database->versionValid()){
        wxMessageBox(L"Das Programm ist auf aktuellstem Stand.", "Update", wxOK);
    } else {
        if(wxMessageBox(L"Die Version dieses Clients stimmt nicht mit der des Servers überein. Soll die neueste Version jetzt heruntergeladen werden?", "Versionsfehler", wxYES_NO) == wxYES){
            wxThreadEvent event(EVT_COMMAND_PERFORMUPDATE, ID_EVT_PERFORMUPDATE);
            wxQueueEvent(this, event.Clone());
        }
    }*/
}

void MainFrame::OnDownloadAttachment(wxCommandEvent& event){
    /*if(database->isConnected()){
        if(mailFolder->GetSelection() == 0){
            if(inboxListBox->GetSelection()>-1){
                if(database->getAttachedFilenames(rsa, inboxListBox->GetSelection()).size() > 0){
                    wxDirDialog dlg(NULL, L"Zielordner auswählen", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
                    if (dlg.ShowModal() == wxID_CANCEL)
                        return; 
                    
                    database->downloadAttachment(rsa, inboxListBox->GetSelection(), std::string(dlg.GetPath().c_str()));

                    wxMessageBox(L"Der Anhang wurde erfolgreich heruntergeladen.", "Anhang herunterladen", wxOK);

                } else {
                    wxMessageBox(L"Diese Nachricht enthält keinen Anhang.", "Anhang herunterladen", wxOK);
                }
            } else {
                wxMessageBox(L"Sie haben keine Nachricht ausgewählt.", "Anhang herunterladen", wxOK);
            }
        } else {
            wxMessageBox(L"Anhänge können nur von empfangenen Nachrichten heruntergeladen werden.", "Anhang herunterladen", wxOK);
        }
    } else {
        wxMessageBox(L"Sie sind nicht angemeldet.", "Anhang herunterladen", wxOK);
    }*/
}

// MailViewWidget

wxBEGIN_EVENT_TABLE(MailViewWidget, wxHtmlWindow)
    EVT_ERASE_BACKGROUND(MailViewWidget::OnEraseBgEvent)
wxEND_EVENT_TABLE()

void MailViewWidget::OnEraseBgEvent(wxEraseEvent& event)
{
    wxSystemSettings settings;
    wxColour color = settings.GetColour(wxSYS_COLOUR_WINDOW);
    this->SetBackgroundColour(color);
    event.Skip(true);
}

//RegisterThread

RegisterThread *MainFrame::CreateRegisterThread(){
    RegisterThread *thread = new RegisterThread(this, rsa);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
    }

    return thread;
}

wxThread::ExitCode RegisterThread::Entry(){

    rsa->generateKeyPair(NULL, 1024);
        
    wxThreadEvent event(wxEVT_THREAD, REGISTER_THREAD_FINISH);
    wxQueueEvent(evtHandler, event.Clone());

    return static_cast<wxThread::ExitCode>(NULL); 
}

//WaitThread

PulseThread *MainFrame::CreatePulseThread()
{
    PulseThread *thread = new PulseThread(this);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
    }

    return thread;
}

//UpdateThread

UpdateThread *MainFrame::CreateUpdateThread()
{
    UpdateThread *thread = new UpdateThread(this);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
    }

    return thread;
}

wxThread::ExitCode UpdateThread::Entry(){
    #ifdef LINUX
        //database->updateUnderLinux();
    #endif
    #ifdef WINDOWS
        //database->updateUnderWindows();
    #endif

    wxThreadEvent event(wxEVT_THREAD, UPDATE_THREAD_FINISH);
    wxQueueEvent(evtHandler, event.Clone());

    return static_cast<wxThread::ExitCode>(NULL); 
}

//InboxWidget

wxBEGIN_EVENT_TABLE(InboxWidget, wxSimpleHtmlListBox)
    EVT_CONTEXT_MENU(InboxWidget::OnInboxContextMenu)
wxEND_EVENT_TABLE()

void InboxWidget::OnEraseBgEvent(wxEraseEvent& event){
    wxSystemSettings settings;
    wxColour color = settings.GetColour(wxSYS_COLOUR_WINDOW);
    this->SetBackgroundColour(color);
    event.Skip(true);
}

void InboxWidget::OnInboxContextMenu(wxContextMenuEvent& event){
    wxMenu menu;
    menu.Append(ID_MENU_CONTEXT_INBOX_1, L"Löschen");
    menu.Append(ID_MENU_CONTEXT_INBOX_2, "Antworten");
    menu.Append(ID_MENU_CONTEXT_INBOX_3, "Weiterleiten");
        
    PopupMenu(&menu); 
}
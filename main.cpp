// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only


#include <iostream>
#include <stdio.h>
#include "rsa_encryption.h"
#include "database.h"
#include <mysql/mysql.h>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/richtext/richtextctrl.h>
#include <wx/progdlg.h>  
#include <wx/thread.h>
#include <wx/dynarray.h>
#include <wx/htmllbox.h>
#include <wx/settings.h>

//Library 1: Installing gmp on Linux mingw cross compile for windows 64bit:
//download gmp library
//cd into the library folder
//run: make clean
//run: export CC=x86_64-w64-mingw32-g++
//run: export CC_FOR_BUILD=x86_64-linux-gnu-g++
//run: ./configure --build=x86_64-linux-gnu --host=x86_64-w64-mingw32 --enable-cxx --prefix=/usr/x86_64-w64-mingw32
//run: make -j8
//run: make install
//Library 2: do same for wxWidgets
//Library 3: mysql port for MINGw : https://github.com/reminisc3/mysql-mingw64-port

//TODO
//Kontaktbuch
//Rechtsklick Kontextmenü: Löschen, Antworten, Weiterleiten

enum
{
    ID_GenKeys,
    ID_Connect,
    ID_Registration,
    ID_Disconnect,
    ID_Delete,
    ID_Data_Get,
    ID_Button_Login,
    ID_Button_Send,
    ID_Button_Register,
    ID_Button_PasswdReset,
    ID_Edit_IP,
    ID_Edit_Username,
    ID_Edit_Password,
    ID_Edit_Password_Repeat,
    ID_Edit_To,
    ID_Edit_Caption,
    ID_Edit_Mail,
    ID_Edit_Email,
    ID_Timer_Mail_Sync,
    ID_ListBox_Inbox,
    ID_Text_Mail,
    ID_Write_Mail,
    ID_Delete_Mail,
    ID_Mail_Folder_Change,
    UPDATE_THREAD_FINISH,
    PULSE_PROGRESS,
    REGISTER_THREAD_FINISH
};

class WaitThread;
class UpdateThread;
WX_DEFINE_ARRAY_PTR(wxThread *, wxArrayThread);

class MyApp : public wxApp {
    wxLocale m_locale;
public:
    virtual bool OnInit();

    wxCriticalSection m_critsect;
    wxArrayThread m_threads;
    wxSemaphore m_semAllDone;
    bool m_shuttingDown;
};

class MyFrame : public wxFrame {
public:
    MyFrame();
    ~MyFrame();
    wxTimer* mailTimer;
    wxSimpleHtmlListBox* inboxListBox;
    wxRichTextCtrl* mailText;
    wxStaticText* senderText;
    wxStaticText* accountText;
    wxChoice* mailFolder;

    void performUpdate();


    WaitThread *CreateWaitThread();
    WaitThread* waitThread;
private:
    void OnGenKeys(wxCommandEvent& event);
    void OnConnect(wxCommandEvent& event);
    void OnRegister(wxCommandEvent& event);
    void OnDisconnect(wxCommandEvent& event);
    void OnMailSyncTimer(wxTimerEvent& event);
    void OnMailSelected(wxCommandEvent& event);
    void OnWriteMail(wxCommandEvent& event);
    void OnUpdateFinish(wxThreadEvent& event);
    void OnRegisterFinish(wxThreadEvent& event);
    void OnPulseProgress(wxThreadEvent& event);
    void OnMailFolderChanged(wxCommandEvent& event);
    void OnDeleteMail(wxCommandEvent& event);
    void OnDeleteAccount(wxCommandEvent& event);
    void OnGetData(wxCommandEvent& event);
    void UpdateInboxListBox();

    UpdateThread *CreateUpdateThread();
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_THREAD(UPDATE_THREAD_FINISH, MyFrame::OnUpdateFinish)
    EVT_THREAD(REGISTER_THREAD_FINISH, MyFrame::OnRegisterFinish)
    EVT_THREAD(PULSE_PROGRESS, MyFrame::OnPulseProgress)
wxEND_EVENT_TABLE()

class WaitThread
  : public wxThread
{
public:
  WaitThread(wxEvtHandler* frame){
      this->evtHandler = frame;
  }
  ~WaitThread();
private:
  wxEvtHandler* evtHandler;
protected:
  virtual ExitCode Entry()
  {
    while(!TestDestroy()) {
        wxThreadEvent event(wxEVT_THREAD, PULSE_PROGRESS);
        wxQueueEvent(evtHandler, event.Clone());
        Sleep(150);
    }
    return static_cast<ExitCode>(NULL);
  }
};

class UpdateThread : public wxThread
{
public:
    UpdateThread(wxEvtHandler *frame){
        this->evtHandler = frame;
    }

    virtual ExitCode Entry();
    ~UpdateThread();

public:
    wxEvtHandler *evtHandler;
};

class RegisterThread : public wxThread
{
public:
    RegisterThread(wxEvtHandler *frame){
        this->evtHandler = frame;
    }

    virtual ExitCode Entry();
    ~RegisterThread();

public:
    wxEvtHandler *evtHandler;
};

class LoginFrame : public wxFrame {
public:
    LoginFrame();
private:
    wxTextCtrl* ipEdit;
    wxTextCtrl* usernameEdit;
    wxTextCtrl* passwordEdit;
    void OnLogin(wxCommandEvent& event);
    void OnPasswordReset(wxCommandEvent& event);
};

class RegisterFrame : public wxFrame {
public:
    RegisterFrame();
private:
    wxTextCtrl* ipEdit;
    wxTextCtrl* usernameEdit;
    wxTextCtrl* emailEdit;
    wxTextCtrl* passwordEdit;
    wxTextCtrl* passwordRepeatEdit;
    wxDialog* tmpDlg;
    void OnRegister(wxCommandEvent& event);

    RegisterThread *CreateRegisterThread();
};

class MailWriterFrame : public wxFrame {
public:
    MailWriterFrame();
private:
    wxTextCtrl* toEdit;
    wxTextCtrl* captionEdit;
    wxRichTextCtrl* mailEdit;
    void OnSend(wxCommandEvent& event);
};

wxIMPLEMENT_APP(MyApp);

void deleteOld(){
    remove("RSA_Client_Old.exe");
    remove("RSA_Client_Old");
}

//Global Vars
static RSA_Encryptor rsa;
static Mail_Database database;
static LoginFrame *loginframe;
static RegisterFrame* registerframe;
static MailWriterFrame* writerframe;
static MyFrame *frame;
static wxProgressDialog* progress;
static std::wstring email;

wxThread::ExitCode UpdateThread::Entry(){
        #ifdef LINUX
            database.updateUnderLinux();
        #endif
        #ifdef WINDOWS
            database.updateUnderWindows();
        #endif

        wxThreadEvent event(wxEVT_THREAD, UPDATE_THREAD_FINISH);
        wxQueueEvent(evtHandler, event.Clone());

        return static_cast<wxThread::ExitCode>(NULL); 
    }

UpdateThread::~UpdateThread(){
        wxCriticalSectionLocker locker(wxGetApp().m_critsect);

        wxArrayThread& threads = wxGetApp().m_threads;
        threads.Remove(this);

        if ( threads.IsEmpty() )
        {
            // signal the main thread that there are no more threads left if it is
            // waiting for us
            if ( wxGetApp().m_shuttingDown )
            {
                wxGetApp().m_shuttingDown = false;

                wxGetApp().m_semAllDone.Post();
            }
        }
}

wxThread::ExitCode RegisterThread::Entry(){

    rsa.generateKeyPair(NULL);
        
    wxThreadEvent event(wxEVT_THREAD, REGISTER_THREAD_FINISH);
    wxQueueEvent(evtHandler, event.Clone());

    return static_cast<wxThread::ExitCode>(NULL); 
}

RegisterThread::~RegisterThread(){
        wxCriticalSectionLocker locker(wxGetApp().m_critsect);

        wxArrayThread& threads = wxGetApp().m_threads;
        threads.Remove(this);

        if ( threads.IsEmpty() )
        {
            // signal the main thread that there are no more threads left if it is
            // waiting for us
            if ( wxGetApp().m_shuttingDown )
            {
                wxGetApp().m_shuttingDown = false;

                wxGetApp().m_semAllDone.Post();
            }
        }
}

WaitThread::~WaitThread(){
       wxCriticalSectionLocker locker(wxGetApp().m_critsect);

        wxArrayThread& threads = wxGetApp().m_threads;
        threads.Remove(this);

        if ( threads.IsEmpty() )
        {
            // signal the main thread that there are no more threads left if it is
            // waiting for us
            if ( wxGetApp().m_shuttingDown )
            {
                wxGetApp().m_shuttingDown = false;

                wxGetApp().m_semAllDone.Post();
            }
        }
  }

bool MyApp::OnInit() {
    m_shuttingDown = false;
    //Delete old Updatefiles
    deleteOld();
    //Init Application
    m_locale.Init(m_locale.GetSystemLanguage(), wxLOCALE_LOAD_DEFAULT);
    frame = new MyFrame();
    wxSize size;
    size.x = 800;
    size.y = 500;
    frame->SetMinSize(size);
    frame->SetSize(800, 500);
    frame->Show(true);
    database.init();
    rsa.init();
    return true;
}

MyFrame::MyFrame(): wxFrame(NULL, wxID_ANY, "RSA Mail Client") {
    wxFont myFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    //Description of the GUI
    wxMenu *menuKey = new wxMenu;
    menuKey->Append(ID_GenKeys, L"&RSA-Test", L"Führt den RSA-Test aus!");
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
    //Rich Text Control
    wxStaticText* mailboxText = new wxStaticText(this, wxID_ANY, L"Nachricht:", wxPoint(320, 15));
    hBox1->Add(mailboxText, 3);
    mailText = new wxRichTextCtrl(this, ID_Text_Mail, "", wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE | wxRE_READONLY);
    mailText->SetFont(myFont);
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
    //Create Timer
    mailTimer = new wxTimer(this, ID_Timer_Mail_Sync);
    
    this->Connect(ID_Timer_Mail_Sync, wxEVT_TIMER, wxTimerEventHandler(MyFrame::OnMailSyncTimer), NULL, this);
    this->Connect(ID_Mail_Folder_Change, wxEVT_CHOICE, wxCommandEventHandler(MyFrame::OnMailFolderChanged), NULL, this);
}

MyFrame::~MyFrame(){
    this->Disconnect(ID_Timer_Mail_Sync, wxEVT_TIMER, wxTimerEventHandler(MyFrame::OnMailSyncTimer), NULL, this);
    {
        wxCriticalSectionLocker locker(wxGetApp().m_critsect);

        // check if we have any threads running first
        const wxArrayThread& threads = wxGetApp().m_threads;
        size_t count = threads.GetCount();

        if ( !count )
            return;

        // set the flag indicating that all threads should exit
        wxGetApp().m_shuttingDown = true;
    }

    // now wait for them to really terminate
    wxGetApp().m_semAllDone.Wait();
}

WaitThread *MyFrame::CreateWaitThread()
{
    WaitThread *thread = new WaitThread(frame);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
    }

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);
    wxGetApp().m_threads.Add(thread);

    return thread;
}

UpdateThread *MyFrame::CreateUpdateThread()
{
    UpdateThread *thread = new UpdateThread(frame);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
    }

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);
    wxGetApp().m_threads.Add(thread);

    return thread;
}

RegisterThread *RegisterFrame::CreateRegisterThread(){
    RegisterThread *thread = new RegisterThread(frame);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
    }

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);
    wxGetApp().m_threads.Add(thread);

    return thread;
}

void MyFrame::performUpdate(){
    progress = new wxProgressDialog("Update", "Lade Update herunter... Bitte warten.");
    progress->CenterOnParent();
    progress->Show();
    UpdateThread* thread = CreateUpdateThread();
    thread->Run();
    waitThread = CreateWaitThread();
    waitThread->Run();
}

void MyFrame::OnPulseProgress(wxThreadEvent& event){
    progress->Pulse();
}

void MyFrame::OnUpdateFinish(wxThreadEvent& event){
    waitThread->Delete();
    progress->Close();
    wxYield();
    wxMessageBox(L"Das Update wurde erfolgreich durchgeführt. Das Programm muss nun neu gestartet werden.", "Update", wxOK);
    wxGetApp().Exit();
}

void MyFrame::OnGenKeys(wxCommandEvent& event) {
    if (wxMessageBox(L"Dies ist eine Entwickleroption zum Test der RSA-Verschlüsselung. Möchten Sie fortfahren?", L"Entwickleroption", wxYES_NO) == wxYES){
        rsa.generateKeyPair(NULL);
        if(rsa.verifyKey()){
            if (rsa.decryptString(rsa.encryptString(L"Hallo", rsa.getPubKey()), rsa.getPubKey(), rsa.getPrivKey()) == "Hallo"){
                wxMessageBox("Der Verschlüsselungstest war erfolgreich.", "RSATest", wxOK);
                std::string tmp = rsa.getD();
                rsa.savePrivKey("priv.key");
                rsa.loadPrivKey("priv.key");
                if (rsa.getD() == tmp){
                    wxMessageBox("Der Speichertest war erfolgreich.", "RSATest", wxOK);
                    if(database.isConnected()){
                        wxCommandEvent event(wxEVT_MENU, ID_Disconnect);
                        wxQueueEvent(frame, event.Clone());
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
    loginframe = new LoginFrame();
    wxSize size;
    size.x = 500;
    size.y = 230;
    loginframe->SetMinSize(size);
    loginframe->SetSize(500, 230);
    loginframe->Show(true);
}

void MyFrame::OnRegister(wxCommandEvent& event) {
    //Register Dialog
    registerframe = new RegisterFrame();
    wxSize size;
    size.x = 500;
    size.y = 250;
    registerframe->SetMinSize(size);
    registerframe->SetSize(500, 250);
    registerframe->Show(true);
}

void MyFrame::OnDisconnect(wxCommandEvent& event){
    inboxListBox->Clear();
    mailText->Clear();
    accountText->SetLabelText("Benutzer: ");
    senderText->SetLabelText("Sender: ");
    if(database.isConnected()){
        database.close();
    }
}

void MyFrame::OnWriteMail(wxCommandEvent& event){
    if(database.isConnected()){
        //Mail Writer Dialog
        writerframe = new MailWriterFrame();
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
                database.deleteMail(inboxListBox->GetSelection());
            } else if (mailFolder->GetSelection() == 1){
                //Gesendete
                database.deleteSentMail(inboxListBox->GetSelection());
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
    if(database.isConnected()){
        database.exportData(std::string(database.getUsername() + ".export"));
        wxMessageBox(std::wstring(L"Die Daten wurden in der Datei " + database.getUsername() + ".export abgelegt."), L"Datenauszug", wxOK);
    } else {
        wxMessageBox(L"Sie sind nicht angemeldet.", L"Datenauszug", wxOK);
    }
}

void MyFrame::OnDeleteAccount(wxCommandEvent& event){
    if (database.isConnected()){
        if(wxMessageBox(L"Möchten Sie wirklich Ihren Account inklusive aller gesendeten Nachrichten löschen?", L"Account löschen", wxYES_NO) == wxYES){
            database.deleteUser();
            wxCommandEvent event(wxEVT_MENU, ID_Disconnect);
            wxQueueEvent(frame, event.Clone());
            remove(std::string(database.getUsername() + ".key").c_str());
            wxMessageBox(L"Ihr Account wurde erfolgreich gelöscht.", L"Account löschen", wxOK);
        }
    } else {
        wxMessageBox(L"Sie sind nicht angemeldet.", L"Account löschen", wxOK);
    }
}

void MyFrame::UpdateInboxListBox(){
    inboxListBox->Clear();
    wxSystemSettings settings;
    wxColour color;
    std::vector<Mail_Caption> tmp;
    if(mailFolder->GetSelection() == 0) {
        tmp = database.getMailCaptions();
        for (int i = 0; i < tmp.size(); i++){
            if(i != inboxListBox->GetSelection()){
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXTEXT);
            } else {
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
            }
            if(tmp.at(i).Read == 0){
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).caption + " (Ungelesen)</b> <br>Von: " + database.getSender(&rsa, i) + "</font>");
            } else {
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).caption + "</b> <br>Von: " + database.getSender(&rsa, i)+ "</font>");
            }
        }
    } else if(mailFolder->GetSelection() == 1) {
        tmp = database.getSentCaptions();
        for (int i = 0; i < tmp.size(); i++){
            if(i != inboxListBox->GetSelection()){
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXTEXT);
            } else {
                color = settings.GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
            }
            if(tmp.at(i).Read == 0){
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).caption + " (Ungelesen)</b> <br>An: " + database.getReceiver(&rsa, i)+ "</font>");
            } else {
                inboxListBox->Append("<font color="+color.GetAsString(wxC2S_HTML_SYNTAX)+"><b>"+tmp.at(i).caption + "</b> <br>An: " + database.getReceiver(&rsa, i)+ "</font>");
            }
        }
    }
}

void MyFrame::OnMailSyncTimer(wxTimerEvent& event) {
    if (database.isConnected()){
        //Get Messages
        if(mailFolder->GetSelection() == 0) {
            if (database.messagesAvailable()){
                database.receiveCaptions(&rsa);
                UpdateInboxListBox();
            }
        } else if (mailFolder->GetSelection() == 1){
            if(database.sentMessagesAvailable()){
                database.receiveSentCaptions(&rsa);
                UpdateInboxListBox();
            }
            //Check read states
            if (database.updateSentRead(&rsa)){
                //Update ListBox
                UpdateInboxListBox();
            }
        }
    } else {
        frame->mailTimer->Stop();
    }
    if(inboxListBox->GetSelection() == -1){
        mailText->Clear();
        senderText->SetLabelText(L"Sender: ");
    }
}

void MyFrame::OnMailFolderChanged(wxCommandEvent& event){
    mailTimer->Notify();
    mailText->Clear();
    inboxListBox->Clear();
    
    UpdateInboxListBox();
}


void MyFrame::OnMailSelected(wxCommandEvent& event){
    if (mailFolder->GetSelection() == 0){
    if (inboxListBox->GetSelection() > -1){
        //Display selected Mail
        mailText->Clear();
        mailText->WriteText(database.receiveMail(&rsa, inboxListBox->GetSelection()));
        //Gelesen markieren
        if (!database.getRead(inboxListBox->GetSelection())){
            database.setRead(&rsa, inboxListBox->GetSelection());
            UpdateInboxListBox();
        }
        if(database.mailSignatureValid(&rsa, inboxListBox->GetSelection())){
            senderText->SetLabelText(std::wstring("Sender: " + database.getSender(&rsa, inboxListBox->GetSelection()) + L" (ID OK) Gesendet: " + database.getDate(inboxListBox->GetSelection()) + L" Empfangen: " + database.getReceiveDate(inboxListBox->GetSelection())));
        } else {
            senderText->SetLabelText(std::wstring("Sender: " + database.getSender(&rsa, inboxListBox->GetSelection()) + L" (ACHTUNG: ID FALSCH) Gesendet: " + database.getDate(inboxListBox->GetSelection()) + L" Empfangen: " + database.getReceiveDate(inboxListBox->GetSelection())));
        }
    }
    } else if (mailFolder->GetSelection() == 1){
        if (inboxListBox->GetSelection() > -1){
            //Display Mail
            mailText->Clear();
            mailText->WriteText(database.receiveSentMail(&rsa, inboxListBox->GetSelection()));
            if(database.getSentRead(inboxListBox->GetSelection())){
                senderText->SetLabelText(std::wstring(L"Empfänger: "+database.getReceiver(&rsa, inboxListBox->GetSelection())+" Gesendet: " + database.getSentDate(inboxListBox->GetSelection()) + L" Empfangen: " + database.getSentReceiveDate(inboxListBox->GetSelection())));
            } else {
                senderText->SetLabelText(std::wstring(L"Empfänger: "+database.getReceiver(&rsa, inboxListBox->GetSelection())+" Gesendet: " + database.getSentDate(inboxListBox->GetSelection()) + L" Empfangen: noch nicht"));
            }
        }
    }
}

void LoginFrame::OnLogin(wxCommandEvent& event) {  
    if (loginframe->ipEdit->GetValue() != "" && loginframe->usernameEdit->GetValue() != "" && loginframe->passwordEdit->GetValue() != ""){
        if (database.isConnected()){
            frame->inboxListBox->Clear();
            frame->mailText->Clear();
            database.close();
        }
        database.setLoginInfo(std::string(loginframe->usernameEdit->GetValue().c_str()), std::wstring(loginframe->passwordEdit->GetValue().c_str()));
        
        if (database.connect(std::string(ipEdit->GetValue().c_str()))){
            if(database.versionValid()){
                if (database.userExists(&rsa)){
                    // Load Keys
                    rsa.loadPrivKey(std::string(database.getUsername() + ".key"));
                    rsa.setE(database.getE(&rsa));
                    rsa.setN(database.getN(&rsa));
                    //Check password
                    if(database.passwordValid(&rsa)){
                        wxMessageBox("Die Anmeldung ist erfolgreich.", "Anmeldung", wxOK);
                        frame->accountText->SetLabelText(std::string("Benutzer: " + database.getUsername()));
                        frame->mailTimer->Start(5000, false);
                        frame->mailTimer->Notify();

                        loginframe->Close();
                    } else {
                        wxMessageBox("Das eingegebene Passwort ist falsch. Probieren Sie es nocheinmal.", "Anmeldung", wxOK);
                        if (database.isConnected()){
                            database.close();
                        }
                    }
                } else {
                    // User does not exist
                    wxMessageBox(L"Der angegebene Benutzer konnte nicht gefunden werden. Nutzen Sie die Registrierung um ein Benutzerkonto anzulegen.", "Anmeldung", wxOK);
                }
            } else {
                if(wxMessageBox(L"Die Version dieses Clients stimmt nicht mit der des Servers überein. Soll die neueste Version jetzt heruntergeladen werden?", "Versionsfehler", wxYES_NO) == wxYES){
                    frame->performUpdate();
                }
            }
        } else {
            wxMessageBox("Es konnte keine Verbindung zum Server hergestellt werden!", "Fehler", wxOK);
        }
    } else {
        if (loginframe->ipEdit->GetValue() == ""){
            wxMessageBox("Bitte gib eine IP ein!", "Fehler", wxOK, loginframe);
        } 
        if(loginframe->usernameEdit->GetValue() == ""){
            wxMessageBox("Bitte gib einen Benutzername ein!", "Fehler", wxOK, loginframe);
        } 
        if(loginframe->passwordEdit->GetValue() == ""){
            wxMessageBox("Bitte gib ein Passwort ein!", "Fehler", wxOK, loginframe);
        }
    }
}

void LoginFrame::OnPasswordReset(wxCommandEvent& event){
    if (loginframe->ipEdit->GetValue() != "" && loginframe->usernameEdit->GetValue() != ""){
        if (database.isConnected()){
            frame->inboxListBox->Clear();
            frame->mailText->Clear();
            database.close();
        }
        database.setLoginInfo(std::string(loginframe->usernameEdit->GetValue().c_str()), std::wstring(loginframe->passwordEdit->GetValue().c_str()));
        
        if (database.connect(std::string(ipEdit->GetValue().c_str()))){
            if(database.versionValid()){
                if (database.userExists(&rsa)){
                    // Load Keys
                    rsa.loadPrivKey(std::string(database.getUsername() + ".key"));
                    rsa.setE(database.getE(&rsa));
                    rsa.setN(database.getN(&rsa));
                    //Change Password
                    if (database.hasEmail()){
                        //Email Request
                        int code = database.sendPasswordResetEmail(&rsa);
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
                                            if (database.passwordSecure(passwd1)){
                                                //Change Password
                                                database.changePassword(&rsa, passwd1);
                                                wxMessageBox(L"Das Passwort wurde erfolgreich geändert. Sie könnten sich nun anmelden.", L"Passwort ändern", wxOK);
                                                database.close();
                                            } else {
                                                wxMessageBox(L"Das gewählte Passwort ist unsicher. Das Passwort muss Groß - und Kleinbuchstaben und eine Zahl enthalten. Außerdem sollte es mindestens 8 Zeichen lang sein.", "Fehler", wxOK, registerframe);
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
                        loginframe->Close();
                    } else {
                        if(wxMessageBox(L"In Ihrem Account ist keine Email-Adresse hinterlegt. Wollen Sie das Benutzerkonto löschen? Sie können sich anschließend neu registieren.", "Passwort zurücksetzen", wxYES_NO) == wxYES){
                            database.deleteUser();
                            wxCommandEvent event(wxEVT_MENU, ID_Disconnect);
                            wxQueueEvent(frame, event.Clone());
                            remove(std::string(database.getUsername() + ".key").c_str());
                            wxMessageBox(L"Ihr Account wurde erfolgreich gelöscht.", L"Account löschen", wxOK);
                            loginframe->Close();
                        }
                    }
                } else {
                    // User does not exist
                    wxMessageBox(L"Der angegebene Benutzer konnte nicht gefunden werden. Nutzen Sie die Registrierung um ein Benutzerkonto anzulegen.", "Passwort zurücksetzen", wxOK);
                }
            } else {
                if(wxMessageBox(L"Die Version dieses Clients stimmt nicht mit der des Servers überein. Soll die neueste Version jetzt heruntergeladen werden?", "Versionsfehler", wxYES_NO) == wxYES){
                    frame->performUpdate();
                }
            }
        } else {
            wxMessageBox("Es konnte keine Verbindung zum Server hergestellt werden!", "Passwort zurücksetzen", wxOK);
        }
    } else {
        if (loginframe->ipEdit->GetValue() == ""){
            wxMessageBox("Bitte gib eine IP ein!", "Passwort zurücksetzen", wxOK, loginframe);
        } 
        if(loginframe->usernameEdit->GetValue() == ""){
            wxMessageBox("Bitte gib einen Benutzername ein!", "Passwort zurücksetzen", wxOK, loginframe);
        } 
    }
}

LoginFrame::LoginFrame(): wxFrame(NULL, wxID_ANY, "Anmelden") {
    wxFont myFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    //Description of the GUI
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(3, 2, 5, 5);

    wxStaticText* ipText = new wxStaticText(this, wxID_ANY, L"IP-Adresse:", wxPoint(10, 15));
    ipText->SetFont(myFont);
    gridSizer->Add(ipText, 1, wxEXPAND);
    ipEdit = new wxTextCtrl(this, ID_Edit_IP, L"rsamail.ddns.net", wxPoint(100, 10), wxSize(100, 30));
    ipEdit->SetFont(myFont);
    gridSizer->Add(ipEdit, 1, wxEXPAND);
    wxStaticText* usernameText = new wxStaticText(this, wxID_ANY, L"Benutzername:", wxPoint(10, 65));
    usernameText->SetFont(myFont);
    gridSizer->Add(usernameText, 1, wxEXPAND);
    usernameEdit = new wxTextCtrl(this, ID_Edit_Username, L"", wxPoint(100, 60), wxSize(100, 30));
    usernameEdit->SetFont(myFont);
    gridSizer->Add(usernameEdit, 1, wxEXPAND);
    wxStaticText* passwordText = new wxStaticText(this, wxID_ANY, L"Passwort:", wxPoint(10, 125));
    passwordText->SetFont(myFont);
    gridSizer->Add(passwordText, 1, wxEXPAND);
    passwordEdit = new wxTextCtrl(this, ID_Edit_Password, L"", wxPoint(100, 110), wxSize(100, 30), wxTE_PASSWORD);
    passwordEdit->SetFont(myFont);
    gridSizer->Add(passwordEdit, 1, wxEXPAND);
    gridSizer->AddGrowableCol(1, 1);
    wxButton* login = new wxButton(this, ID_Button_Login, L"Anmelden", wxPoint(100, 150), wxSize(100, 50));
    wxButton* passwdReset = new wxButton(this, ID_Button_PasswdReset, L"Passwort vergessen");

    mainSizer->Add(gridSizer, 1, wxEXPAND);
    mainSizer->Add(login, 0, wxEXPAND);
    mainSizer->Add(passwdReset, 0, wxEXPAND);
    this->SetSizer(mainSizer);
    Bind(wxEVT_BUTTON, &LoginFrame::OnLogin, this, ID_Button_Login);
    Bind(wxEVT_BUTTON, &LoginFrame::OnPasswordReset, this, ID_Button_PasswdReset);
}

MailWriterFrame::MailWriterFrame(): wxFrame(NULL, wxID_ANY, "Nachricht schreiben"){
    wxFont myFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* toText = new wxStaticText(this, wxID_ANY, L"An:");
    toEdit = new wxTextCtrl(this, ID_Edit_To, "");
    toEdit->SetFont(myFont);
    mainSizer->Add(toText, 0, wxEXPAND);
    mainSizer->Add(toEdit, 0, wxEXPAND);

    wxStaticText* captionText = new wxStaticText(this, wxID_ANY, L"Betreff:");
    captionEdit = new wxTextCtrl(this, ID_Edit_Caption, "");
    captionEdit->SetFont(myFont);
    mainSizer->Add(captionText, 0, wxEXPAND);
    mainSizer->Add(captionEdit, 0, wxEXPAND);

    wxStaticText* mailboxText = new wxStaticText(this, wxID_ANY, L"Nachricht:");
    mainSizer->Add(mailboxText, 0, wxEXPAND);

    mailEdit = new wxRichTextCtrl(this, ID_Edit_Mail, "");
    mailEdit->SetFont(myFont);
    mainSizer->Add(mailEdit, 1, wxEXPAND);

    wxButton* sendButton = new wxButton(this, ID_Button_Send, L"Senden");
    mainSizer->Add(sendButton, 0, wxCENTER);

    Bind(wxEVT_BUTTON, &MailWriterFrame::OnSend, this, ID_Button_Send);

    this->SetSizer(mainSizer);
}

void MailWriterFrame::OnSend(wxCommandEvent& event) {
    // Check if Boxes are filled
    if (captionEdit->GetValue() != "" && toEdit->GetValue() != ""){
        //Check if Receiver exists
        if (std::wstring(mailEdit->GetValue().c_str()).length() < 50000){
            std::string recv = std::string(toEdit->GetValue().c_str());
            int recvID = database.userExists(&rsa, recv);
            if(recvID > -1){
                database.sendMail(&rsa, recvID, std::wstring(captionEdit->GetValue().c_str()), std::wstring(mailEdit->GetValue().c_str()), database.userKey(&rsa, recv));
                wxMessageBox(L"Die Nachricht wurde versandt!", "Nachricht schreiben", wxOK, writerframe);
                writerframe->Close();
            } else {
                wxMessageBox(L"Der angegebene Empfänger konnte nicht gefunden werden!", "Fehler", wxOK, writerframe);
            }
        } else {
            wxMessageBox(L"Sie haben die maximale Zeichenanzahl von 50000 überschritten. Bitte halten Sie sich etwas kürzer.", "Fehler", wxOK, writerframe);
        }

    } else {
        if (captionEdit->GetValue() == ""){
            wxMessageBox(L"Bitte gib einen Betreff ein!", "Fehler", wxOK, writerframe);
        } 
        if (toEdit->GetValue() == ""){
            wxMessageBox(L"Bitte gib einen Empfänger ein!", "Fehler", wxOK, writerframe);
        }
    }
}

RegisterFrame::RegisterFrame(): wxFrame(NULL, wxID_ANY, "Registrierung") {
    wxFont myFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    //Description of the GUI
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(5, 2, 5, 5);

    wxStaticText* ipText = new wxStaticText(this, wxID_ANY, L"IP-Adresse:", wxPoint(10, 15));
    ipText->SetFont(myFont);
    gridSizer->Add(ipText, 1, wxEXPAND);
    ipEdit = new wxTextCtrl(this, ID_Edit_IP, L"rsamail.ddns.net", wxPoint(100, 10), wxSize(100, 30));
    ipEdit->SetFont(myFont);
    gridSizer->Add(ipEdit, 1, wxEXPAND);
    wxStaticText* usernameText = new wxStaticText(this, wxID_ANY, L"Benutzername:", wxPoint(10, 65));
    usernameText->SetFont(myFont);
    gridSizer->Add(usernameText, 1, wxEXPAND);
    usernameEdit = new wxTextCtrl(this, ID_Edit_Username, L"", wxPoint(100, 60), wxSize(100, 30));
    usernameEdit->SetFont(myFont);
    gridSizer->Add(usernameEdit, 1, wxEXPAND);
    wxStaticText* emailText = new wxStaticText(this, wxID_ANY, L"Email:", wxPoint(10, 65));
    emailText->SetFont(myFont);
    gridSizer->Add(emailText, 1, wxEXPAND);
    emailEdit = new wxTextCtrl(this, ID_Edit_Email, L"", wxPoint(100, 60), wxSize(100, 30));
    emailEdit->SetFont(myFont);
    gridSizer->Add(emailEdit, 1, wxEXPAND);
    wxStaticText* passwordText = new wxStaticText(this, wxID_ANY, L"Passwort:", wxPoint(10, 125));
    passwordText->SetFont(myFont);
    gridSizer->Add(passwordText, 1, wxEXPAND);
    passwordEdit = new wxTextCtrl(this, ID_Edit_Password, L"", wxPoint(100, 110), wxSize(100, 30), wxTE_PASSWORD);
    passwordEdit->SetFont(myFont);
    gridSizer->Add(passwordEdit, 1, wxEXPAND);
    wxStaticText* passwordRepeatText = new wxStaticText(this, wxID_ANY, L"wiederholen:", wxPoint(10, 125));
    passwordRepeatText->SetFont(myFont);
    gridSizer->Add(passwordRepeatText, 1, wxEXPAND);
    passwordRepeatEdit = new wxTextCtrl(this, ID_Edit_Password_Repeat, L"", wxPoint(100, 110), wxSize(100, 30), wxTE_PASSWORD);
    passwordRepeatEdit->SetFont(myFont);
    gridSizer->Add(passwordRepeatEdit, 1, wxEXPAND);
    gridSizer->AddGrowableCol(1, 1);

    wxButton* registerButton = new wxButton(this, ID_Button_Register, L"Registrieren", wxPoint(100, 150), wxSize(100, 50));

    mainSizer->Add(gridSizer, 1, wxEXPAND);
    mainSizer->Add(registerButton, 0, wxEXPAND);
    this->SetSizer(mainSizer);
    Bind(wxEVT_BUTTON, &RegisterFrame::OnRegister, this, ID_Button_Register);
}

void RegisterFrame::OnRegister(wxCommandEvent& event){
    email = L"None";
    bool continueProcess = true;
    bool emailValid = true;
    if (registerframe->ipEdit->GetValue() != "" && registerframe->usernameEdit->GetValue() != "" && registerframe->passwordEdit->GetValue() != "" && registerframe->passwordRepeatEdit->GetValue() != ""){
        if(registerframe->emailEdit->GetValue() == ""){
            if(!wxMessageBox(L"Sie haben keine Email-Adresse eingegeben. Diese wird benötigt, falls Sie Ihr Passwort vergessen. Falls Sie keine Email-Adresse angeben möchten, können Sie Ihr Benutzerkonto bei Verlust des Passwortes nur noch löschen. Wollen Sie fortfahren?", "Email-Adresse", wxYES_NO) == wxYES){
                continueProcess = false; 
            } 
        }
        if (continueProcess){
            if(registerframe->emailEdit->GetValue() != ""){
                email = registerframe->emailEdit->GetValue();
            }
            if (database.isConnected()){
                frame->inboxListBox->Clear();
                frame->mailText->Clear();
                database.close();
            }
            database.setLoginInfo(std::string(registerframe->usernameEdit->GetValue().c_str()), std::wstring(registerframe->passwordEdit->GetValue().c_str()));
            
            if (registerframe->passwordEdit->GetValue() == registerframe->passwordRepeatEdit->GetValue()){
                if (database.passwordSecure(std::wstring(registerframe->passwordEdit->GetValue().c_str()))){

                if (database.connect(std::string(ipEdit->GetValue().c_str()))){
                    if(database.versionValid()){
                        if (database.userExists(&rsa)){
                            wxMessageBox("Es gibt schon einen Benutzer unter diesem Namen!", "Fehler", wxOK);
                        } else {
                            if (email != L"None"){
                                //Verify Email
                                int code = database.sendVerificationEmail(&rsa, std::string(email.begin(), email.end()));
                                wxMessageBox(L"Es wurde eine Email zur Bestätigung Ihrer Email-Adresse verschickt. Bitte geben Sie den dort enthaltenen Bestätigungscode im folgenden Fenster ein.", "Benutzerkonto erstellen", wxOK);
                                wxTextEntryDialog dlg(this, L"Bitte geben Sie den Bestätigungscode ein.", "Email Verifizierung", "", wxOK | wxCANCEL);
                                if(dlg.ShowModal() == wxID_OK){
                                    if(dlg.GetValue() == std::to_string(code)){
                                        wxMessageBox("Der eingegebene Code ist richtig. Das Benutzerkonto kann erstellt werden.", "Email Verifizierung", wxOK);
                                        emailValid = true;
                                    } else {
                                        wxMessageBox("Der eingegebene Code ist falsch. Das Benutzerkonto kann nicht erstellt werden.", "Email Verifizierung", wxOK);
                                        emailValid = false;
                                        registerframe->Close();
                                    }
                                }
                            } 
                            if(emailValid){
                                //Create New User
                                progress = new wxProgressDialog("Benutzer erstellen", "Erstelle Benutzer... Bitte warten.", 100, frame);
                                progress->CenterOnParent();
                                progress->Show();
                                wxYield();
                                RegisterThread* thread = CreateRegisterThread();
                                thread->Run();
                                frame->waitThread = frame->CreateWaitThread();
                                frame->waitThread->Run();

                                registerframe->Close();
                            }
                        }
                    } else {
                        wxMessageBox(L"Die Version dieses Clients stimmt nicht mit der des Servers überein. Bitte laden Sie die neuste Version herunter.", "Versionsfehler", wxOK);
                        frame->performUpdate();
                    }
                } else {
                    wxMessageBox("Es konnte keine Verbindung zum Server hergestellt werden!", "Fehler", wxOK);
                }
                } else {
                    wxMessageBox(L"Das gewählte Passwort ist unsicher. Das Passwort muss Groß - und Kleinbuchstaben und eine Zahl enthalten. Außerdem sollte es mindestens 8 Zeichen lang sein.", "Fehler", wxOK, registerframe);
                }
            } else {
                wxMessageBox(L"Die Passwörter stimmen nicht überein!", "Fehler", wxOK, registerframe);
            }
        }
    } else {
        if (registerframe->ipEdit->GetValue() == ""){
            wxMessageBox("Bitte gib eine IP ein!", "Fehler", wxOK, registerframe);
        } 
        if(registerframe->usernameEdit->GetValue() == ""){
            wxMessageBox("Bitte gib einen Benutzername ein!", "Fehler", wxOK, registerframe);
        } 
        if(registerframe->passwordEdit->GetValue() == ""){
            wxMessageBox("Bitte gib ein Passwort ein!", "Fehler", wxOK, registerframe);
        }
        if(registerframe->passwordRepeatEdit->GetValue() == ""){
            wxMessageBox("Bitte gib das Passwort zur Bestätigung nochmals ein!", "Fehler", wxOK, registerframe);
        }
    }
}

void MyFrame::OnRegisterFinish(wxThreadEvent& event){
    waitThread->Delete();
    progress->Close();
    progress->Destroy();
    if(!rsa.verifyKey()){
        wxMessageBox("Der Benutzer konnte nicht erstellt werden. Probieren Sie es nocheinmal!", "Fehler", wxOK);
    } else {
        rsa.savePrivKey(std::string(database.getUsername() + ".key"));
        // Save User To Database
        database.createUser(&rsa, email);
        accountText->SetLabelText(std::string("Benutzer: " + database.getUsername()));
        wxMessageBox("Der Benutzer wurde erfolgreich registriert.", "Registrierung", wxOK);
        mailTimer->Start(5000, false);
    }
}
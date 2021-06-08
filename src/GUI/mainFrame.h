#pragma once
#include <wx/wx.h>
#include <wx/htmllbox.h>
#include <wx/progdlg.h>
#include "login.h"
#include "mailwriter.h"
#include "register.h"
#include "../define.h"
#include "../Icon.xpm.h"
#include "taskbarIcon.h"
#include <wx/notifmsg.h>
#include <wx/taskbar.h>
#include <regex>

class WaitThread;
class UpdateThread;
class RegisterThread;

class MyFrame : public wxFrame {
public:
    MyFrame();
private:
    RSA_Encryptor* rsa;
    Mail_Database* database;
    WaitThread *CreateWaitThread();
    WaitThread* waitThread;
    wxProgressDialog* progress;
    TBIcon* tbIcon;
    wxTimer* mailTimer;
    wxSimpleHtmlListBox* inboxListBox;
    wxHtmlWindow* mailText;
    wxStaticText* senderText;
    wxStaticText* accountText;
    wxChoice* mailFolder;

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
    void OnClearMailView(wxCommandEvent& event);
    void OnLoggedIn(wxCommandEvent& event);
    void UpdateInboxListBox(int selection);
    void OnPerformRegister(wxCommandEvent& event);
    void OnInboxContextMenu(wxContextMenuEvent& event);
    void OnContextMenuSelected(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnMinimize(wxCommandEvent& event);
    void performUpdate(wxCommandEvent& event); 

    UpdateThread *CreateUpdateThread();
    RegisterThread *CreateRegisterThread();
    wxDECLARE_EVENT_TABLE();
};

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
    UpdateThread(wxEvtHandler *frame, Mail_Database* database){
        this->evtHandler = frame;
        this->database = database;
    }

    virtual ExitCode Entry();
    ~UpdateThread();

private:
    wxEvtHandler *evtHandler;
    Mail_Database* database;
};

class RegisterThread : public wxThread
{
public:
    RegisterThread(wxEvtHandler *frame,RSA_Encryptor* rsa){
        this->evtHandler = frame;
        this->rsa = rsa;
    }

    virtual ExitCode Entry();
    ~RegisterThread();

private:
    wxEvtHandler *evtHandler;
    RSA_Encryptor* rsa;
};
// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include <wx/wx.h>
#include <wx/event.h>
#include <wx/html/htmlproc.h>
#include <wx/html/htmlwin.h>
#include <wx/htmllbox.h>
#include <wx/notifmsg.h>
#include <wx/dirdlg.h>
#include "taskbarIcon.h"
#include "../Encryptor/rsa_encryption.h"
#include "../Encryptor/file_encryption.h"
#include "../Database/database.h"
#include "../Icon.xpm.h"
#include "../define.h"
#include "login.h"
#include "register.h"
#include "mailwriter.h"
#include "../Network/network.h"
#include <regex>

class UpdateThread : public wxThread
{
public:
    UpdateThread(wxEvtHandler *frame){
        this->evtHandler = frame;
    }

    virtual ExitCode Entry();

private:
    wxEvtHandler *evtHandler;
};

class RegisterThread : public wxThread
{
public:
    RegisterThread(wxEvtHandler *frame,RSA_Encryptor* rsa){
        this->evtHandler = frame;
        this->rsa = rsa;
    }

    virtual ExitCode Entry();

private:
    wxEvtHandler *evtHandler;
    RSA_Encryptor* rsa;
};

class PulseThread
  : public wxThread
{
public:
  PulseThread(wxEvtHandler* frame){
      this->evtHandler = frame;
  }
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

class MailViewWidget : public wxHtmlWindow {
  public:
    MailViewWidget(wxWindow *parent) : wxHtmlWindow( parent )
    {
    }
  private:
    void OnEraseBgEvent(wxEraseEvent& event);
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(MailViewWidget);
};

class InboxWidget : public wxSimpleHtmlListBox {
    public:
        InboxWidget(wxWindow* parent, wxWindowID id) : wxSimpleHtmlListBox(parent, id){

        }
    private:
        void OnInboxContextMenu(wxContextMenuEvent& event);
        void OnEraseBgEvent(wxEraseEvent& event);
        wxDECLARE_EVENT_TABLE();
        wxDECLARE_NO_COPY_CLASS(InboxWidget);
};

class MainFrame : public wxFrame {
public:
    MainFrame();
private:

    RSA_Encryptor* rsa;
    Database* database;
    Client* client;
    MailViewWidget* mailText;
    InboxWidget* inboxListBox;
    wxChoice* mailFolder;
    wxStaticText* senderText;
    wxStaticText* accountText;
    wxTimer* mailTimer;
    wxProgressDialog* progress;
    PulseThread* pulseThread;
    wxListBox* attachListBox;

    void OnExit(wxCommandEvent& event);
    void OnMinimize(wxCommandEvent& event);
    void OnLogin(wxCommandEvent& event);
    void OnLoggedIn(wxCommandEvent& event);
    void OnEnableFrame(wxCommandEvent& event);
    void OnSyncMails(wxTimerEvent& event);
    void UpdateInboxListBox(int selection);
    void OnMailFolderChanged(wxCommandEvent& event);
    void OnMailSelected(wxCommandEvent& event);
    void OnRegister(wxCommandEvent& event);
    void OnPerformRegister(wxCommandEvent& event);
    void OnRegisterFinish(wxThreadEvent& event);
    void OnPulseProgress(wxThreadEvent& event);
    void OnLogout(wxCommandEvent& event);
    void OnDeleteAccount(wxCommandEvent& event);
    void OnGetData(wxCommandEvent& event);
    void OnWriteMail(wxCommandEvent& event);
    void OnDeleteMail(wxCommandEvent& event);
    void OnInboxContextMenu(wxContextMenuEvent& event);
    void OnContextMenuSelected(wxCommandEvent& event);
    void OnClearMailView(wxCommandEvent& event);
    void OnPerformUpdate(wxCommandEvent& event); 
    void OnUpdateFinish(wxThreadEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnCheckUpdate(wxCommandEvent& event);
    void OnDownloadAttachment(wxCommandEvent& event);

    RegisterThread *CreateRegisterThread();
    PulseThread *CreatePulseThread();
    UpdateThread *CreateUpdateThread();

    wxDECLARE_EVENT_TABLE();
};
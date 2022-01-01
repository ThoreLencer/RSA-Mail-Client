// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

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
#include <wx/html/htmlwin.h>

#include <iostream>
#include <stdio.h>
#include "define.h"
#include "GUI/mainFrame.h"
#include "Encryptor/rsa_encryption.h"
#include "GUI/login.h"
#include "GUI/mailwriter.h"
#include "GUI/register.h"
#include <mysql/mysql.h>

#include <time.h>

//Library 1: Installing gmp on Linux mingw cross compile for windows 64bit:
//download gmp library
//cd into the library folder
//run: make clean
//run: export CC=x86_64-w64-mingw32-g++
//run: export CC_FOR_BUILD=x86_64-linux-gnu-g++
//run: ./configure --build=x86_64-linux-gnu --host=x86_64-w64-mingw32 --enable-cxx --prefix=/usr/x86_64-w64-mingw32
//run: make -j8
//run: make install
//Library 2: do same for wxWidgets: ./configure --build=x86_64-linux-gnu --host=x86_64-w64-mingw32 --prefix=/usr/x86_64-w64-mingw32 --disable-shared --with-msw
//Library 3: mysql port for MINGw : https://github.com/reminisc3/mysql-mingw64-port

//TODO
//Kontakte (Anzeigen von Personen, denen man bereits geschrieben hat bzw. bereits eine Nachricht erhalten hat)
//Warteanzeige beim Versenden und Herunterladen der Nachricht/des Anhangs
//MailSync als Thread (Kein stocken bei Sync) -> Status: Synchronisiere...
//Rechtsklick Kontextmenü: Weiterleiten
//Nutzereigene Datenbank -> Nachrichten auf Server löschen nach Empfang

wxDEFINE_EVENT(EVT_COMMAND_CLEARMAILVIEW, wxCommandEvent);
wxDEFINE_EVENT(EVT_COMMAND_LOGGEDIN, wxCommandEvent);
wxDEFINE_EVENT(EVT_COMMAND_PERFORMUPDATE, wxCommandEvent);
wxDEFINE_EVENT(EVT_COMMAND_REGISTER, wxCommandEvent);
wxDEFINE_EVENT(EVT_COMMAND_MAINFRAME_ENABLE, wxCommandEvent);

class MyApp : public wxApp {
private:
    wxLocale* m_locale;
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

void deleteOld(){
    remove("RSA_Client_Old.exe");
    remove("RSA_Client_Old");
}

//Reimplement Features:
//-Delete Account
//-Get Data Extract
//-Update
//-Attachments

bool MyApp::OnInit() {
    srand (time(NULL));
    //Delete old Updatefiles
    deleteOld();
    //Init Application
    m_locale = new wxLocale(m_locale->GetSystemLanguage(), wxLOCALE_LOAD_DEFAULT);
    MainFrame *frame = new MainFrame();
    wxSize size;
    size.x = 800;
    size.y = 500;
    frame->SetMinSize(size);
    frame->SetSize(800, 500);
    frame->Show(true);
    return true;
}
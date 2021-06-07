// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include <iostream>
#include <stdio.h>
#include "define.h"
#include "GUI/mainFrame.h"
#include "Encryptor/rsa_encryption.h"
#include "Database/database.h"
#include "GUI/login.h"
#include "GUI/mailwriter.h"
#include "GUI/register.h"
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
#include <wx/html/htmlwin.h>

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
//Replace some MessageBoxes with Notifications

class MyApp : public wxApp {
    wxLocale m_locale;
public:
    virtual bool OnInit();
    virtual int OnExit();
};

wxIMPLEMENT_APP(MyApp);

void deleteOld(){
    remove("RSA_Client_Old.exe");
    remove("RSA_Client_Old");
}

//Global Vars
static RSA_Encryptor rsa;
static Mail_Database database;
static MyFrame *frame;

bool MyApp::OnInit() {
    //Delete old Updatefiles
    deleteOld();
    //Init Application
    m_locale.Init(m_locale.GetSystemLanguage(), wxLOCALE_LOAD_DEFAULT);
    frame = new MyFrame(&database, &rsa);
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

int MyApp::OnExit() {
    delete &database;
    delete &rsa;
    delete frame;

    return this->wxApp::OnExit();
}
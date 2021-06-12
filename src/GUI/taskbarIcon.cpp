// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include "taskbarIcon.h"

TBIcon::TBIcon(wxFrame* parent){
    this->parent = parent;
    this->SetIcon(wxIcon(icon_xmp), "RSA Mail Client");

    this->Connect(ID_TaskbarIcon_Show, wxEVT_MENU, wxCommandEventHandler(TBIcon::OnShow), NULL, this);
    this->Connect(ID_TaskbarIcon_Close, wxEVT_MENU, wxCommandEventHandler(TBIcon::OnClose), NULL, this);
}

wxMenu *TBIcon::CreatePopupMenu(){
    //Show Menu
    wxMenu* menu = new wxMenu();
    menu->Append(ID_TaskbarIcon_Show, L"Öffnen", L"Öffnet den RSA Mail Client.");
    menu->Append(ID_TaskbarIcon_Close, L"Beenden", L"Beendet den RSA Mail Client.");
    return menu;
}

TBIcon::~TBIcon(){
    this->RemoveIcon();
}

void TBIcon::OnShow(wxCommandEvent& event){
    this->parent->Show(true);
    this->RemoveIcon();
    this->Destroy();
}

void TBIcon::OnClose(wxCommandEvent& event){
    wxCommandEvent evt(wxEVT_MENU, wxID_EXIT);
    wxQueueEvent(parent, evt.Clone());
    this->RemoveIcon();
    this->Destroy();
}
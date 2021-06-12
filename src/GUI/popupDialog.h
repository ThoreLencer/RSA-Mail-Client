// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include <wx/wx.h>
#include <wx/artprov.h>

class PopupDialog : public wxDialog{
public:
    PopupDialog(std::wstring title, std::wstring message, wxWindow* parent);
    void Close();
private:
};
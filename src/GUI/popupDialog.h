#pragma once
#include <wx/wx.h>
#include <wx/artprov.h>

class PopupDialog : public wxDialog{
public:
    PopupDialog(std::wstring title, std::wstring message, wxWindow* parent);
    void Close();
private:
};
#pragma once
#include <wx/wx.h>
#include <wx/taskbar.h>
#include "../Icon.xpm.h"
#include "../define.h"


class TBIcon : public wxTaskBarIcon {
public:
    TBIcon(wxFrame* parent);
    ~TBIcon();
    virtual wxMenu *CreatePopupMenu();
    void OnShow(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
private:
    wxFrame* parent;
};
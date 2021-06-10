#include "popupDialog.h"

PopupDialog::PopupDialog(std::wstring title, std::wstring message, wxWindow* parent):
    wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(300, 100)){
    wxBoxSizer* box1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* box2 = new wxBoxSizer(wxHORIZONTAL);
    //Info Graphic
    wxArtProvider* prov = new wxArtProvider();
    wxBitmap bitmap = prov->GetBitmap(wxART_INFORMATION, wxART_MESSAGE_BOX, wxSize(32, 32));
    wxStaticBitmap* graphic = new wxStaticBitmap(this, wxID_ANY, bitmap);
    box2->Add(graphic, 0, wxALIGN_CENTER, 10);
    wxStaticText* text = new wxStaticText(this, wxID_ANY, message);
    box2->Add(text, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL, 10);
    box1->Add(box2, 1, wxEXPAND);
    SetSizer(box1);
    CenterOnParent();
    Show(true);
}

void PopupDialog::Close(){
    this->Destroy();
}
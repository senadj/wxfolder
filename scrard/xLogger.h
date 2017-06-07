#pragma once
#include <wx/minifram.h>
#include <wx/textctrl.h>

class xLogger : public wxMiniFrame
{
public:
    xLogger(wxWindow* parent, wxString title = "Logger", wxPoint pos = wxDefaultPosition, wxSize size = wxSize(900,100))
        : wxMiniFrame (parent, wxID_ANY, title, pos, size, wxSTAY_ON_TOP|wxSYSTEM_MENU|wxCLOSE_BOX|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxCAPTION)
    {
        wxTextCtrl* tctl = new wxTextCtrl( this , wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH );
        wxLogTextCtrl* lctl = new wxLogTextCtrl(tctl);
        wxLog::SetActiveTarget(lctl);

        wxPoint pp = parent->GetPosition();
        this->SetPosition(wxPoint(pp.x,pp.y+parent->GetSize().GetY()-this->GetSize().GetHeight()));

        tctl->Bind( wxEVT_TEXT, &xLogger::OnLogTextChange, this );
    }

    void OnLogTextChange(wxCommandEvent& event)
    {
        if ( !this->IsShown() )
            this->Show();
    }
};

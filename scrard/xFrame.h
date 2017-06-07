#pragma once
#include <wx/frame.h>

class xPanel;

class xFrame : public wxFrame
{
public:
	xFrame();
	void OnClose(wxCloseEvent&);
	void OnSerialUpdate(wxString& pData);

    xPanel* m_panel;
};

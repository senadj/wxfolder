#pragma once
#include <wx/frame.h>
#include <wx/timer.h>

class xPanel;

class xFrame : public wxFrame
{
    //wxTimer m_timer;
public:
       //wxTimer timer;
      // wxStopWatch sw;

	xFrame();
	void OnClose(wxCloseEvent&);
	void OnSerialUpdate(wxString& pData);
	void OnTimer(wxTimerEvent& event );

    xPanel* m_panel;
};

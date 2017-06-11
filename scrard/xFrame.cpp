#include "xApp.h"
#include "xFrame.h"
#include "xPanel.h"
#include "scrard.xpm"

#include <wx/msgdlg.h>
void xFrame::OnTimer(wxTimerEvent& event ) {
   //wxGetApp().panel->m_textCtrl1->SetLabel(wxString::Format(wxT("%i"),sw.Time()));
   wxMessageBox("timer");
}


xFrame::xFrame() : wxFrame(NULL,wxID_ANY,"ScrArd", wxDefaultPosition, wxSize(1100,850))
{
    m_panel = new xPanel(this);
    SetIcon(wxIcon(scrard_xpm));
    CreateStatusBar( 3, wxST_SIZEGRIP, wxID_ANY );
    int statusWidths[3] = {-3,-5,-2};
    GetStatusBar()->SetStatusWidths(3, statusWidths );
	Bind(wxEVT_CLOSE_WINDOW, &xFrame::OnClose, this);

    //timer.SetOwner( this);
    //this->Connect( wxEVT_TIMER, wxTimerEventHandler( xFrame::OnTimer ) );

    //timer.Start(1);

	//timer.StartOnce(2000);
}


void xFrame::OnClose(wxCloseEvent& WXUNUSED(evt))
{
    wxGetApp().CloseTcpSocket();
    wxGetApp().CloseSerial(); // prevent QueueEvent make app call this frame's members
    Destroy();
}

void xFrame::OnSerialUpdate(wxString& pData)
{
    //m_panel->m_tctl->AppendText(pData);
    this->GetStatusBar()->SetStatusText(pData,0);
    m_panel->OnSerialUpdate(pData);
}

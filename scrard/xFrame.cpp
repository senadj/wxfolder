#include "xApp.h"
#include "xFrame.h"
#include "xPanel.h"

xFrame::xFrame() : wxFrame(NULL,wxID_ANY,"serial frame", wxDefaultPosition, wxSize(1100,800))
{
    m_panel = new xPanel(this);
	Bind(wxEVT_CLOSE_WINDOW, &xFrame::OnClose, this);
}


void xFrame::OnClose(wxCloseEvent&)
{
    wxGetApp().CloseTcpSocket();
    wxGetApp().CloseSerial(); // prevent QueueEvent make app call this frame's members
    Destroy();
}

void xFrame::OnSerialUpdate(wxString& pData)
{
    //m_panel->m_tctl->AppendText(pData);
    m_panel->OnSerialUpdate(pData);
}

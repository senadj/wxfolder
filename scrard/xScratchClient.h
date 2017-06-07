#pragma once
#include <wx/socket.h>

class xSocketHandler;

class xScratchClient : public wxSocketClient
{
    xSocketHandler *m_handler;

public:

    xScratchClient();
    ~xScratchClient();
    void ProcessIncoming(int sockid, const wxString &line);
    // void SensorUpdate(int pin, int val);
    void ScratchUpdate();
    bool Connect();
};

class xSocketHandler : public wxEvtHandler
{
    xScratchClient &m_client;

public:

    xSocketHandler(xScratchClient& pClient);
    void OnSocketEvent(wxSocketEvent& ev);
    void OnSocketConnection();
    void OnSocketLost();
};

#pragma once
#include <wx/app.h>
#include <wx/timer.h>

class xComPort;
class wxBoostSerial;
class xScratchClient;
class xFrame;
class xLogger;

enum pin_state {UNCONFIGURED, DIGITALWRITE, ANALOGWRITE};

WX_DECLARE_STRING_HASH_MAP( int, StringKeyIntValHashMap );
WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash, wxIntegerEqual, IntKeyStringValHashMap );

class xApp : public wxApp
{
public:

    int m_pincnt; // arduino pin count
    bool m_ping;
    wxTimer m_timer;
    xComPort* m_com;
    wxBoostSerial* m_serial;
    xScratchClient* m_tcp;
    xFrame* m_frame;
    xLogger* m_logger;

    StringKeyIntValHashMap m_alias2pin;
    IntKeyStringValHashMap m_pin2alias;
    std::vector<int> m_buff4arduino;   // used for storing pin values for arduino
    std::vector<bool> m_flag4arduino;   // pin change flag vector
    std::vector<int> m_buff4scratch;   // used for storing pin values for scratch
    std::vector<bool> m_flag4scratch;   // pin change flag vector
    std::vector<pin_state> m_writetype4arduino;   // UNCONFIGURED, DIGITALWRITE, ANALOGWRITE (used when updating from scratch)
    wxArrayString m_broadcasts;

    xApp();
    bool ProcessCmdLine (wxChar** argv, int argc, wxString& port);
    bool OnInit();
    int OnExit();
    void OnTimer(wxTimerEvent& event );
    int FilterEvent(wxEvent& evt);
    void CloseSerial();
    void CloseTcpSocket();
    void WriteSerial(wxString pData);
    void ArduinoCommand(int pin, char command);
    void ArduinoPinData(int pin, int val);
    void ArduinoPinsData(std::vector<std::pair<int,int>>& vPinVals);
    void ArduinoBroadcasts();
    void ArduinoPinsUpdate();
    bool IsScratchConnected();
    void AppendInfo(const wxString& pinfo);
    void SetScratchInfo(const wxString& pinfo);
    void SetInfo3(const wxString& pinfo);
};

DECLARE_APP(xApp)

#pragma once

#include <vector>
#include <wx/app.h>
#include <wx/timer.h>
#include <wx/socket.h>
#include "xConfigManager.h"

class xComPort;
class wxBoostSerial;
class xScratchClient;
class xFrame;
class xLogger;

enum pin_state {UNCONFIGURED, DIGITALWRITE, ANALOGWRITE, CUSTOMWRITE};

WX_DECLARE_STRING_HASH_MAP( int, StringKeyIntValHashMap );
WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash, wxIntegerEqual, IntKeyStringValHashMap );
typedef std::pair<int,int>  StdPairType;
WX_DECLARE_HASH_MAP( int, StdPairType, wxIntegerHash, wxIntegerEqual, IntKeyIntPairHashMap );


class xApp : public wxApp
{
public:

    int m_pincnt; // arduino pin count
    bool m_ping;
    wxTimer m_timer;
    xComPort* m_com;
    wxBoostSerial* m_serial;
    xScratchClient* m_tcp;
    xConfigManager* m_cfg_mgr;
    xFrame* m_frame;
    xLogger* m_logger;

    wxString m_base;
    wxString m_basewsep;
    wxString m_inifile;
    wxString m_hidepins;

    StringKeyIntValHashMap m_alias2pin;
    IntKeyStringValHashMap m_pin2alias;
    IntKeyIntPairHashMap m_slider_limits; // slider range from ini file (0-180)
    std::vector<std::pair<int,int>> m_pins_init; // values from ini file,ready for serial bulk send
    std::vector<int> m_buff4arduino;   // used for storing pin values for arduino
    std::vector<bool> m_flag4arduino;   // pin change flag vector
    std::vector<int> m_buff4scratch;   // used for storing pin values for scratch
    std::vector<bool> m_flag4scratch;   // pin change flag vector
    std::vector<pin_state> m_writetype4arduino;   // UNCONFIGURED, DIGITALWRITE, ANALOGWRITE (used when updating from scratch)
    std::vector<wxString> m_inipincfg;
    std::vector<wxString> m_inipincmd;
    std::vector<int> m_inipinval;
    wxArrayString m_broadcasts;

    xApp();
    bool ProcessCmdLine (wxChar** argv, int argc, wxString& hidepins);
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
    void OpenComPort();
    bool IsScratchConnected();
    void SetScratchInfo(const wxString& pinfo);
    void SetInfo3(const wxString& pinfo);
};

DECLARE_APP(xApp)

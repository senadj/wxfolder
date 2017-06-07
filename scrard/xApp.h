#pragma once
#include <wx/app.h>
#include "xScratchClient.h"


typedef wxVector<std::pair<wxString,wxString>> KVType;

class wxBoostSerial;
class xComPort;
class xFrame;
class xLogger;

enum pin_state {UNCONFIGURED, DigitalWRITE, AnalogWRITE};

class xApp : public wxApp
{
public:
    wxStringToStringHashMap m_alias4pin;    // remap pin names
    std::vector<long> m_buff4arduino;   // used for storing pin values for arduino
    std::vector<bool> m_flag4arduino;   // pin change flag vector
    std::vector<long> m_buff4scratch;   // used for storing pin values for scratch
    std::vector<bool> m_flag4scratch;   // pin change flag vector
    std::vector<pin_state> m_writetype4arduino;   // enum UNCONFIGURED, DIGITALWRITE, ANALOGWRITE
    wxArrayString m_broadcasts;

    int m_pincnt; // arduino pin count
    xScratchClient* m_tcp;
    wxBoostSerial* m_serial;
    xComPort* m_com;
    xFrame* m_frame;
    xLogger* m_logger;
    xApp();
    bool OnInit();
    int OnExit();
    int FilterEvent(wxEvent& evt);
    void CloseSerial();
    void CloseTcpSocket();
    void WriteSerial(wxString pData);
    void ArduinoCommand(int pin, char command);
    void ArduinoPinData(int pin, int val);
    void ArduinoPinsData(std::vector<std::pair<int,int>>& vPinVals);
    void ArduinoBroadcasts();
    void ArduinoUpdate();
    bool IsScratchConnected();
    void AppendInfo(const wxString& pinfo);
    void Test();

};

DECLARE_APP(xApp)

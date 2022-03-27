#include "xApp.h"
#include "xFrame.h"
#include "xPanel.h"
#include "wxBoostSerial.h"
#include "xComPort.h"
#include "xLogger.h"
#include "xScratchClient.h"
#include <wx/stdpaths.h>
#include <wx/cmdline.h>

static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
    { wxCMD_LINE_OPTION, "p", "port" , "serial port name" },
    { wxCMD_LINE_OPTION, "b", "baudrate" , "serial port baudrate" },
    { wxCMD_LINE_OPTION, "h", "hide" , "hide pins" },
    { wxCMD_LINE_PARAM, NULL, NULL, "params", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL|wxCMD_LINE_PARAM_MULTIPLE  }, // app.exe 1 2 3
    { wxCMD_LINE_NONE }
};

xApp::xApp() : wxApp(), m_serial(NULL), m_ping(false)
{
    //InitBase();
    m_base = wxFileName( wxStandardPaths::Get().GetExecutablePath() ).GetPath( wxPATH_GET_VOLUME|wxPATH_NO_SEPARATOR );
    m_basewsep = m_base + wxFileName::GetPathSeparator();
    m_inifile = m_basewsep + GetAppName().Append(".ini");

    //inip.ProcessFile();
}

void xApp::ArduinoPinsUpdate()
{
    char buffer [3];
    wxString pinvalhex;
    int msglen = 0;

    for ( size_t i=0; i < m_flag4arduino.size(); i++ )
    {
        if ( m_flag4arduino[i] )
        {
            m_frame->m_panel->SetOutputFromExternal(i,m_buff4arduino[i]);

            snprintf ( buffer, 3, "%02x", i );
            pinvalhex.Append(wxString(buffer));
            //pinvalhex = wxString(buffer);
            snprintf ( buffer, 3, "%02x", m_buff4arduino[i] );
            pinvalhex.Append(wxString(buffer));

            msglen += 4;
            m_flag4arduino[i] = false;
        }
    }

    if ( msglen > 0 )
    {
        snprintf ( buffer, 3, "%02x", msglen );
        pinvalhex.Append(wxString(buffer) + '\n');
        SetInfo3(pinvalhex);
        WriteSerial(pinvalhex);
    }
}


bool xApp::ProcessCmdLine (wxChar** argv, int argc, wxString& hidepins)
{
    wxCmdLineParser parser (argc, argv);
    parser.SetDesc(g_cmdLineDesc);
    parser.SetLogo( "Scratch-Arduino bridge\n");
    parser.AddUsageText("\nUse target serial port name as a parameter\n");

    switch ( parser.Parse(true) ) // "true" izpise Usage() v cmd pri napacnem switchu (tudi frame se odpre)
    {
        case -1: // --help ali /h
           // m_msg = parser.GetUsageString();
           // return false ;  // exit app! help se na mac-u izpise potem, ko kliknes na frame (ki se potem zapre)
            break;
        case 0:
            if ( argc > 1 )
           {
                long pinId;
                if ( parser.Found( "p" , &m_com->m_port) ) {} // wxLogMessage("s: " + port);
                if ( parser.Found( "b" , &m_com->m_baudrate) ) {}
                if ( parser.Found( "h" , &hidepins) ) {}

                for (size_t i=0; i < parser.GetParamCount(); i++)
                {
                    wxArrayString tokens = wxSplit(parser.GetParam(i),':');

                    if ( tokens.GetCount() == 2 )
                    {

                        if ( !tokens.Item(0).Mid(3).Trim().ToLong(&pinId) )
                            continue;

                        if ( pinId > m_pincnt )
                            continue;

                        m_pin2alias[(int)pinId]=tokens.Item(1);
                        m_alias2pin[tokens.Item(1)] = (int)pinId;
                    }
                    else
                    {
                        for (wxArrayString::const_iterator it=tokens.begin(); it!=tokens.end(); ++it)
                        {
                            // to do
                        }
                    }
                }
           }
            break;
        default:
            break;
    }

    return true ;
}

void xApp::OnTimer(wxTimerEvent& event )
{
    if (m_ping)
    {
        this->ArduinoPinsData(m_pins_init);

        if ( !m_tcp->IsOk() )
            m_tcp->Connect();
    }
    else
    {
        ArduinoCommand(255,127);
        m_timer.StartOnce(100);
    }
}

void xApp::OpenComPort()
{
    long baudrate;
    wxString& comport = m_com->m_port;
    wxString& combaudrate = m_com->m_baudrate;

    if ( comport.IsEmpty() )
        comport = m_cfg_mgr->ReadEntry("/port", wxEmptyString);

    if ( combaudrate.IsEmpty() )
        combaudrate = m_cfg_mgr->ReadEntry("/baudrate","115200");

    if ( !combaudrate.ToLong(&baudrate) )
    {
        combaudrate = "115200";
        baudrate = 115200;
    }


    if ( !comport.IsEmpty() )
    {
        wxLogMessage( "Connecting "  + comport + " @" + combaudrate + " ..." );
        m_serial->open(comport, baudrate);
    }
    else if ( m_com->GetBestPort(comport) )
    {
        wxLogMessage( "AutoConnecting Best Port "  + comport + " @" + combaudrate + " ..." );
        m_serial->open(comport, baudrate);
    }
    else
    {
        if (m_com->vPorts.size()>0)
            comport = m_com->vPorts[m_com->vPorts.size()-1].second;
        wxLogMessage( "Blindly AutoConnecting " + comport + " @" + combaudrate + " ..." );
        m_serial->open(comport, baudrate);
    }

    if ( !m_serial->isOpen() )  // wxLogError is called from wxBoostSerial.cpp
    {
        if ( !m_com->vPorts.empty() )
            wxLogMessage( "Suggestions:" );
        for (KVType::const_iterator it=m_com->vPorts.begin(); it != m_com->vPorts.end(); ++it )
            wxLogMessage( (*it).first + " " + (*it).second );
    }

    if ( !m_serial->errorStatus() && m_serial->isOpen() )
        wxLogMessage("Serial Port Connected.");

    m_timer.StartOnce(0);
}

//#include "wx/msgdlg.h"
bool xApp::OnInit ()
{
    m_cfg_mgr = new xConfigManager( m_inifile );

    long pincnt;
    wxString sval = m_cfg_mgr->ReadEntry("/pins","20");
    if(!sval.ToLong(&pincnt)) {}

    m_pincnt = pincnt; // arduino uno pin count
    //m_pincnt = 70; // arduino mega pin count
    m_buff4arduino.resize(m_pincnt);
    m_flag4arduino.resize(m_pincnt);
    m_buff4scratch.resize(m_pincnt);
    m_flag4scratch.resize(m_pincnt);
    m_writetype4arduino.resize(m_pincnt);
    m_inipincfg.resize(m_pincnt);
    m_inipincmd.resize(m_pincnt);
    m_inipinval.resize(m_pincnt);

    // Serial
    m_com = new xComPort();
    wxString hidepins;

    if ( !ProcessCmdLine(argv, argc, hidepins) )
        return false;


    // Process Pins Init Config Entries
    for ( int pin=0; pin < m_pincnt; pin++ )
    {
        wxString pinKey = "/pin" + wxString::Format("%d",pin);
        int pinAlt = pin + 63;
        if ( m_cfg_mgr->HasEntry(pinKey) )
        {
            wxString pinConfigLine = m_cfg_mgr->ReadEntry(pinKey,wxEmptyString);
            if ( !pinConfigLine.IsEmpty() )
            {
                wxArrayString tokens1 = wxSplit(pinConfigLine,';');
                for (wxArrayString::const_iterator it=tokens1.begin(); it!=tokens1.end(); ++it)
                {
                    wxString part1 = *it;
                    wxString part2 = "";
                    part1.Replace(' ',wxEmptyString,true);
                    if ( part1.Find("(") != wxNOT_FOUND ) // for example digitalWrite(HIGH), SERVO(0-180)
                    {
                        wxArrayString tokens2 = wxSplit(part1,'(');
                        part1 = tokens2.Item(0);
                        part2 = tokens2.Item(1);
                        part2.Replace(')',wxEmptyString,true);
                    }

                    if ( part1.IsSameAs("INPUT") ) { m_inipincfg[pin]=part1; m_pins_init.push_back(std::make_pair(pinAlt,'I')); }
                    else if ( part1.IsSameAs("INPUT_PULLUP") ) { m_inipincfg[pin]=part1; m_pins_init.push_back(std::make_pair(pinAlt,'P')); }
                    else if ( part1.IsSameAs("OUTPUT") ) { m_inipincfg[pin]=part1; m_pins_init.push_back(std::make_pair(pinAlt,'O')); }
                    else if ( part1.IsSameAs("SERVO") ) { m_inipincfg[pin]=part1; m_pins_init.push_back(std::make_pair(pinAlt,'S')); }
                    else if ( part1.IsSameAs("CUSTOM") ) { m_inipincfg[pin]=part1; m_pins_init.push_back(std::make_pair(pinAlt,'C')); }
                    else if ( part1.IsSameAs("digitalRead") ) { m_inipincmd[pin]=part1; m_pins_init.push_back(std::make_pair(pinAlt,'D')); }
                    else if ( part1.IsSameAs("analogRead") ) { m_inipincmd[pin]=part1; m_pins_init.push_back(std::make_pair(pinAlt,'A')); }
                    else if ( part1.IsSameAs("digitalWrite") ) { m_inipincmd[pin]=part1; m_pins_init.push_back(std::make_pair(pinAlt,'W')); }
                    else if ( part1.IsSameAs("analogWrite") ) { m_inipincmd[pin]=part1; m_pins_init.push_back(std::make_pair(pinAlt,'U')); }
                    else if ( !m_inipincfg[pin].IsEmpty() && m_inipincmd[pin].IsEmpty() ) m_inipincmd[pin]=part1; // servo.write for example

                    if ( !part2.IsEmpty() )
                    {
                        long val1, val2;

                        if ( part2.IsSameAs("LOW") ) { m_inipinval[pin]=0; m_pins_init.push_back(std::make_pair(pin,0)); }
                        else if ( part2.IsSameAs("HIGH") ) { m_inipinval[pin]=1; m_pins_init.push_back(std::make_pair(pin,1)); }
                        else if ( part2.Find("-") != wxNOT_FOUND )  // SERVO(0-180)
                        {
                            wxArrayString tokens3 = wxSplit(part2,'-');
                            if ( tokens3.GetCount() == 2 )
                            {
                                if ( tokens3.Item(0).ToLong(&val1) && tokens3.Item(1).ToLong(&val2) )
                                    m_slider_limits[pin] = std::make_pair(val1,val2);
                                    //wxLogMessage(part1 + " " + wxString::Format("%d",val1) + ":" + wxString::Format("%d",val2));
                            }
                        }
                        else if ( part2.ToLong(&val1) )
                                if ( val1 >= 0 && val1 <= 255 )
                                {
                                    m_inipinval[pin]=val1;
                                    m_pins_init.push_back(std::make_pair(pin,val1));
                                }
                    }
                }
            }
        }
    }

    m_hidepins = hidepins.IsEmpty() ? m_cfg_mgr->ReadEntry("/hide", wxEmptyString) : hidepins;


    for ( int i=0; i < m_pincnt; i++ )
    {   // pin aliases pin13 -> 13
        m_writetype4arduino[i] = UNCONFIGURED;
        m_buff4arduino[i] = wxINT32_MIN;
        m_buff4scratch[i] = wxINT32_MIN;

        if ( m_pin2alias.find(i) != m_pin2alias.end() )
            continue;

        wxString pinalias = "pin" + wxString::Format("%i",i);
        if ( i>13 && i<20)
            pinalias = "A" + wxString::Format("%i",i-14);
        //else             pinalias = wxString::Format("%i",i);
        m_alias2pin[pinalias] = i;
        m_pin2alias[i] = pinalias;
    }

    m_frame = new xFrame();
    //m_logger = new xLogger(m_frame);

    m_frame->Center();
    m_frame->Show();

    // Serial
    m_serial = new wxBoostSerial();

    m_timer.SetOwner(this);
    Bind( wxEVT_TIMER, &xApp::OnTimer, this );

    // TCP
    wxSocketBase::Initialize();
    m_tcp = new xScratchClient();

    CallAfter(&xApp::OpenComPort);

    return true ;
}


int xApp::FilterEvent(wxEvent& evt)
{
    if ( evt.GetEventType() == wxEVT_THREAD )
    {
        wxString str = wxStaticCast( &evt, wxThreadEvent )->GetString() + '\n';
        m_frame->OnSerialUpdate(str);
        m_tcp->ScratchUpdate(); // checks if tcp->IsOk()
    }
    return -1;
}

void xApp::SetScratchInfo(const wxString& pinfo)
{
    m_frame->GetStatusBar()->SetStatusText(pinfo,1);
}

void xApp::SetInfo3(const wxString& pinfo)
{
    m_frame->GetStatusBar()->SetStatusText(pinfo,2);
}


void xApp::CloseSerial()
{
    if (m_serial && m_serial->isOpen())
        m_serial->close();
}

void xApp::CloseTcpSocket()
{
    if ( m_tcp->IsConnected() )
        m_tcp->Close();
}

void xApp::WriteSerial(wxString pData)
{
    if (m_serial)
        m_serial->write(pData);
}

bool xApp::IsScratchConnected()
{
    return m_tcp->IsOk();
}

void xApp::ArduinoCommand( int pPin, char pCmd )
{
    char buffer [3];
    snprintf ( buffer, 3, "%02x", pPin );
    WriteSerial(wxString(buffer) + pCmd + "03\n"); // 03 = message length
}

void xApp::ArduinoPinData(int pin, int val)
{
    char buffer [3];
    wxString pinvalhex;

    snprintf ( buffer, 3, "%02x", pin );
    pinvalhex = wxString(buffer);
    snprintf ( buffer, 3, "%02x", val );
    pinvalhex.Append(wxString(buffer));

    WriteSerial(pinvalhex + "04\n");
}

void xApp::ArduinoPinsData( std::vector<std::pair<int,int>>& vPinVals )
{
    char buffer [3];
    wxString pinvalhex = wxEmptyString;
    int msglen = 0;

    for (std::vector<std::pair<int,int>>::const_iterator it = vPinVals.begin(); it!=vPinVals.end(); ++it)
    {
        snprintf ( buffer, 3, "%02x", (*it).first );
        pinvalhex.Append(wxString(buffer));
        snprintf ( buffer, 3, "%02x", (*it).second );
        pinvalhex.Append(wxString(buffer));

        msglen += 4;
    }

    if ( msglen > 0 )
    {
        snprintf ( buffer, 3, "%02x", msglen );
        pinvalhex.Append(wxString(buffer) + '\n');
        WriteSerial(pinvalhex);
    }
}

void xApp::ArduinoBroadcasts()
{
    long pinId;

    if (m_broadcasts.IsEmpty())
        return;

    for (int j=0; j < m_broadcasts.GetCount(); j++)
    {
        if ( m_broadcasts[j].length() > 10 )
        {
            if ( m_broadcasts[j].Mid(0,10).IsSameAs("analogRead") )
            {
                wxString sensorId = m_broadcasts[j].Mid(11);
                sensorId.Replace("(","");
                sensorId.Replace(")","");

                if ( sensorId.ToLong(&pinId) )
                {
                    if ( pinId < 0 ) // unsubscribe
                    {
                        //wxLogMessage("UnSubscribe: " + wxString::Format("%i",pinId));
                        //ArduinoCommand(-pinId,'a');
                        //m_pinfw[-pinId] = false;
                    }
                    else
                    {
                        m_frame->m_panel->SetInputFromExternal(pinId,'A');

                        //wxLogMessage("Subscribe ANALOG: " + wxString::Format("%i",pinId));
                        //ArduinoCommand(pinId,'A');
                        //m_pinfw[pinId] = true;
                    }
                }
            }
            else if ( m_broadcasts[j].Mid(0,11).IsSameAs("digitalRead") )
            {
                wxString sensorId = m_broadcasts[j].Mid(12);
                sensorId.Replace("(","");
                sensorId.Replace(")","");

                if ( sensorId.ToLong(&pinId) )
                {
                    if ( pinId < 0 ) // unsubscribe
                    {
                        //wxLogMessage("UnSubscribe: " + wxString::Format("%i",pinId));
                        //ArduinoCommand(-pinId,'d');
                        //m_pinfw[-pinId] = false;
                    }
                    else
                    {
                        m_frame->m_panel->SetInputFromExternal(pinId,'D');
                        //wxLogMessage("Subscribe DIGITAL: " + wxString::Format("%i",pinId));
                        //ArduinoCommand(pinId,'D');
                        //m_pinfw[pinId] = true;
                    }
                }
            }
        }
    }
    m_broadcasts.Empty();
}

int xApp::OnExit()
{
    wxDELETE(m_serial);
    wxDELETE(m_tcp);
    wxDELETE(m_com);
    wxDELETE(m_cfg_mgr);
    return 0;
}


IMPLEMENT_APP(xApp)


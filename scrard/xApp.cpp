#include "xApp.h"
#include "xFrame.h"
#include "xPanel.h"
#include "wxBoostSerial.h"
#include "xComPort.h"
#include "xLogger.h"
#include "xScratchClient.h"
#include <wx/cmdline.h>

static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
    { wxCMD_LINE_OPTION, "s", "serial" , "serial port name" },
    { wxCMD_LINE_PARAM, NULL, NULL, "params", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL|wxCMD_LINE_PARAM_MULTIPLE  }, // app.exe 1 2 3
    { wxCMD_LINE_NONE }
};

xApp::xApp() : wxApp(), m_serial(NULL){}

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


bool xApp::ProcessCmdLine (wxChar** argv, int argc, wxString& port)
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
                if ( parser.Found( "s" , &port) ) {} // wxLogMessage("s: " + port);

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


bool xApp::OnInit ()
{
    m_pincnt = 20; // arduino uno pin count
    //m_pincnt = 70; // arduino mega pin count
    m_buff4arduino.resize(m_pincnt);
    m_flag4arduino.resize(m_pincnt);
    m_buff4scratch.resize(m_pincnt);
    m_flag4scratch.resize(m_pincnt);
    m_writetype4arduino.resize(m_pincnt);

    // Serial
    m_com = new xComPort();
    wxString comport;

   if (!ProcessCmdLine(argv, argc, comport))
        return false;
    else
        m_com->m_cmdport = comport;



    for ( int i=0; i < m_pincnt; i++ )
    {   // pin aliases pin13 -> 13
        m_writetype4arduino[i] = UNCONFIGURED;
        m_buff4arduino[i] = wxINT32_MIN;
        m_buff4scratch[i] = wxINT32_MIN;

        if ( m_pin2alias.find(i) != m_pin2alias.end() )
            continue;

        wxString pinalias = "pin"+wxString::Format("%i",i);
        m_alias2pin[pinalias] = i;
        m_pin2alias[i] = pinalias;
    }

    //m_alias4pin["a2"]="4";

    m_frame = new xFrame();
    m_logger = new xLogger(m_frame);

    for ( int i=0;i<m_pincnt;i++ )
        m_frame->m_panel->SetAlias(i);

    m_frame->Center();
    m_frame->Show();

    if ( !comport.IsEmpty() )
    {
        AppendInfo( "Connecting "  + comport + " ..." );
        m_serial = new wxBoostSerial(comport,115200);
    }
    else if ( m_com->GetBestPort(comport) )
    {
        AppendInfo( "AutoConnecting Best Port"  + comport + " ..." );
        m_serial = new wxBoostSerial(comport,115200);
    }
    else
    {
        if (m_com->vPorts.size()>0)
            comport = m_com->vPorts[m_com->vPorts.size()-1].second;
        AppendInfo( "Blindly AutoConnecting " + comport +" ..." );
        m_serial = new wxBoostSerial(comport,115200);
    }

    if ( !m_serial->isOpen() )
    {
        AppendInfo( "Serial (Arduino) connection failed." );
        for (KVType::const_iterator it=m_com->vPorts.begin(); it != m_com->vPorts.end(); ++it )
        {
            AppendInfo( (*it).first + " " + (*it).second );
        }
    }

    if ( !m_serial->errorStatus() && m_serial->isOpen() )
    {
        AppendInfo( "Connected.\n" );
    }

    // TCP
    m_tcp = new xScratchClient();
    m_tcp->Connect();



    return true ;
}


int xApp::FilterEvent(wxEvent& evt)
{
    if ( evt.GetEventType() == wxEVT_THREAD )
    {
        wxString str = wxStaticCast( &evt, wxThreadEvent )->GetString() + '\n';
        m_frame->OnSerialUpdate(str);
        m_tcp->ScratchUpdate();
    }
    return -1;
}


void xApp::AppendInfo(const wxString& pinfo)
{
    m_frame->m_panel->m_tctl->AppendText(pinfo+'\n');
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
    wxString pinvalhex;
    int msglen = 0;

    for (std::vector<std::pair<int,int>>::const_iterator it = vPinVals.begin(); it!=vPinVals.end(); ++it)
    {
        snprintf ( buffer, 3, "%02x", (*it).first );
        pinvalhex = wxString(buffer);
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
    return 0;
}


IMPLEMENT_APP(xApp)


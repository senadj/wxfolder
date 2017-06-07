#include "xApp.h"
#include "xFrame.h"
#include "xPanel.h"
#include "wxBoostSerial.h"
#include "xComPort.h"
#include "xLogger.h"



xApp::xApp() : wxApp()
{
    m_serial = NULL;
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

void xApp::Test()
{
    /*
    int pinId = 11;

    m_frame->m_panel->SetOutputFromExternal(6,-1);
    m_frame->m_panel->SetOutputFromExternal(pinId,55);

    m_frame->m_panel->SetOutputFromExternal(12,0);
    m_frame->m_panel->SetOutputFromExternal(13,1);
    */
}


bool xApp::IsScratchConnected()
{
    return m_tcp->IsOk();
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

void xApp::ArduinoUpdate()
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
            pinvalhex = wxString(buffer);
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
        WriteSerial(pinvalhex);
    }
}

int xApp::OnExit()
{
    wxDELETE(m_serial);
    wxDELETE(m_tcp);
    wxDELETE(m_com);
    return 0;
}




bool xApp::OnInit ()
{
    m_pincnt = 20; // arduino pin count
    m_buff4arduino.resize(m_pincnt);
    m_flag4arduino.resize(m_pincnt);
    m_buff4scratch.resize(m_pincnt);
    m_flag4scratch.resize(m_pincnt);
    m_writetype4arduino.resize(m_pincnt);

    for (int i=0;i<m_pincnt;i++)
    {   // pin aliases pin13 -> 13
        wxString pinalias = "pin"+wxString::Format("%i",i);
        m_alias4pin[pinalias] = wxString::Format("%i",i);
        m_writetype4arduino[i] = UNCONFIGURED;
        m_buff4arduino[i] = wxINT32_MIN;
        m_buff4scratch[i] = wxINT32_MIN;
    }

    m_frame = new xFrame();
    m_frame->Center();
    //m_frame->Maximize();
    m_frame->Show();

    m_logger = new xLogger(m_frame);
    //m_logger->Show();

    // Serial
    m_com = new xComPort();

    wxString com;

     for (KVType::const_iterator it=m_com->vPorts.begin(); it != m_com->vPorts.end(); ++it )
     {
        AppendInfo( (*it).first + " " + (*it).second );
     }

     wxString comport;

     if ( m_com->GetBestPort(comport) )
     {
         com = "AutoConnecting "  + comport + " ...";
         AppendInfo( com );
         m_serial = new wxBoostSerial(comport,115200);
     }
     else
     {
          m_serial = new wxBoostSerial("COM5",115200);
          if ( !m_serial->isOpen() )
            wxLogError("serial not open");

          if ( m_serial->errorStatus() )
            wxLogError("errorStatus true");
     }

    if ( !m_serial->errorStatus() && m_serial->isOpen() )
    {
        com = "Connected.\r\n";
        AppendInfo( com );
    }

    // TCP
    m_tcp = new xScratchClient();
    m_tcp->Connect();

    //for (int i=0; i<10; i++)
    //    m_tcp->SensorUpdate(i,i+100);

     //ArduinoCommand(0,0);
     return true ;
}

void xApp::AppendInfo(const wxString& pinfo)
{
    m_frame->m_panel->m_tctl->AppendText(pinfo+'\n');
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


IMPLEMENT_APP(xApp)


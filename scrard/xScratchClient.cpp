#include "xApp.h"
#include "xScratchClient.h"
#include <wx/log.h>

xScratchClient::xScratchClient() : wxSocketClient() { m_handler = new xSocketHandler(*this); }
xScratchClient::~xScratchClient() { wxDELETE(m_handler); }

bool xScratchClient::Connect()
{
    wxIPV4address addr;
    addr.Hostname("127.0.0.1");
    addr.Service("42001");
    wxSocketClient::Connect(addr);
}

void UpdateArduinoBuffer(wxString& pSensorId, wxString pValue)
{
    int sensorId;
    long value;
    wxString mappedPinName;

    // find pin number from alias
    StringKeyIntValHashMap::const_iterator it = wxGetApp().m_alias2pin.find(pSensorId);

    if ( it==wxGetApp().m_alias2pin.end() )
        return;

    sensorId = (*it).second;

    if ( pValue.Trim().ToLong(&value) ) // scratch value to number
    {
        // set values intended for arduino
        if ( wxGetApp().m_buff4arduino[sensorId] != value && value >= 0 && value < 256 )   // conditionally set pin values
        {
            wxGetApp().m_buff4arduino[sensorId] = (int)value;
            wxGetApp().m_flag4arduino[sensorId] = true;
            //wxLogMessage("new val set: " + wxString::Format("%i",value));
        }
    }
}

void xScratchClient::ProcessIncoming(int sockid, const wxString &line)
{
    wxArrayString tokens = wxSplit(line, '"');
    bool sensorUpdate = false;
    bool broadcast = false;
    wxString  sensor;

    // wxLogMessage(line);
    wxGetApp().SetScratchInfo(line);

    for ( wxArrayString::iterator it=tokens.begin(); it!=tokens.end(); ++it )
    {
        const wxString& tok = *it;

        if ( broadcast && !tok.IsEmpty() )
        {
            //wxLogMessage("broadcast: " + tok);
            wxGetApp().m_broadcasts.Add(tok);
            broadcast=false;
        }
        else if (sensorUpdate)
        {
            if ( sensor.IsEmpty() )
                sensor=tok;
            else
            {
                if ( tok.length()>16 && tok.Mid(tok.length()-15).IsSameAs(" sensor-update ") )
                {
                    UpdateArduinoBuffer(sensor,tok.Mid(0,tok.length()-15));
                }
                else if ( tok.length()>9 && tok.Mid(tok.length()-10).IsSameAs("broadcast ") )
                {
                    //wxLogMessage("sensor-update: " + sensor + ' ' + tok.Mid(0,tok.length()-10));
                    UpdateArduinoBuffer(sensor,tok.Mid(0,tok.length()-10));
                    sensorUpdate=false;
                    broadcast=true;
                }
                else
                {
                    UpdateArduinoBuffer(sensor,tok);
                }

                sensor.Empty();
            }
        }

        if ( !sensorUpdate && tok.IsSameAs("sensor-update ") )
            sensorUpdate=true;
        else if ( !broadcast && tok.IsSameAs("broadcast ") )
        {
            broadcast=true;
            sensorUpdate=false;
        }
    }

    wxGetApp().ArduinoBroadcasts();
    wxGetApp().ArduinoUpdate();   // send pin changes to serial
}

/*
void xScratchClient::SensorUpdate(int pin, int val)
{
    wxString msg = "sensor-update pin" + wxString::Format("%i",pin) + ' ' + wxString::Format("%i",val);

    wxCharBuffer buff = msg.mb_str(wxConvLocal);
    size_t buffsz = strlen(buff.data());

    unsigned char cb[1024];
    cb[2] = cb[1] = cb[0] = 0;
    cb[3]=buffsz;
    memcpy(&(cb[4]), buff, buffsz);

    wxSocketBase::Write(cb,buffsz+4);
}
*/

void xScratchClient::ScratchUpdate()
{
    wxString msg = "sensor-update";

    if ( !this->IsOk() )
        return;

    for ( size_t i=0; i < wxGetApp().m_flag4scratch.size(); i++ )
    {
        if ( wxGetApp().m_flag4scratch[i] )
        {
            //msg.Append(" pin" + wxString::Format("%i",i) + ' ' + wxString::Format("%i",wxGetApp().m_buff4scratch[i]));
            msg.Append( ' ' + wxGetApp().m_pin2alias[i] + ' ' + wxString::Format("%i",wxGetApp().m_buff4scratch[i]) );
            wxGetApp().m_flag4scratch[i] = false;
        }
    }

    wxCharBuffer buff = msg.mb_str(wxConvLocal);
    size_t buffsz = strlen(buff.data());

    unsigned char cb[1024];
    cb[2] = cb[1] = cb[0] = 0;
    cb[3]=buffsz;
    memcpy(&(cb[4]), buff, buffsz);

    if ( msg.length() > 13 )
        wxSocketBase::Write(cb,buffsz+4);
}

// HANDLER

xSocketHandler::xSocketHandler(xScratchClient& pClient) : m_client(pClient)
{
    Bind(wxEVT_SOCKET,&xSocketHandler::OnSocketEvent,this);

    m_client.SetEventHandler(*this,1);
    m_client.SetNotify( wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG | wxSOCKET_CONNECTION_FLAG );
    m_client.Notify(true);
}


void xSocketHandler::OnSocketEvent(wxSocketEvent& ev)
{
    switch ( ev.GetSocketEvent() )
    {
        case wxSOCKET_INPUT:
        {
            wxString out = wxEmptyString;

            int i = 1;
            int cksm = 0;
            int msgLen = 0;

            char buf[1024];
            m_client.Read(buf, 1024);

            for ( size_t j = 0; j < m_client.LastReadCount(); j++ )
            {
                if ( (i<4) && ( buf[j] > 0 ) ){ i=0; cksm=-buf[j]; }
                else if ( cksm == 6 ) { msgLen=buf[j]+4; /*msg=wxEmptyString;*/ }
                else
                {   //msg.Append(buf[j]);
                    if ( i == msgLen )
                    {
                        i=0; cksm=-buf[j];
                        out.Append(wxString::FromUTF8( &(buf[j-msgLen+5]), msgLen-4) );
                    }
                }
                cksm = cksm + i + buf[j];
                i++;
            }

            if ( !out.IsEmpty() )
                m_client.ProcessIncoming(ev.GetId(), out);

            break;
        }

        case wxSOCKET_CONNECTION :
            OnSocketConnection(); break;

        case wxSOCKET_LOST:
            OnSocketLost(); break;
    }
}

void xSocketHandler::OnSocketLost()
{
    wxLogMessage("TCP Connection Lost");
}


void xSocketHandler::OnSocketConnection()
{
    wxGetApp().ArduinoCommand(0,0); // make arduino flush buffer
    wxGetApp().AppendInfo("Scratch connected.");
/*
    wxString msg = "sensor-update p28 288 \"p29 kk\" 299 zz 22";

    wxCharBuffer buff = msg.mb_str(wxConvLocal);
    size_t buffsz = strlen(buff.data());

    unsigned char cb[1024];
    cb[2] = cb[1] = cb[0] = 0;
    cb[3]=buffsz;
    memcpy(&(cb[4]), buff, buffsz);

  const char* data = "sensor-update \"zilt\" -11";

  unsigned char ab[1024];
  ab[2] = ab[1] = ab[0] = 0;
  ab[3]=strlen(data);
  memcpy(&(ab[4]),data,strlen(data));
  m_hndl.m_client.Write(ab,strlen(data)+4);

    m_client.Write(cb,buffsz+4);
*/
}

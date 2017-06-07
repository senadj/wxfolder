#include "xComPort.h"

xComPort::xComPort()
#if defined __WINDOWS__
    : key(wxRegKey::HKLM, "Hardware\\DeviceMap\\SerialComm")
#endif
{
    ScanPorts();
}

bool xComPort::GetBestPort(wxString& portname)
{
    if ( !vBestPortIdx.empty() )
    {
        portname = vPorts[vBestPortIdx.back()].second;
        return true;
    }
    return false;
}

void xComPort::ScanPorts()
{
    wxString val_name;
    wxString val_data;
    long idx;

    vPorts.empty();
    vBestPortIdx.empty();

#if defined __WINDOWS__
    if ( key.Exists() )
        if ( key.Open( wxRegKey::Read ) )
        {
            if ( key.HasValues() )
            {
                 if ( key.GetFirstValue(val_name, idx) )
                 {
                     do
                     {
                          key.QueryValue(val_name, val_data);

                          if ( val_name.Contains("USBSER") )
                            vBestPortIdx.push_back(vPorts.size());

                          vPorts.push_back(std::make_pair(val_name,val_data));
                     }
                     while ( key.GetNextValue(val_name, idx) );
                 }
            }
            key.Close();
        }
        else
            wxLogError("xComPort: can't open RegKey.");
    else
        wxLogError("xComPort: non existent registry key.");
#else
  vPorts.push_back(std::make_pair("/dev/ttyACM0","/dev/ttyACM0"));
#endif
}

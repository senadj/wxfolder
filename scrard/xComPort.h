#pragma once
#include <vector>
#include <wx/string.h>
#if defined __WINDOWS__
#include <wx/msw/registry.h>
#endif

typedef std::vector<std::pair<wxString,wxString>> KVType;

class xComPort
{
#if defined __WINDOWS__
    wxRegKey key;
#endif
public:

    wxString m_port;
    wxString m_baudrate;

	KVType vPorts;
	std::vector<size_t > vBestPortIdx;

	xComPort();
	void ScanPorts();
	bool GetBestPort(wxString& portname);

};

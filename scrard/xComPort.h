#pragma once
#include "xApp.h"
//#include <wx/utils.h>
#if defined __WINDOWS__
#include <wx/msw/registry.h>
#endif

class xComPort
{
#if defined __WINDOWS__
    wxRegKey key;
#endif
public:

	KVType vPorts;
	wxVector<size_t > vBestPortIdx;

	xComPort();
	void ScanPorts();
	bool GetBestPort(wxString& portname);

};

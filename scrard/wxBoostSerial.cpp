#include "wxBoostSerial.h"
#include "AsyncSerial.h"
#include <boost/algorithm/string.hpp>
#include <wx/app.h>
#include <vector>

/**
 * Implementation details of wxBoostSerial class.
 */
class wxBoostSerialImpl
{
public:
    CallbackAsyncSerial serial;
    std::string strdata;
};

wxBoostSerial::wxBoostSerial() : pimpl(new wxBoostSerialImpl)
{
}

wxBoostSerial::wxBoostSerial(wxString devname, unsigned int baudrate)
        : pimpl(new wxBoostSerialImpl)
{
    open(devname,baudrate);
}

void wxBoostSerial::open(wxString devname, unsigned int baudrate)
{
    try {
        pimpl->serial.open(devname.ToStdString(),baudrate);
    } catch(boost::system::system_error&)
    {
        //Errors during open
    }
    pimpl->serial.setCallback(bind(&wxBoostSerial::readCallback,this, _1, _2));
}

void wxBoostSerial::close()
{
    pimpl->serial.clearCallback();
    try {
        pimpl->serial.close();
    } catch(boost::system::system_error&)
    {
        //Errors during port close
    }
    pimpl->strdata.clear();
}

bool wxBoostSerial::isOpen()
{
    return pimpl->serial.isOpen();
}

bool wxBoostSerial::errorStatus()
{
    return pimpl->serial.errorStatus();
}

void wxBoostSerial::write(wxString data)
{
    pimpl->serial.writeString(data.ToStdString());
}

wxBoostSerial::~wxBoostSerial()
{
    pimpl->serial.clearCallback();
    try {
        pimpl->serial.close();
    } catch(...)
    {
        //Don't throw from a destructor
    }
}

void wxBoostSerial::readCallback(const char *data, size_t size)
{
	pimpl->strdata.append(data, size);
	std::vector<std::string> fields;

	if ( pimpl->strdata.find('\n') != std::string::npos )
    {
        // http://stackoverflow.com/questions/7930796/boosttokenizer-vs-boostsplit
        boost::split(fields, pimpl->strdata, boost::is_from_range('\n','\n'));
        int numLines = fields.size()-1;
        pimpl->strdata = fields.at(numLines);

        for( int i=0; i<numLines; i++ )
        {
            wxThreadEvent myevt;
            myevt.SetString(wxString(fields.at(i)));
            wxTheApp->QueueEvent(myevt.Clone());
        }
    }
}

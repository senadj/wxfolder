#pragma once
#include <wx/string.h>
#include <boost/shared_ptr.hpp>

class wxBoostSerialImpl;

class wxBoostSerial
{
public:
    /**
     * Default constructor
     */
    wxBoostSerial();

    /**
     * Constructor. Opens a serial port
     * \param devname port name, like "/dev/ttyUSB0" or "COM4"
     * \param baudrate port baud rate, example 115200
     * Format is 8N1, flow control is disabled.
     */
    wxBoostSerial(wxString devname, unsigned int baudrate);

    /**
     * Opens a serial port
     * \param devname port name, like "/dev/ttyUSB0" or "COM4"
     * \param baudrate port baud rate, example 115200
     * Format is 8N1, flow control is disabled.
     */
    void open(wxString devname, unsigned int baudrate);

    /**
     * Closes a serial port.
     */
    void close();

    /**
     * \return true if the port is open
     */
    bool isOpen();

    /**
     * \return true if any error
     */
    bool errorStatus();

    /**
     * Write a string to the serial port
     */
    void write(wxString data);

    /**
     * Destructor
     */
    ~wxBoostSerial();

private:
    /**
     * Called when data is received
     */
    void readCallback(const char *data, size_t size);

    boost::shared_ptr<wxBoostSerialImpl> pimpl; ///< Pimpl idiom
};

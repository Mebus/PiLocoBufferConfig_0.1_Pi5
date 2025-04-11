//////////////////////////////////////////////////////////////////////////////
// File name:       SerialPort.cpp
// $Id: SerialPort.cpp 1228 2023-02-15 10:20:50Z pischky $
//
// Class/Module(s): CSerialPort
//
// Description: Implementation of Class/Module(s). For further documentation
//              see description in header file.
//
// 050827  SBor  Added CTS flow control for transmit stream
// 2006-11-22    Modified for linux, Ian Cunningham
// 2006-12-02    Replaced port index by device string
// 2016-05-25    Support devices without ioctl(TIOCGSERIAL) Martin Pischky/Knut Schwichtenberg
//
//////////////////////////////////////////////////////////////////////////////

#include <errno.h>        // errno
#include <string.h>       // strncpy(), strerror(), ...
#include <stdlib.h>       // abort()
#include <unistd.h>       // close()
#include <fcntl.h>        // open(), O_RDWR, O_NOCTTY
#include <termios.h>      // cfsetispeed(), cfsetospeed(), ...
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>    // ioctl()
#include <linux/serial.h>
#include "log.h"             // LOG_ERR, LOG_WARNING
#include "DisplayFrontend.h" // extern Display
#include "SerialPort.h"

/*** STATIC DATA ***/

static struct termios       oldSettings;
static struct termios       newSettings;
static struct serial_struct oldSerialInfo;
static bool                 oldSerialInfoSet = false;

/**
 * Shutdown using signal SIGABRT. Used on unknown exceptions.
 * Note: we use macro to preserve __func__ and __LINE__.
 */
#define die() \
        do {                                                     \
            log_print( LOG_ERR, "abort at %s: %s: %d", __FILE__, \
                       __func__, __LINE__ );                     \
            abort();                                             \
        } while(0)

//////////////////////////////////////////////////////////////////////////////
// CSerialPort
//
// Description:	Standard constructor.
//
CSerialPort::CSerialPort(unsigned int uiTxBufferSize, unsigned int uiRxBufferSize)
:	m_enDataBits(DATA_BITS_8_E), m_enStopBits(STOP_BITS_1_E),
	m_enParity(NO_PARITY_E), m_enFlow(FLOW_CONTROL_NONE_E), m_iBaud(57600)
{
	m_uiTxBufferSize = uiTxBufferSize;
	m_uiRxBufferSize = uiRxBufferSize;

	m_hComm = -1;
}


//////////////////////////////////////////////////////////////////////////////
// ~CSerialPort
//
// Description:	Standard destructor.
//
CSerialPort::~CSerialPort()
{
	if (GetOpenStatus()) Close();
}


////////////////////////////////////////////////////////////////////////////
// CSerialPort::Open
//
// Description: Open the serial port. In case of data reception on the
//              serial port either the windows message mechanism or a callback
//              function mechanism can be used for application notification.
//
// Return:  bool  -  true if successful, otherwise false
//
// Parameter:  EPort Port          -  Port number (PORT_1_E, PORT_2_E, PORT_3_E, PORT_4_E).
//             int iBaud           -  Baud rate (e.g. 4800, 19200, ...).
//             EDataBits DataBits  -  Data bits (DATA_BITS_5_E, DATA_BITS_6_E, DATA_BITS_7_E, DATA_BITS_8_E = 8).
//             EStopBits StopBits  -  Stop bits (STOP_BITS_1_E, STOP_BITS_1P5_E, STOP_BITS_2_E).
//             EParity Parity      -  Parity (NO_PARITY_E, ODD_PARITY_E, EVEN_PARITY_E, MARK_PARITY_E, SPACE_PARITY_E).
//             EFlow Flow          -  Flow control (FLOW_CONTROL_NONE_E).
//
bool CSerialPort::Open(const char *pcPort, int iBaud, EDataBits DataBits,
                       EStopBits StopBits, EParity Parity, EFlow Flow)
{
	try
	{
		// Check if already opened:
		if(m_hComm >= 0) return false;
		oldSerialInfoSet = false;

		strncpy(m_acPort, pcPort, sizeof(m_acPort));
		m_acPort[sizeof(m_acPort)-1] = 0;  // make sure destination string is null terminated
		m_iBaud = iBaud;
		m_enDataBits = DataBits;
		m_enStopBits = StopBits;
		m_enParity = Parity;
		m_enFlow = Flow;
		
		// Open serial device for read/write and no console capability:
		m_hComm = open(pcPort, O_RDWR | O_NOCTTY);
		if (m_hComm < 0) { // m_hComm == -1
			Display.ErrorMessageF( LOG_ERR, "CSerialPort::Open() failed on %s. "
			                       "(errno=%i: %s)", pcPort, errno,
			                       strerror(errno) );
			return false;
		}

		// Get present configuration so that we can fully restore port when done
		oldSerialInfo.reserved_char[0] = 0;
		int retCode = ioctl(m_hComm, TIOCGSERIAL, &oldSerialInfo);
		if (retCode < 0) { // retCode == -1
			// ioctl(TIOCGSERIAL) failed
			if (errno == ENOTTY) {
				// Driver does not support ioctl(TOCGSERIAL) (e.g.ch341) 
				oldSerialInfoSet = false;
			} else {
				// Other reason: report and terminate
				Display.ErrorMessageF( LOG_ERR, "CSerialPort::Open() ioctl failed on "
				                       "%s. (errno=%i: %s)", pcPort, errno,
				                       strerror(errno) );
				close(m_hComm);
				m_hComm = -1;
				return false;
			}
		} else {
			// ioctl(TIOCGSERIAL) success
			oldSerialInfoSet = true;
		}

		tcgetattr(m_hComm, &oldSettings);
		newSettings = oldSettings;

		// raw 8-bit use:
		cfmakeraw(&newSettings);

		// non-canonical input settings:
		newSettings.c_cc[VMIN]  = 1;       // min characters to read, blocks if > 0
		newSettings.c_cc[VTIME] = 0;       // timeout in deciseconds

		// baud rate:
		switch (m_iBaud) {
			case 19200L:
				cfsetospeed(&newSettings, B19200);
				cfsetispeed(&newSettings, B19200);
				break;
			case 38400L:
				cfsetospeed(&newSettings, B38400);
				cfsetispeed(&newSettings, B38400);
				break;
			case 57600L:
				cfsetospeed(&newSettings, B57600);
				cfsetispeed(&newSettings, B57600);
				break;
			case 115200L:
				cfsetospeed(&newSettings, B115200);
				cfsetispeed(&newSettings, B115200);
				break;
			default:
				Display.ErrorMessageF( LOG_ERR, "CSerialPort::Open() requested "
				                       "baud rate %d unsupported", m_iBaud );
				close(m_hComm);
				m_hComm = -1;
				return false;
		}

		// Set hardware flow control on/off:
		switch(m_enFlow) {
			case FLOW_CONTROL_NONE_E:	// disable flow control
				newSettings.c_iflag &= ~CRTSCTS;
				newSettings.c_oflag &= ~CRTSCTS;
				break;
			case FLOW_CONTROL_CTS_E:	// enable flow control
				newSettings.c_iflag |= CRTSCTS;
				newSettings.c_oflag |= CRTSCTS;
				break;
			default:
				Display.ErrorMessageF( LOG_ERR, "CSerialPort::Open() requested "
				                       "illegal flow control value");
				close(m_hComm);
				m_hComm = -1;
				return false;
		}

		// Set port configuration
		if (tcsetattr(m_hComm, TCSANOW, &newSettings)) {
			Display.ErrorMessageF( LOG_ERR, "CSerialPort::Open() tcsetattr failed "
			                       "(errno=%i: %s)", errno, strerror(errno) );
			close(m_hComm);
			m_hComm = -1;
			return false;
		}

		return true;
	}
	catch(...)
	{
		log_print_and_stderr( LOG_ERR, "CSerialPort::Open() unknown exception" );
		die();
		return false;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Close
//
// Description: Closes the opened port. Can be called directly, but is also
//              called automatically when the CSerialPort object is destructed.
//
// Return:      bool -  true if successful, otherwise false.
//
// Parameter:   void
//
bool CSerialPort::Close(void)
{
	try
	{
		// return if already closed - return true or false?
		if (m_hComm < 0) return false;

		// restore attributes
		if (tcsetattr(m_hComm, TCSANOW, &oldSettings) < 0) {
			Display.ErrorMessageF( LOG_ERR, "CSerialPort::Close() tcsetattr failed "
			                       "(errno=%i: %s)", errno, strerror(errno) );
			close(m_hComm);
			m_hComm = -1;
			return false;
		}

		if (oldSerialInfoSet) {
			// restore port settings
			if (ioctl(m_hComm, TIOCSSERIAL, &oldSerialInfo) < 0) {
				Display.ErrorMessageF( LOG_ERR, "CSerialPort::Close() ioctl failed "
				                       "(errno=%i: %s)", errno, strerror(errno) );
				close(m_hComm);
				m_hComm = -1;
				return false;
			}
		}

		close(m_hComm);
		m_hComm = -1;

		return true;
	}
	catch(...)
	{
		log_print_and_stderr( LOG_ERR, "CSerialPort::Close() unknown exception" );
		die();
		return false;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Reset
//
// Description: Reset port. Rx and Tx buffers are flushed
//
// Return:      bool  -  true if successful, otherwise false
//
// Parameter:   void
//
bool CSerialPort::Reset(void)
{
	try
	{
		// return if not open
		if(m_hComm < 0) return false;

		// flush both input and output buffers
		if (!tcflush(m_hComm, TCIOFLUSH)) return false; //TODO: on rc=-1 log errno
		// if(tcflush(m_hComm, TCIOFLUSH) == -1) {
		//     log_print_and_stderr(...)
		//     return false;
		// }

		return true;
	}
	catch(...)
	{
		log_print_and_stderr( LOG_ERR, "CSerialPort::Reset() unknown exception" );
		die();
		return false;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Read
//
// Description:
//
// Return:      bool  -  true if successful, otherwise false.
//
// Parameter:   unsigned char *ucBuffer
//              unsigned int uiNumOfBytes
//              unsigned int &uiNumOfBytesRead
//
bool CSerialPort::Read(void *pvBuffer, unsigned int uiNumOfBytes, unsigned long *pulNumOfBytesRead)
{
	try
	{
		// return if not open
		if (m_hComm < 0) {
			Display.ErrorMessageF( LOG_WARNING, "CSerialPort::Read()"
			                       " error: port closed" );
			return false;
		}

		int rc = read(m_hComm, pvBuffer, (size_t)uiNumOfBytes);
		if (rc < 0) {
			Display.ErrorMessageF( LOG_ERR, "CSerialPort::Read() "
			                       "failed. (errno=%i: %s)", errno,
			                       strerror(errno) );
			*pulNumOfBytesRead = 0;
			return false;
		}
		*pulNumOfBytesRead = rc;

		return true;
	}
	catch(...)
	{
		log_print_and_stderr( LOG_ERR, "CSerialPort::Read() unknown exception" );
		die();
		return false;
	}
}


bool CSerialPort::ReadBlocking(void *pvBuffer, unsigned int uiNumOfBytes, unsigned long *pulNumOfBytesRead)
{
	try
	{
		*pulNumOfBytesRead = 0;
		// return if not open
		if (m_hComm < 0) {
			Display.ErrorMessageF( LOG_WARNING, "CSerialPort::ReadBlocking()"
			                       " error: port closed" );
			return false;
		}
		int rc = read(m_hComm, pvBuffer, (size_t)uiNumOfBytes);
		if (rc == 0 && uiNumOfBytes > 0) {
			// A blocking read should never return 0 normally.
			// If it does, something strange (device unplugged) happened.
			Display.ErrorMessageF( LOG_ERR, "CSerialPort::ReadBlocking(): "
			                       "read returned 0. End of file reached?");
			return false;
		} else if (rc < 0) {
			Display.ErrorMessageF( LOG_ERR, "CSerialPort::ReadBlocking() "
			                       "failed. (errno=%i: %s)", errno,
			                       strerror(errno) );
			return false;
		} else {
			*pulNumOfBytesRead = rc;
			return true;
		}
	}
	catch(...)
	{
		log_print_and_stderr( LOG_ERR, "CSerialPort::ReadBlocking() unknown exception" );
		die();
		return false;
	}
}



//////////////////////////////////////////////////////////////////////////////
// Write
//
// Description: Write one or more bytes to the serial port.
//
// Return:      bool  -  true if successful, otherwise false.
//
// Parameter: unsigned char *pucBuffer  -  Contains the data to be written.
//            unsigned int uiNumOfBytes -  Number of bytes to write
//
bool CSerialPort::Write(void *pvBuffer, unsigned int uiNumOfBytes)
{
	try
	{
		// return if not open
		if (m_hComm < 0) {
			Display.ErrorMessageF( LOG_WARNING, "CSerialPort::Write()"
			                       " error: port closed" );
			return false;
		}

		//TODO rc==-1
		if ((unsigned int)write(m_hComm, pvBuffer, uiNumOfBytes) != uiNumOfBytes) {
			Display.ErrorMessageF( LOG_ERR, "CSerialPort::Write() could not write data"
			                       "to locobuffer." );
			return false;
		}

		return true;

	}
	catch(...)
	{
		log_print_and_stderr( LOG_ERR, "CSerialPort::Write() unknown exception" );
		die();
		return false;
	}
}


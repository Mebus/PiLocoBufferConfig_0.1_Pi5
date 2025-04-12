//////////////////////////////////////////////////////////////////////////////
// File name:		SerialPort.cpp
// $Id: SerialPort.cpp 1262 2023-02-27 14:33:22Z pischky $
//
// Class/Module(s):	CSerialPort
//
// Description:		Implementation of Class/Module(s). For further documentation
//					see description in header file.
//
// 050827  SBor     Added CTS flow control for transmit stream
// 2006-12-02       Replaced port index by device string
//
//////////////////////////////////////////////////////////////////////////////


#include <string.h> // strncpy
#include "SerialPort.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////////////
// CSerialPort
//
// Description:	Standard constructor.
//
CSerialPort::CSerialPort(unsigned int uiTxBufferSize, unsigned int uiRxBufferSize)
{
	m_uiTxBufferSize = uiTxBufferSize;
	m_uiRxBufferSize = uiRxBufferSize;

	m_KillEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_RxEvent   = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_ReadOverlapped.Offset = 0;
	m_ReadOverlapped.OffsetHigh = 0;
	m_ReadOverlapped.hEvent = 0;//(HANDLE)m_ReadEvent;

	m_WriteOverlapped.Offset = 0;
	m_WriteOverlapped.OffsetHigh = 0;
	m_WriteOverlapped.hEvent = 0;//(HANDLE)m_WriteEvent;

	m_RxOverlapped.Offset = 0;
	m_RxOverlapped.OffsetHigh = 0;
	m_RxOverlapped.hEvent = m_RxEvent;

	m_hComm = NULL;
}


//////////////////////////////////////////////////////////////////////////////
// ~CSerialPort
//
// Description:	Standard destructor.
//
CSerialPort::~CSerialPort()
{
	if(GetOpenStatus()) Close();
}



/*////////////////////////////////////////////////////////////////////////////
// CSerialPort::Open
//
// Description:	Open the serial port. In case of data reception on the
//				serial port either the windows message mechanism or a callback
//				function mechanism can be used for application notification.
//
// Return:		bool	-	true if successful, otherwise false
//
// Parameter:	EPort Port				-	Port number (PORT_1_E, PORT_2_E, PORT_3_E, PORT_4_E).
//				int iBaud				-	Baud rate (e.g. 4800, 19200, ...).
//				EDataBits DataBits		-	Data bits (DATA_BITS_5_E, DATA_BITS_6_E, DATA_BITS_7_E, DATA_BITS_8_E = 8).
//				EStopBits StopBits		-	Stop bits (STOP_BITS_1_E, STOP_BITS_1P5_E, STOP_BITS_2_E).
//				EParity Parity			-	Parity (NO_PARITY_E, ODD_PARITY_E, EVEN_PARITY_E, MARK_PARITY_E, SPACE_PARITY_E).
//				EFlow Flow				-	Flow control (FLOW_CONTROL_NONE_E).
*/
bool CSerialPort::Open(const char *pcPort, int iBaud, EDataBits DataBits, EStopBits StopBits, EParity Parity, EFlow Flow)
{
	try
	{
		// Check if already opened:
		if(m_hComm != NULL) return false;

		strncpy(m_acPort, pcPort, sizeof(m_acPort));
		m_acPort[sizeof(m_acPort)-1] = 0;  // make sure destination string is null terminated
		m_iBaud = iBaud;
		m_enDataBits = DataBits;
		m_enStopBits = StopBits;
		m_enParity = Parity;
		m_enFlow = Flow;

		// Create Com handle:
		m_hComm = CreateFile(	pcPort,
								GENERIC_READ | GENERIC_WRITE,
								0,
								0,
								OPEN_EXISTING,
								FILE_FLAG_OVERLAPPED,
								0);

		if(m_hComm == INVALID_HANDLE_VALUE) return false;

		// Set Tx and Rx buffer size:
		if(SetupComm(m_hComm, m_uiRxBufferSize, m_uiTxBufferSize) == FALSE) return false;

		// Fill DCB structure:	
		m_Dcb.DCBlength = sizeof(DCB);				// sizeof(DCB) 
		m_Dcb.fBinary = 1;							// binary mode, no EOF check 
		m_Dcb.fParity = 1;							// enable parity checking 
		m_Dcb.fErrorChar = 0;						// enable error replacement 
		m_Dcb.fNull = 0;							// enable null stripping 
		m_Dcb.fAbortOnError = 0;					// abort reads/writes on error 
		m_Dcb.wReserved = 0;						// not currently used 
		m_Dcb.ErrorChar = 0;						// error replacement character 
		m_Dcb.EofChar = 0;							// end of input character 
		m_Dcb.EvtChar = 0;							// received event character 
		m_Dcb.XonLim = 0;							// transmit XON threshold 
		m_Dcb.XoffLim = 0;							// transmit XOFF threshold 
		m_Dcb.fTXContinueOnXoff = 1;				// XOFF continues Tx 
		m_Dcb.fOutX = 0;							// XON/XOFF out flow control 
		m_Dcb.fInX = 0;								// XON/XOFF in flow control 
		m_Dcb.XonChar = 0;							// Tx and Rx XON character 
		m_Dcb.XoffChar = 0;							// Tx and Rx XOFF character 

		m_Dcb.BaudRate = m_iBaud;					// current baud rate 
		m_Dcb.ByteSize = m_enDataBits;				// number of bits/byte, 4-8 
		m_Dcb.Parity = m_enParity;					// 0-4=no,odd,even,mark,space 
		m_Dcb.StopBits = m_enStopBits;				// 0,1,2 = 1, 1.5, 2

		switch(m_enFlow)
		{
			case FLOW_CONTROL_CTS_E:
				m_Dcb.fRtsControl = RTS_CONTROL_ENABLE;     // RTS output for instream flow control
				m_Dcb.fOutxCtsFlow = TRUE;                  // CTS input for outstream flow control
				m_Dcb.fDtrControl = DTR_CONTROL_ENABLE;     // DTR output for instream flow control
				m_Dcb.fOutxDsrFlow = FALSE;                 // DSR input for outstream flow control
				m_Dcb.fDsrSensitivity = FALSE;              // DSR sensitivity 
				break;

			case FLOW_CONTROL_NONE_E:
			default:
				m_Dcb.fRtsControl = RTS_CONTROL_ENABLE;     // RTS output for instream flow control
				m_Dcb.fOutxCtsFlow = FALSE;                 // CTS input for outstream flow control
				m_Dcb.fDtrControl = DTR_CONTROL_ENABLE;     // DTR output for instream flow control
				m_Dcb.fOutxDsrFlow = FALSE;                 // DSR input for outstream flow control
				m_Dcb.fDsrSensitivity = FALSE;              // DSR sensitivity 
				break;
		}

		// Initialize port:
		if(SetCommState(m_hComm, &m_Dcb) == FALSE)
		{
			//unused: DWORD dw = GetLastError();

			Close();
			return false;
		}

		// Set timeouts:
		m_CommTimeouts.ReadIntervalTimeout = MAXDWORD ; 
		m_CommTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD ; 
		m_CommTimeouts.ReadTotalTimeoutConstant = 1; 
		m_CommTimeouts.WriteTotalTimeoutMultiplier = 0; 
		m_CommTimeouts.WriteTotalTimeoutConstant = 0; 

		if(SetCommTimeouts(m_hComm, &m_CommTimeouts) == FALSE)
		{
			Close();
			return false;
		}

		if(SetCommMask(m_hComm, EV_RXCHAR) == FALSE) return false;


		Reset();

		return true;
	}
	catch(...)
	{
		return false;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Close
//
// Description:	Closes the opened port. Can be called directly, but is also
//				called automatically when the CSerialPort object is destructed.
//
// Return:		bool	-	true if successful, otherwise false.
//
// Parameter:	void
//
bool CSerialPort::Close(void)
{
	try
	{
		SetEvent(m_KillEvent);

		if(m_hComm != NULL)
		{
			CloseHandle(m_hComm);
			m_hComm = NULL;
		}

		return true;
	}
	catch(...)
	{
		return false;
	}
}



//////////////////////////////////////////////////////////////////////////////
// Reset
//
// Description: Reset port. Rx and Tx buffers are flushed.
//
// Return:		bool	-	true if successful, otherwise false.
//
// Parameter:	void
//
bool CSerialPort::Reset(void)
{
	try
	{
		if(m_hComm == NULL) return false;
		if(PurgeComm(m_hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) == FALSE) return false;

		return true;
	}
	catch(...)
	{
		return false;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Read
//
// Description:	
//
// Return:		bool	-	true if successful, otherwise false.
//
// Parameter:	unsigned char *ucBuffer
//				unsigned int uiNumOfBytes
//				unsigned int &uiNumOfBytesRead
//
bool CSerialPort::Read(void *pvBuffer, unsigned int uiNumOfBytes, unsigned long *pulNumOfBytesRead)
{
	try
	{
		*pulNumOfBytesRead = 0;

		m_ReadOverlapped.Offset = 0;
		m_ReadOverlapped.OffsetHigh = 0;
		
		DWORD dwErrors = 0;
		if(ClearCommError(m_hComm, &dwErrors, &m_ComStat) == FALSE)
		{
			return false;
		}
		if(m_ComStat.cbInQue < uiNumOfBytes) uiNumOfBytes = m_ComStat.cbInQue;

		/* This is a bug fix for SiLabs virtual COM port driver as experienced
		   on XP for IB2. This driver returns error ERROR_INVALID_USER_BUFFER,
		   when uiNumOfBytes passes a value of zero. */
		if (uiNumOfBytes==0)
			return true;

		if(ReadFile(m_hComm, pvBuffer, uiNumOfBytes, pulNumOfBytesRead, &m_ReadOverlapped) == FALSE)
		{
			unsigned long ulError = GetLastError();
			switch(ulError) 
			{ 
				case ERROR_IO_PENDING: 	
				{ 
					if(GetOverlappedResult(m_hComm, &m_ReadOverlapped, pulNumOfBytesRead, FALSE) == FALSE)
					{
						if (pulNumOfBytesRead != 0) return true;
					}
				
					return false;
				}

				default:
				{
					return false;
				}
			}
		}

		return true;
	}
	catch(...)
	{
		return false;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Write
//
// Description:	Write one or more bytes to the serial port.
//
// Return:		bool	-	true if successful, otherwise false.
//
// Parameter:	unsigned char *pucBuffer			-	Contains the data to
//														be written.	
//				unsigned int uiNumOfBytes			-	Number of bytes to
//														write.
//
bool CSerialPort::Write(void *pvBuffer, unsigned int uiNumOfBytes)
{
	try
	{
		m_WriteOverlapped.Offset = 0;
		m_WriteOverlapped.OffsetHigh = 0;
		DWORD dwWritten;

		if(WriteFile(m_hComm, pvBuffer, uiNumOfBytes, &dwWritten, &m_WriteOverlapped) == FALSE)
		{
			switch(GetLastError())
			{
				case ERROR_IO_PENDING:
				{
					return true;
				}

				default:
				{
/*
					DWORD dwError = GetLastError();
					LPVOID lpMsgBuf;
					FormatMessage( 
						FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						dwError,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &lpMsgBuf,
						0,
						NULL 
					);
					// Process any inserts in lpMsgBuf.
					// ...
					// Display the string.
					printf("[%lu:%s]", dwError, (LPCTSTR)lpMsgBuf);
					// Free the buffer.
					LocalFree( lpMsgBuf );
 */
					return false;
				}
			}
		}

		return true;
	}
	catch(...)
	{
		return false;
	}
}


bool CSerialPort::ReadBlocking(void *pvBuffer, unsigned int uiNumOfBytes, unsigned long *pulNumOfBytesRead)
{
	try
	{
		if (!Read(pvBuffer, uiNumOfBytes, pulNumOfBytesRead))
			return false;
		if (*pulNumOfBytesRead>0) return true;

		HANDLE aEventHandles[2] = {	m_KillEvent, (HANDLE)m_RxEvent };
		DWORD dwEventMask = 0;

		if(WaitCommEvent(m_hComm, &dwEventMask, &m_RxOverlapped) == FALSE)
		{
			switch(GetLastError())
			{
				case ERROR_IO_PENDING:
					break;

				default: return false;
			}
		}

		switch(WaitForMultipleObjects(2, aEventHandles, FALSE, INFINITE))
		{
			case 1:
				if((dwEventMask & EV_RXCHAR) == EV_RXCHAR)
				{
					return Read(pvBuffer, uiNumOfBytes, pulNumOfBytesRead);
				}
				return true;

			default:
				return false;
		}
	}
	catch(...)
	{
		return false;
	}
}

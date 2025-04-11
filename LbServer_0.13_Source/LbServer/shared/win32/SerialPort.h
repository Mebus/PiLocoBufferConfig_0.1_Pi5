//////////////////////////////////////////////////////////////////////////////
// File name:		SerialPort.h
// $Id: SerialPort.h 1226 2023-02-14 11:02:08Z pischky $
//
// Class/Module(s):	CSerialPort
//
// Description:		Communication with the serial port using win32 sdk
//					functions.
//
// Author(s):		If this code works: UW
//
// History			15.07.1999	Version 0.1
//					28.07.1999	IsOpened() function added
//					2000-05-03	
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _SERIAL_PORT_H_
#define _SERIAL_PORT_H_


#include "wtypes.h" //FIXME: This supplies HANDLE, COMMCONFIG, DCB 
                    // but documentation tells things different (use windows.h ?)


const int SERIAL_PORT_STRING_LENGTH=128;


class CSerialPort  
{
public:
	enum EDataBits
	{
		DATA_BITS_5_E = 5,
		DATA_BITS_6_E = 6,
		DATA_BITS_7_E = 7,
		DATA_BITS_8_E = 8
	} m_enDataBits;

	enum EStopBits
	{
		STOP_BITS_1_E = 0,
		STOP_BITS_1P5_E = 1,
		STOP_BITS_2_E = 2
	} m_enStopBits;
	
	enum EParity
	{
		NO_PARITY_E = 0,
		ODD_PARITY_E = 1,
		EVEN_PARITY_E = 2,
		MARK_PARITY_E = 3,
		SPACE_PARITY_E = 4
	} m_enParity;

	enum EFlow
	{
		FLOW_CONTROL_NONE_E = 0,
		FLOW_CONTROL_CTS_E = 1
	} m_enFlow;

	char m_acPort[SERIAL_PORT_STRING_LENGTH];

	CSerialPort(unsigned int uiTxBufferSize = 1024, unsigned int uiRxBufferSize = 1024);
	virtual ~CSerialPort();
	bool Open(const char *pcPort, int iBaud = 9600, EDataBits DataBits = DATA_BITS_8_E, EStopBits StopBits = STOP_BITS_1_E, EParity Parity = NO_PARITY_E, EFlow Flow = FLOW_CONTROL_NONE_E);
	bool Close(void);
	bool Read(void *pvBuffer, unsigned int uiNumOfBytes, unsigned long *pulNumOfBytesRead);
	bool ReadBlocking(void *pvBuffer, unsigned int uiNumOfBytes, unsigned long *pulNumOfBytesRead);
	bool Write(void *pvBuffer, unsigned int uiNumOfBytes);
	bool Reset(void);
	bool GetOpenStatus(void) { if(m_hComm == NULL) return false; else return true; }
	
protected:

	HANDLE m_hComm;
	COMMCONFIG m_CommConfig;
	DCB	m_Dcb;
	COMMTIMEOUTS m_CommTimeouts;
	COMSTAT m_ComStat;
	OVERLAPPED m_RxOverlapped;
	HANDLE m_RxEvent;
	OVERLAPPED m_ReadOverlapped;
//	CEvent m_ReadEvent;
	OVERLAPPED m_WriteOverlapped;
//	CEvent m_WriteEvent;
	HANDLE m_KillEvent; 
	int m_iBaud;
	unsigned int m_uiTxBufferSize;
	unsigned int m_uiRxBufferSize;
};


#endif

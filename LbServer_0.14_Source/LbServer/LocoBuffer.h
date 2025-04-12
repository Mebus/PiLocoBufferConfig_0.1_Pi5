//-------------------------------------------------------------------
// File name   LocoBuffer.h
// $Id: LocoBuffer.h 1171 2023-01-07 12:56:40Z sbormann71 $
//-------------------------------------------------------------------
// Author      Stefan Bormann
//-------------------------------------------------------------------
// License:    This piece of code is part of the LoconetOverTcp 
//             project (http://loconetovertcp.sf.net) and therefore 
//             published under the terms of the GNU General Public 
//             Licence (http://www.gnu.org/licenses/gpl.html).
//             LocoNet is owned by Digitrax (http://www.digitrax.com)
//             Commercial use is subject to licensing by Digitrax!
//-------------------------------------------------------------------
// Description CLocoBuffer encapsulates the protocol between the
//             application and a LocoBuffer or Intellibx over an
//             RS232 line.
//-------------------------------------------------------------------
// History     2005-08-27  SBor: Added this header.
//-------------------------------------------------------------------


#if !defined(AFX_ABSTRACTLOCOBUFFER_H__960F5434_8B59_4710_AC1A_4254C9324D07__INCLUDED_)
#define AFX_ABSTRACTLOCOBUFFER_H__960F5434_8B59_4710_AC1A_4254C9324D07__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "SerialPort.h"
#include "AbstractLoconetDevice.h"



class CLocoBuffer : public CAbstractLoconetDevice
{
public:	// LoconetDevice abstract interface implementation
	virtual int ReceivePacket(void *pvData, unsigned int uiMaxLength);
	virtual bool SendPacket(void *pvData, unsigned int uiLength);

public:	// construction/destruction
	CLocoBuffer(void);
	virtual ~CLocoBuffer(){}

public:	// additional interface
	bool Open(const char *pcPort, int iComSpeed, bool bFlow);

private:
	void StripReceivedLeadingNonopcodes();
	bool CheckReceivedCheckByte(unsigned int uiCount);
	int GetLengthOfReceivedPacket();
	void ShrinkBuffer(unsigned int uiCount);
	int FindPacketInReceiveBuffer();
	unsigned char m_aucReceiveBuffer[127];
	unsigned int m_uiReceiveBufferFill;
	CSerialPort m_clPort;
};

#endif // !defined(AFX_ABSTRACTLOCOBUFFER_H__960F5434_8B59_4710_AC1A_4254C9324D07__INCLUDED_)

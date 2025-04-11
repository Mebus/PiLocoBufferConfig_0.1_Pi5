//-------------------------------------------------------------------
// File name   LocoBuffer.cpp
//             $Id: LocoBuffer.cpp 1214 2023-02-06 13:06:17Z pischky $
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
// History     2005-08-27  SBor: Added this header and added CTS
//                         flow control in order to support the
//                         small serial receive buffer of the
//                         Uhlenbrock Intellibox and long LN frames.
//             2005-11-23  IC: Mods to work with linux(macro Fprintfn
//                         same as printf but flushes stdout)
//             2022-11-23  SBor: refactor all display outputs
//-------------------------------------------------------------------

#include <string.h> /* memcpy(), memset() */

#include "LocoBuffer.h"     /* class CLocoBuffer */
#include "SerialPort.h"     /* class CSerialPort */
#include "DisplayFrontend.h" // Display object
#include "log.h"            /* LOG_ERR */

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CLocoBuffer::CLocoBuffer()
:	m_uiReceiveBufferFill(0)
{
	memset(m_aucReceiveBuffer, 0, sizeof(m_aucReceiveBuffer));
}


bool CLocoBuffer::Open(const char *pcPort, int iComSpeed, bool bFlow)
{
	bool rc = m_clPort.Open(pcPort,
		                    iComSpeed,
		                    CSerialPort::DATA_BITS_8_E,
		                    CSerialPort::STOP_BITS_1_E,
		                    CSerialPort::NO_PARITY_E,
		                    bFlow ? CSerialPort::FLOW_CONTROL_CTS_E
                                  : CSerialPort::FLOW_CONTROL_NONE_E);
	if (!rc)
	{
		// Display.ErrorMessage("[CSerialPort::Open() returned false]");
	}
	return rc;
}


bool CLocoBuffer::SendPacket(void *pvData, unsigned int uiLength)
{
//---- SBor050827: new version, writing byte by byte to force OS to make CTS hardware handshake (this apparently flushes the TX buffer for every call)
	unsigned char *puc = (unsigned char *)pvData;

	for (unsigned int ui=0; ui<uiLength; ui++)
	{
		if (!m_clPort.Write(puc, 1))
		{
			Display.ErrorMessage(LOG_ERR, "CSerialPort::Write() returned false");
			return false;
		}
		puc++;
	}
	return true;

//---- SBor050827: old version that is not compatible with CTS hardware handshake
//	return m_clPort.Write(pvData, uiLength);
}


int CLocoBuffer::ReceivePacket(void *pvData, unsigned int uiMaxLength)
{
	while (1) 
	{
//----------- Extract Packet from buffer

		int iReturn = FindPacketInReceiveBuffer();
		if (iReturn<0) return iReturn;
		if (iReturn>0)
		{
			{
				if (uiMaxLength>=(unsigned int)iReturn)	// packets fits in buffer of caller
				{
					memcpy(pvData, &m_aucReceiveBuffer[0], iReturn);	// return packet

					if ((unsigned int)iReturn < m_uiReceiveBufferFill)				// tidy up (remaining) buffer content
						ShrinkBuffer((unsigned int)iReturn);
					else
						m_uiReceiveBufferFill = 0;

					return iReturn;
				}
				else
				{
					Display.ErrorMessageF(LOG_ERR, "Incoming Packet (%d bytes) does not fit into callers buffer (%u bytes)", iReturn, uiMaxLength);
					return -1;
				}
			}
		}

//----------- Receive Bytes
		
		unsigned long ulRead;

		if (!m_clPort.ReadBlocking(&m_aucReceiveBuffer[m_uiReceiveBufferFill], 
		                   3,//sizeof(m_aucReceiveBuffer)-m_uiReceiveBufferFill, 
		                   &ulRead))
		{
			Display.ErrorMessage(LOG_ERR, "CSerialPort::ReadBlocking() returned false");
			return -1;
		}

		Display.ReceivedFromPort(&m_aucReceiveBuffer[m_uiReceiveBufferFill], (unsigned int)ulRead);

		m_uiReceiveBufferFill += ulRead;
	}
}


int CLocoBuffer::FindPacketInReceiveBuffer()
{
//----------- strip leading non-opcodes
	StripReceivedLeadingNonopcodes();

//----------- assure minimal legal packet size
	if (m_uiReceiveBufferFill<2) return 0;	// well, smallest packet is 2 bytes

//----------- checking packet length
	int iLength = GetLengthOfReceivedPacket();
	if (iLength<0)
	{
		Display.ReceiveError("[Illegal length byte -> discarding opcode]");
		ShrinkBuffer(1);
		return FindPacketInReceiveBuffer();	// recursion rules
	}
	if (m_uiReceiveBufferFill<(unsigned int)iLength) return 0;	// not yet completely received

//----------- checking xor byte
	if (!CheckReceivedCheckByte((unsigned int)iLength))
	{
		Display.ReceiveError("CLocoBuffer::FindPacketInReceiveBuffer(): XOR check failed -> discarding opcode");
		ShrinkBuffer(1);
		return FindPacketInReceiveBuffer();	// recursion rules
	}

//----------- now we are happy
	return iLength;
}


void CLocoBuffer::ShrinkBuffer(unsigned int uiCount)
{
	if (uiCount>0)
	{
		for (unsigned int ui=uiCount; ui<m_uiReceiveBufferFill; ui++)
			m_aucReceiveBuffer[ui-uiCount] = m_aucReceiveBuffer[ui];

		m_uiReceiveBufferFill -= uiCount;
	}
}


int CLocoBuffer::GetLengthOfReceivedPacket()
{
	switch (m_aucReceiveBuffer[0] & 0x60)
	{
		case 0x00:	return 2;	

		case 0x20:	return 4;

		case 0x40:	return 6;

		case 0x60:	{
						unsigned char ucLen = m_aucReceiveBuffer[1];
						if (ucLen<3)	// we made sure, that we have at least two bytes in buffer, didn't we?
							return -1;	// anyway, must be at least three for N-byte length packet
						else if (ucLen>0x7F)
							return -1;
						else
							return ucLen;
					}

		default:	return -1;	// not really a possible case ;-)
	}
}


bool CLocoBuffer::CheckReceivedCheckByte(unsigned int uiCount)
{
	unsigned char ucCheck = 0;

	for (unsigned int ui=0; ui<uiCount; ui++)
		ucCheck ^= m_aucReceiveBuffer[ui];

	return (ucCheck==0xFF);
}


void CLocoBuffer::StripReceivedLeadingNonopcodes()
{
	unsigned int ui, uiCount = 0;

	for (ui=0; ui<m_uiReceiveBufferFill; ui++)	
	{
		if (m_aucReceiveBuffer[ui] & 0x80) break;
		uiCount++;
	}
	ShrinkBuffer(uiCount);
}

//-------------------------------------------------------------------
// File name   LoconetMessage.cpp
// $Id: LoconetMessage.cpp 996 2017-12-09 18:18:22Z pischky $
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
// History     2003-11-29	SB: Added this header
//-------------------------------------------------------------------
// Description CLoconetMessage encapsulates a message between (and
//             including) opcode and check byte.
//-------------------------------------------------------------------


#include "LoconetMessage.h"
#include <string.h>


//////////////////////////////////////////////////////////////////////


CLoconetMessage::CLoconetMessage()
:	m_uiByteCount(0)
{
	memset(m_aucMessage, 0, sizeof(m_aucMessage));
}


CLoconetMessage::CLoconetMessage(unsigned char ucOpcode)
{
	m_aucMessage[0] = ucOpcode;
	m_aucMessage[1] = CalcCheck(1);
	m_uiByteCount = 2;
}


CLoconetMessage::CLoconetMessage(unsigned char ucOpcode, unsigned char ucParam1, unsigned char ucParam2)
{
	m_aucMessage[0] = ucOpcode;
	m_aucMessage[1] = ucParam1;
	m_aucMessage[2] = ucParam2;
	m_aucMessage[3] = CalcCheck(3);
	m_uiByteCount = 4;
}


CLoconetMessage::CLoconetMessage(const CLoconetMessage &rclCopyFrom)
:	m_uiByteCount(rclCopyFrom.m_uiByteCount)
{
//	ASSERT(m_uiByteCount<=sizeof(m_aucMessage));
	memcpy(&m_aucMessage, rclCopyFrom.m_aucMessage, m_uiByteCount);
}


CLoconetMessage::CLoconetMessage(unsigned char *pucMessage, unsigned int m_uiByteCount)
:	m_uiByteCount(m_uiByteCount)
{
//	ASSERT(m_uiByteCount<=sizeof(m_aucMessage));
	memcpy(&m_aucMessage, pucMessage, m_uiByteCount);
}


const char *CLoconetMessage::ParseFromStringWithoutCheck(const char *pcHexString)
{
	m_uiByteCount=0;

	if (pcHexString==NULL)
		return "hex dump of message is non existant";

	for (;;)
	{
		if (m_uiByteCount==sizeof(m_aucMessage))
		{
			return("hex dump of message is bigger than buffer -> must be invalid loconet message");
		}

		char cHigh = *pcHexString++;
		if (*pcHexString==0)
		{
			return("string ends in the middle of a hex byte");
		}
		char cLow = *pcHexString++;

		if ((!Hex2Nibble(cHigh)) || (!Hex2Nibble(cLow)))
		{
			return("Invalid hex coding");
		}

		m_aucMessage[m_uiByteCount] = (unsigned char)cHigh;
		m_aucMessage[m_uiByteCount]<<=4;
		m_aucMessage[m_uiByteCount] |= (unsigned char)cLow;

		m_uiByteCount++;

		while (*pcHexString==' ') pcHexString++;

		if (*pcHexString==0)
		{
			

			return NULL;	// success
		}
	}
}


bool CLoconetMessage::isLengthOk()
{
	unsigned int uiLength=0;

	if (m_uiByteCount<2)
	{
		return false;  // length too short
	}

	switch (m_aucMessage[0] & 0x60)  // examine opcode
	{
		case 0x00:	uiLength = 2;	
					break;

		case 0x20:	uiLength = 4;
					break;

		case 0x40:	uiLength = 6;
					break;

		case 0x60:	uiLength = m_aucMessage[1];  // variable length message
					break;
	}

	return (m_uiByteCount==uiLength);
}

const char *CLoconetMessage::ParseFromString(const char *pcHexString)
{
	const char *pcError = ParseFromStringWithoutCheck(pcHexString);

	if (pcError != NULL)
		return pcError;

	if (m_uiByteCount<2)
		return "message too short";

	if (!isLengthOk())
		return "message length is inconsistent";

	return NULL;
}


unsigned char CLoconetMessage::CalcCheck(unsigned int uiCount)
{
//	ASSERT(uiCount<sizeof(m_aucMessage));

	unsigned char ucWork = 0xff;

	for (unsigned int uiLoop=0; uiLoop<uiCount; uiLoop++)
		ucWork ^= m_aucMessage[uiLoop];

	return ucWork;
}


bool CLoconetMessage::Hex2Nibble(char &rc)
{
	if (rc>='0' && rc<='9')
	{
		rc -= '0';
		return true;
	}
	if (rc>='a' && rc<='f')
	{
		rc -='a';
		rc += 10;
		return true;
	}
	if (rc>='A' && rc<='F')
	{
		rc -='A';
		rc += 10;
		return true;
	}
	return false;
}


bool CLoconetMessage::Compare(CLoconetMessage &rclOther)
{
	if (m_uiByteCount!=rclOther.m_uiByteCount) return false;

	for (unsigned int ui=0; ui<m_uiByteCount; ui++)
		if (m_aucMessage[ui] != rclOther.m_aucMessage[ui]) 
			return false;

	return true;
}


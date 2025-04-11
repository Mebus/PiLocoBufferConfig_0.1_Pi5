//-------------------------------------------------------------------
// File name   LoconetMessage.h
// $Id: LoconetMessage.h 997 2017-12-09 19:06:30Z pischky $
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

#if !defined(_LOCONETMESSAGE_H_INCLUDED_)
#define _LOCONETMESSAGE_H_INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cstdint_compat.h" // uint8_t

class CLoconetMessage  
{
public:
	CLoconetMessage(unsigned char ucOpcode, unsigned char ucParam1, unsigned char ucParam2);
	CLoconetMessage(unsigned char ucOpcode);
	bool Compare(CLoconetMessage &rclOther);
	CLoconetMessage(const CLoconetMessage &rclCopyFrom);
	CLoconetMessage();
	CLoconetMessage(unsigned char *pucMessage, unsigned int uiByteCount);
	virtual ~CLoconetMessage(){}

	const char *ParseFromString(const char *pcHexString);
	const char *ParseFromStringWithoutCheck(const char *pcHexString);
	bool isLengthOk();
	unsigned char CalcCheck(unsigned int uiCount);

	uint8_t       m_aucMessage[130];
	unsigned int  m_uiByteCount;

private:
	// parser for incomming data
	bool Hex2Nibble(char &rc);
};


#endif // !defined(_LOCONETMESSAGE_H_INCLUDED_)

//-------------------------------------------------------------------
// File name   DisplayVerbose.h
//             $Id: DisplayVerbose.h 1214 2023-02-06 13:06:17Z pischky $
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
// Description Output according to IDisplay to normal stdout,
//             more verbose output.
//-------------------------------------------------------------------
// History     2022-11-20  SBor: Added
//-------------------------------------------------------------------

#ifndef _DisplayVerbose_H_
#define _DisplayVerbose_H_


#include <stdio.h>              // printf()
#include "DisplayFrontend.h"    // IDisplayBackend
#include "log.h" /* log_print(), tid(), LOG_EMERG=0, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG=7 */


class CDisplayVerbose : public IDisplayBackend
{
public:
	CDisplayVerbose()
		: bDots(false)
	{}

	virtual ~CDisplayVerbose()
	{
		log_print( LOG_NOTICE, "~CDisplayVerbose()" );
	}

private:
	bool bDots;

	void DotEnd()
	{
		if (bDots) 
			printf("\n");
		bDots = false;
	}

public:
	virtual void ErrorMessage(int priority, const char *text)
	{
		printf("%s\n", text);
	}

	virtual void ReceivedFromPort(const void *pvData, size_t uiLength)
	{
		if (uiLength == 0)
		{
			bDots = true;
			printf(".");
		}
		else
		{
			DotEnd();
			char buffer[1000];
			HexDump(buffer, "Received from port: ", "", pvData, uiLength);
			printf("%s\n", buffer);
		}
	}

	virtual void ReceivedMessage(const void *pvData, size_t uiLength)
	{
		DotEnd();
		char buffer[1000];
		HexDump(buffer, "Received message: ", "", pvData, uiLength);
		printf("%s\n", buffer);
	}

	virtual void SendMessage(const void *pvData, size_t uiLength)
	{
		DotEnd();
		char buffer[1000];
		HexDump(buffer, "Sending message: ", "", pvData, uiLength);
		printf("%s\n", buffer);
	}

	virtual void SentSuccess()
	{
		printf("Sent successfully!\n");
	}

	virtual void Connection(size_t uiCountClients)
	{
		printf("Client connected, now %u clients connected\n", static_cast<unsigned int>(uiCountClients));
	}

	virtual void Disconnection(size_t uiCountClients)
	{
		printf("Client disconnected, now %u clients connected\n", static_cast<unsigned int>(uiCountClients));
	}

	virtual void SendError(const char* text)
	{
		printf("LocoNet send error: %s\n", text);
	}

	virtual void ReceiveError(const char* text)
	{
		printf("LocoNet receive error: %s\n", text);
	}
};


#endif //_DisplayVerbose_H_

//-------------------------------------------------------------------
// File name   DisplayNormal.h
//             $Id: DisplayNormal.h 1218 2023-02-07 14:32:51Z pischky $
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
//             normally with single characters.
//-------------------------------------------------------------------
// History     2022-11-20  SBor: Added
//-------------------------------------------------------------------

#ifndef _DisplayNormal_H_
#define _DisplayNormal_H_


#include <stdio.h>              // printf()
#include "DisplayFrontend.h"    // IDisplayBackend
#include "log.h" /* log_print(), tid(), LOG_EMERG=0, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG=7 */

class CDisplayNormal : public IDisplayBackend
{
public:
	CDisplayNormal()
	{ }

	virtual ~CDisplayNormal()
	{
		log_print( LOG_NOTICE, "~CDisplayNormal()" );
	}

	virtual void ErrorMessage(int priority, const char *text)
	{
		//printf("[%s]", text);
		printf( "\n%s\n", text);
	}

	virtual void ReceivedFromPort(const void *pvData, size_t uiLength)
	{
		if (uiLength > 0)
			printf(".");
	}

	virtual void ReceivedMessage(const void *pvData, size_t uiLength)
	{
		printf("R");
	}

	virtual void SendMessage(const void *pvData, size_t uiLength)
	{
		printf("s");
	}

	virtual void SentSuccess()
	{
		printf("S");
	}

	virtual void Connection(size_t uiCountClients)
	{
		printf("C");
	}

	virtual void Disconnection(size_t uiCountClients)
	{
		printf("D");
	}

	virtual void SendError(const char* text)
	{
		ErrorMessage(LOG_ERR, text);
	}

	virtual void ReceiveError(const char* text)
	{
		ErrorMessage(LOG_ERR, text);
	}
};


#endif //_DisplayNormal_H_

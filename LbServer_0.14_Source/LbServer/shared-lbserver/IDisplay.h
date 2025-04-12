//-------------------------------------------------------------------
// File name   IDisplay.h
//             $Id: IDisplay.h 1214 2023-02-06 13:06:17Z pischky $
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
// Description This abstract base class serves as interface for all
//             code that has something to tell the user.
//             Different implementations output to stdout, LCD display or system journal
//-------------------------------------------------------------------
// History     2022-11-20  SBor: Added
//-------------------------------------------------------------------

#ifndef _IDisplay_H_
#define _IDisplay_H_


#include <stddef.h>    // size_t
#include "log.h" /* log_print(), tid(), LOG_EMERG=0, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG=7 */

class IDisplay
{
public:
	virtual ~IDisplay()
	{
		log_print( LOG_NOTICE, "~IDisplay()" );
	}
	virtual void ErrorMessage(int priority, const char* text)=0;
	virtual void ReceivedFromPort(const void *pvData, size_t uiLength)=0;
	virtual void ReceivedMessage(const void *pvData, size_t uiLength)=0;
	virtual void SendMessage(const void *pvData, size_t uiLength)=0;
	virtual void SentSuccess()=0;
	virtual void Connection(size_t uiCountClients)=0;
	virtual void Disconnection(size_t uiCountClients)=0;
	virtual void SendError(const char* text)=0;
	virtual void ReceiveError(const char* text)=0;
};

#endif

//-------------------------------------------------------------------
// File name   DisplayFrontend.h
//             $Id: DisplayFrontend.h 1223 2023-02-08 13:41:02Z pischky $
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
//             code that hassomething to tell the user.
//             Different implementations output to stdout, LCD display or system journal
//-------------------------------------------------------------------
// History     2022-11-20  SBor: Added
//-------------------------------------------------------------------

#ifndef _DISPLAY_FE_H_
#define _DISPLAY_FE_H_


#include "IDisplay.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "log.h" /* log_print(), tid(), LOG_EMERG=0, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG=7 */

typedef enum
{
	DispErrorMessage,
	DispReceivedFromPort,
	DispReceivedMessage,
	DispSendMessage,
	DispSentSuccess,
	DispConnection,
	DispDisconnection,
	DispSendError,
	DispReceiveError,
}
EDisplay;

class IDisplayBackend : public IDisplay
{
	friend class CDisplayFrontend;
public:
	IDisplayBackend()
	:	_next(NULL)
	{ }
	~IDisplayBackend()
	{
		log_print( LOG_NOTICE, "~IDisplayBackend()" );
	}
private:
	IDisplayBackend* _next;
};


void HexDump(char* target, const char* prefix, const char* postfix, const void* pvData, size_t uiLength);


// This routes all calls from the IDisplay API to a Generic() method and from there to registered IDisplayBackend implementations
class CDisplayFrontend : public IDisplay
{
private:
	IDisplayBackend* _root;

	void Generic(EDisplay enDisplay, const void *pvData, size_t uiLength, int priority = -1, const char* freeText = NULL);

public:
	CDisplayFrontend()
	: _root(NULL)
	{}

	void Add(IDisplayBackend* added)
	{
		added->_next = _root;
		_root = added;
	}

	void DeleteAll()
	{
		IDisplayBackend* p = _root;
		while (p != NULL) {
			IDisplayBackend* tmp = p;
			// first: remove from list
			p = p->_next;
			_root = p;
			// second: delete
			delete tmp;
		}
	}

	/**
	 * Print an error message with format (like printf).
	 *
	 * Note: the message should not include any "\n" at start or end.
	 *
	 * @param priority LOG_EMERG .. LOG_DEBUG
	 * @param format   like in printf
	 */
	void ErrorMessageF(int priority, const char* format, ...)
	{
		char buffer[10000];
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		ErrorMessage(priority, buffer);
	}

	/**
	 * Print an error message.
	 *
	 * Note: the message should not include any "\n" at start or end.
	 *
	 * @param priority LOG_EMERG .. LOG_DEBUG
	 * @param text     the message
	 */
	virtual void ErrorMessage(int priority, const char *text)
	{Generic(DispErrorMessage, NULL, 0, priority, text);}

	virtual void ReceivedFromPort(const void *pvData, size_t uiLength)
	{Generic(DispReceivedFromPort, pvData, uiLength);}

	virtual void ReceivedMessage(const void *pvData, size_t uiLength)
	{Generic(DispReceivedMessage, pvData, uiLength);}

	virtual void SendMessage(const void *pvData, size_t uiLength)
	{Generic(DispSendMessage, pvData, uiLength);}

	virtual void SentSuccess()
	{Generic(DispSentSuccess, NULL, 0);}

	virtual void Connection(size_t uiCountClients)
	{Generic(DispConnection, NULL, uiCountClients);}

	virtual void Disconnection(size_t uiCountClients)
	{Generic(DispDisconnection, NULL, uiCountClients);}

	virtual void SendError(const char* text)
	{Generic(DispSendError, NULL, 0, LOG_ERR, text);}

	virtual void ReceiveError(const char* text)
	{Generic(DispReceiveError, NULL, 0, LOG_ERR, text);}
};


// make the publically accessible interface implementation instance, which forwards generically to the output classes
extern CDisplayFrontend Display;


#endif //_DISPLAY_FE_H_

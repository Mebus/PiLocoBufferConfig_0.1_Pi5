//-------------------------------------------------------------------
// File name   DisplayJournal.h
//             $Id: DisplayJournal.h 1215 2023-02-06 16:46:51Z pischky $
//-------------------------------------------------------------------
// Author      Martin Pischky
//-------------------------------------------------------------------
// License:    This piece of code is part of the LoconetOverTcp 
//             project (http://loconetovertcp.sf.net) and therefore 
//             published under the terms of the GNU General Public 
//             Licence (http://www.gnu.org/licenses/gpl.html).
//             LocoNet is owned by Digitrax (http://www.digitrax.com)
//             Commercial use is subject to licensing by Digitrax!
//-------------------------------------------------------------------
// Description Output according to IDisplay to Journal (SystemD).
//-------------------------------------------------------------------
// History     2023-02-06  Pi: Added
//-------------------------------------------------------------------

#ifndef _DISPLAY_JOURNAL_H_INCLUDED_
#define _DISPLAY_JOURNAL_H_INCLUDED_

#include "DisplayFrontend.h"    // IDisplayBackend, HexDump()
#include "log.h" /* log_print(), LOG_EMERG=0, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG=7 */

#ifdef WITH_JOURNALD
    #include <systemd/sd-journal.h> /* sd_journal_print() */
#endif

#ifdef WITH_JOURNALD

class DisplayJournal : public IDisplayBackend
{
public:
	DisplayJournal()
	{ }

	virtual ~DisplayJournal()
	{
		log_print( LOG_NOTICE, "~DisplayJournal()" );
	}

	virtual void ErrorMessage(int priority, const char *text)
	{
		sd_journal_print(priority, text);
	}

	virtual void ReceivedFromPort(const void *pvData, size_t uiLength)
	{
		if (uiLength > 0) {
			char buffer[3 * uiLength + 200];
			HexDump(buffer, "Received from port: ", "", pvData, uiLength);
			sd_journal_print(LOG_DEBUG, buffer);
		}
	}

	virtual void ReceivedMessage(const void *pvData, size_t uiLength)
	{
		char buffer[3 * uiLength + 200];
		HexDump(buffer, "Received message: ", "", pvData, uiLength);
		sd_journal_print(LOG_INFO, buffer);
	}

	virtual void SendMessage(const void *pvData, size_t uiLength)
	{
		char buffer[3 * uiLength + 200];
		HexDump(buffer, "Sending message: ", "", pvData, uiLength);
		sd_journal_print(LOG_INFO, buffer);
	}

	virtual void SentSuccess()
	{
		sd_journal_print(LOG_INFO, "Sent successfully!");
	}

	virtual void Connection(size_t uiCountClients)
	{
		sd_journal_print(LOG_NOTICE, "Client connected, now %u clients "
		               "connected", static_cast<unsigned int>(uiCountClients));
	}

	virtual void Disconnection(size_t uiCountClients)
	{
		sd_journal_print(LOG_NOTICE, "Client disconnected, now %u clients "
		               "connected", static_cast<unsigned int>(uiCountClients));
	}

	virtual void SendError(const char* text)
	{
		sd_journal_print(LOG_WARNING, "LocoNet send error: %s", text);
	}

	virtual void ReceiveError(const char* text)
	{
		sd_journal_print(LOG_WARNING, "LocoNet receive error: %s", text);
	}
};

#endif // WITH_JOURNALD

#endif // _DISPLAY_JOURNAL_H_INCLUDED_

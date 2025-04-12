//-------------------------------------------------------------------
// File name   DisplayLcd.h
//             $Id: DisplayLcd.h 1224 2023-02-09 21:24:54Z sbormann71 $
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
// Description Display user related information by means of an LCD
//             16x2. For test of this functionallity, there exists
//             a simulation version outputs the two display lines
//             into a single line on the terminal.
//-------------------------------------------------------------------
// History     2022-12-03  SBor: Added this header.
//-------------------------------------------------------------------

#ifndef _LCD_H_
#define _LCD_H_

#include <cassert>      // assert()
#include <cstdio> 	    // FILE
#include <cstring>      // strlen(), memcpy(), memset()
#include "log.h" /* log_print(), tid(), LOG_EMERG=0, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG=7 */
#include "DisplayFrontend.h"
#include "AbstractThread.h"
#include "SyncObjects.h"
#include "LcdTopics.h"

class CDisplayLcd : public CAbstractThread, public IDisplayBackend
{
protected:
	CTxRx    TxRx;
	CDefault Default;
	CConnect Connect;
	CCounter Disconnected;
	CCounter ReceiveErrors;
	CCounter SendErrors;
	CCounter Errors;
	// if this is zero, we are free to change the view, otherwise decrement and drink tee
	unsigned int uiTicksToView;

	virtual void ThreadMain();
	virtual void WriteTopLine(const char *msg)=0;
	virtual void WriteBottomLine(const char *msg)=0;
	void CheckTop(bool mayChange);
	void CheckBottom();

private:
	// Shutdown trigger for the thread, waiting for the event is the way to time the actions
	// of the thread.
	CSyncEvent ShutdownEvent;

public:
	static const int LCD_WIDTH = CTopic::WIDTH; // number of characters per line
	CDisplayLcd();
	virtual ~CDisplayLcd();
	virtual void DisplayShutdown() {
		WriteTopLine   ("LbServer        ");
		WriteBottomLine("       goes down");
	}
	virtual void ErrorMessage(int priority, const char* text);
	virtual void ReceivedFromPort(const void *pvData, size_t uiLength);
	virtual void ReceivedMessage(const void *pvData, size_t uiLength);
	virtual void SendMessage(const void *pvData, size_t uiLength);
	virtual void SentSuccess();
	virtual void Connection(size_t uiCountClients);
	virtual void Disconnection(size_t uiCountClients);
	virtual void SendError(const char* text);
	virtual void ReceiveError(const char* text);
};

class CDisplayLcdSimul : public CDisplayLcd
{
private:
	char m_pcTop   [LCD_WIDTH + 1];
	char m_pcBottom[LCD_WIDTH + 1];
	unsigned int m_uiRefeshCounter;

public:
	CDisplayLcdSimul()
	:   m_uiRefeshCounter(0)
	{
		log_print( LOG_DEBUG, "CDisplayLcdSimul()" );
		memset(m_pcTop,    ' ', LCD_WIDTH); m_pcTop   [LCD_WIDTH] = '\0'; // MSC requires this, sorry
		memset(m_pcBottom, ' ', LCD_WIDTH); m_pcBottom[LCD_WIDTH] = '\0';
		assert( strlen(m_pcTop   ) == LCD_WIDTH );
		assert( strlen(m_pcBottom) == LCD_WIDTH );
		this->StartThread("DispSimul"); // moved from CDisplayLcd::CDisplayLcd() to here
	}

	virtual ~CDisplayLcdSimul()
	{
		log_print( LOG_NOTICE, "~CDisplayLcdSimul()" );
		DisplayShutdown();
	}

	virtual void WriteTopLine(const char *msg)
	{
		size_t len = strlen(msg); // copy to local variable because msg may be on stack
		if (len > LCD_WIDTH) len = LCD_WIDTH;
		memcpy(m_pcTop, msg, len); // if msg is shorter than LCD_WIDTH leave rest as before
		Output();
	}

	virtual void WriteBottomLine(const char *msg)
	{
		size_t len = strlen(msg);
		if (len > LCD_WIDTH) len = LCD_WIDTH;
		memcpy(m_pcBottom, msg, len);
		Output();
	}

private:
	void Output()
	{
		m_uiRefeshCounter++;
		assert( strlen(m_pcTop   ) == LCD_WIDTH );
		assert( strlen(m_pcBottom) == LCD_WIDTH );
		printf("Top:[%s]   Bottom:[%s]   Refreshs:%u\r", m_pcTop, m_pcBottom, m_uiRefeshCounter);
	}
};

#ifdef __linux__

/**
 * Implementaion of CDisplayLcd on PiLocoBuffer hardware.
 * Use 16x2 LCD connected to PiLocoBuffer-Hat by "/dev/lcd".
 */
class CDisplayLcdPiLocoBuffer : public CDisplayLcd
{
private:
	FILE* device;
	inline void gotoxy(int x, int y) {
		fprintf( device, "\033[Lx%03iy%03i;", x, y);
	}
	static const char CLEAR_DISPLAY[];
	static const char CURSOR_HOME[];
	static const char DISPLAY_ON[];
	static const char DISPLAY_OFF[];
	static const char CURSOR_ON[];
	static const char CURSOR_OFF[];
	static const char BLINK_ON[];
	static const char BLINK_OFF[];
	static const char BACK_LIGHT_BLINK[];
	static const char BACK_LIGHT_ON[];
	static const char BACK_LIGHT_OFF[];
	static const char CURSOR_LEFT[];
	static const char CURSOR_RIGHT[];
	static const char SHIFT_DISPLAY_LEFT[];
	static const char SHIFT_DISPLAY_RIGHT[];
	static const char KILL_TO_ENDLINE[];
	static const char REINIT_DISPLAY[];
public:
	CDisplayLcdPiLocoBuffer(FILE* device);
	CDisplayLcdPiLocoBuffer(const char* deviceName);
	virtual ~CDisplayLcdPiLocoBuffer();
	virtual void WriteTopLine(const char *msg);
	virtual void WriteBottomLine(const char *msg);
	static FILE* openLcdDevice(const char* deviceName);
};

#endif // __linux__

#endif //_LCD_H_

//-------------------------------------------------------------------
// File name   Lcd.h
//             $Id: DisplayLcd.cpp 1224 2023-02-09 21:24:54Z sbormann71 $
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

#include <cassert>      // assert()
#include <cerrno>       // errno
#include <cstdio>       // FILE, fopen(), fclose(), fprintf()
#include <cstdlib>      // exit()
#include <cstring>      // strerror()
#include <stdexcept>    // runtime_error
#ifdef __linux__
    #include <unistd.h> // access(), F_OK, W_OK
    #include <pthread.h> // pthread_join()
#endif
#include "log.h" /* log_print(), tid(), LOG_EMERG=0, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG=7 */

#include "DisplayLcd.h"
#include "Version.h"

using std::runtime_error;

const unsigned int MS_SHOW = 2000; // how long to show one information topic
const unsigned int MS_TICK = 150;  // how long to wait at a minimum between updates?
const unsigned int TICKS_PER_TOPIC = MS_SHOW / MS_TICK; // counting iterations befor changing the Topic


// ------------------------------------------------------------ CDisplayLcd ---

CDisplayLcd::CDisplayLcd()
:	Default("LbServer", VERSION),
	Disconnected("Disconnected"),
	ReceiveErrors("Rx-Error"),
	SendErrors("Tx-Error"),
	Errors("Error"),
	uiTicksToView(0)
{
	log_print( LOG_DEBUG, "CDisplayLcd()" );
	//StartThread(); //call later after initialization is complete
}

CDisplayLcd::~CDisplayLcd()
{
	#ifdef __linux__
		log_print( LOG_NOTICE, "~CDisplayLcd() m_pthread=%lx", m_pthread );
	#else
		log_print( LOG_NOTICE, "~CDisplayLcd()" );
	#endif
	ShutdownEvent.Set(); // trigger shutdown and "Bye"
	#ifdef __linux__
		// on real hardware we should wait on output completion
		if (m_pthread != 0) {
			log_print(LOG_DEBUG, "~CDisplayLcd() pthread_join(%lx) --start", m_pthread);
			int rc = pthread_join(m_pthread, NULL);
			if (rc != 0) {
				log_print(LOG_ERR, "~CDisplayLcd() pthread_join failed rc=%i: %s", rc, strerror(rc));
			} else {
				// when thread terminates before we get here m_pthread is 0. Why ??
				log_print(LOG_DEBUG, "~CDisplayLcd() pthread_join() --done m_pthread=%lx", m_pthread);
			}
		 	m_pthread = 0;
		}
	#endif
}

void CDisplayLcd::ThreadMain()
{
	log_print( LOG_DEBUG, "CDisplayLcd::ThreadMain() --start" );
	while (!ShutdownEvent.Wait(MS_TICK))
	{
		if (uiTicksToView==0)
		{
			CheckTop(true);
			uiTicksToView = TICKS_PER_TOPIC;
		}
		else
		{
			CheckTop(false);
			uiTicksToView--;
		}

		CheckBottom();
	}
	log_print( LOG_DEBUG, "CDisplayLcd::ThreadMain() --end" );
}

void CDisplayLcd::CheckTop(bool mayChange)
{
	static CTopic *previousT = NULL;
	CTopic *t = NULL;

	{
		CMutexUser mu(CTopic::LcdMutex);

		if (mayChange)
		{
			if      (Connect.      m_Request)  t = &Connect;
			else if (Errors.       m_Request)  t = &Errors;
			else if (SendErrors.   m_Request)  t = &SendErrors;
			else if (ReceiveErrors.m_Request)  t = &ReceiveErrors;
			else                               t = &Default;

			if (t == previousT && !t->m_Request)
				return; // same topic and no new request

			previousT = t;
		}
		else
		{
			t = previousT;
			if (!t->m_Request)
				return;

		}
		t->m_Request = false; // resetting before exiting mutex!
	}


	if (t == NULL)
	{
		WriteTopLine("****************"); // IP address as good default??
		return;
	}

	const char *msg = t->Get();
	WriteTopLine(msg);
}

void CDisplayLcd::CheckBottom()
{
	{
		CMutexUser mu(CTopic::LcdMutex);
		if (!TxRx.m_Request)
			return;

		TxRx.m_Request = false;
	}

	const char *msg =  TxRx.Get();
	WriteBottomLine(msg);
}

void CDisplayLcd::ErrorMessage(int priority, const char* text)
{
	if (priority <= LOG_ERR) { //quick and dirty fix
		Errors.Increment();
	}
}

void CDisplayLcd::ReceivedFromPort(const void *pvData, size_t uiLength)
{}

void CDisplayLcd::ReceivedMessage(const void *pvData, size_t uiLength)
{
	TxRx.IncrementRx();
}

void CDisplayLcd::SendMessage(const void *pvData, size_t uiLength)
{}

void CDisplayLcd::SentSuccess()
{
	TxRx.IncrementTx();
}

void CDisplayLcd::Connection(size_t uiCountClients)
{
	Connect.Connected((unsigned int)uiCountClients);
}

void CDisplayLcd::Disconnection(size_t uiCountClients)
{
	Connect.Disconnected((unsigned int)uiCountClients);
}

void CDisplayLcd::SendError(const char* text)
{
	SendErrors.Increment();
}

void CDisplayLcd::ReceiveError(const char* text)
{
	ReceiveErrors.Increment();
}

// ------------------------------------------------ CDisplayLcdPiLocoBuffer ---

#ifdef __linux__

// see kernal source linux/drivers/auxdisplay/charlcd.c
// and https://falb18.github.io/tinkering_at_night/raspberrypi-hd44780-driver.html
// "\b", "\t", "\n", "\r" are also valid on LCD display

#define ESC "\033" // "\x1B"

const char CDisplayLcdPiLocoBuffer::CLEAR_DISPLAY[]       = "\014";
							// "\v" 0x0b: vertical tab, or: ESC "[2J"
const char CDisplayLcdPiLocoBuffer::CURSOR_HOME[]         = ESC "[H";
							// "[h" does not work
const char CDisplayLcdPiLocoBuffer::DISPLAY_ON[]          = ESC "[LD";
const char CDisplayLcdPiLocoBuffer::DISPLAY_OFF[]         = ESC "[Ld";
const char CDisplayLcdPiLocoBuffer::CURSOR_ON[]           = ESC "[LC";
const char CDisplayLcdPiLocoBuffer::CURSOR_OFF[]          = ESC "[Lc";
const char CDisplayLcdPiLocoBuffer::BLINK_ON[]            = ESC "[LB";
const char CDisplayLcdPiLocoBuffer::BLINK_OFF[]           = ESC "[Lb";
const char CDisplayLcdPiLocoBuffer::BACK_LIGHT_ON[]       = ESC "[L+";
const char CDisplayLcdPiLocoBuffer::BACK_LIGHT_OFF[]      = ESC "[L-";
const char CDisplayLcdPiLocoBuffer::BACK_LIGHT_BLINK[]    = ESC "[L*";
const char CDisplayLcdPiLocoBuffer::CURSOR_LEFT[]         = ESC "[Ll";
const char CDisplayLcdPiLocoBuffer::CURSOR_RIGHT[]        = ESC "[Lr";
const char CDisplayLcdPiLocoBuffer::SHIFT_DISPLAY_LEFT[]  = ESC "[LL";
const char CDisplayLcdPiLocoBuffer::SHIFT_DISPLAY_RIGHT[] = ESC "[LR";
const char CDisplayLcdPiLocoBuffer::KILL_TO_ENDLINE[]     = ESC "[Lk";
const char CDisplayLcdPiLocoBuffer::REINIT_DISPLAY[]      = ESC "[LI";

FILE* CDisplayLcdPiLocoBuffer::openLcdDevice(const char* deviceName) {
	log_print( LOG_INFO, "openLcdDevice(\"%s\")", deviceName );
	int rc;
	rc = access(deviceName, F_OK); // test on files exists
	if (rc != 0) {
		log_print( LOG_ERR, "openDevice(\"%s\") failed: "
		           "file does not exist (errno=%i: %s)", deviceName,
		           static_cast<int>(errno), strerror(errno) );
		// exit(99); //TODO should throw an exception
		throw runtime_error("openLcdDevice() failed");
	}
	rc = access(deviceName, W_OK); // test on write access
	if (rc != 0) {
		log_print( LOG_ERR, "openDevice(\"%s\") failed: "
		           "no access (errno=%i: %s)", deviceName,
		           errno, strerror(errno) );
		throw runtime_error("openLcdDevice() failed");
	}
	FILE* device = fopen(deviceName, "w");
	if (device == nullptr) {
		log_print( LOG_ERR, "openDevice(\"%s\") failed: "
		           "fopen failed with errno=%i: %s", deviceName,
		           errno, strerror(errno) );
		throw runtime_error("openLcdDevice() failed");
	}
	fprintf( device, "%s%s%s%s%s%s",
	         CDisplayLcdPiLocoBuffer::BACK_LIGHT_ON,
	         CDisplayLcdPiLocoBuffer::DISPLAY_ON,
	         CDisplayLcdPiLocoBuffer::CLEAR_DISPLAY,
	         CDisplayLcdPiLocoBuffer::CURSOR_OFF,
	         CDisplayLcdPiLocoBuffer::BLINK_OFF,
	         CDisplayLcdPiLocoBuffer::CURSOR_HOME);
	// otherwise second line is empty until first RX/TX
	fprintf( device, "\nTX: 0      RX: 0%s",
	         CDisplayLcdPiLocoBuffer::CURSOR_HOME );
	fflush( device );
	log_print( LOG_DEBUG, "openLcdDevice(\"%s\") succeeded", deviceName );
	return device;
}

CDisplayLcdPiLocoBuffer::CDisplayLcdPiLocoBuffer(FILE* device)
:	device(device)
{
	log_print( LOG_DEBUG, "CDisplayLcdPiLocoBuffer(FILE*)" );
	this->StartThread("DispLcdPi"); // moved from CDisplayLcd::CDisplayLcd() to here
}

CDisplayLcdPiLocoBuffer::CDisplayLcdPiLocoBuffer(const char* deviceName)
:	CDisplayLcdPiLocoBuffer( openLcdDevice(deviceName) )
{
	log_print( LOG_DEBUG, "CDisplayLcdPiLocoBuffer(\"%s\")", deviceName );
}

CDisplayLcdPiLocoBuffer::~CDisplayLcdPiLocoBuffer() {
	log_print( LOG_NOTICE, "~CDisplayLcdPiLocoBuffer()" );
	if (device != nullptr) {
		DisplayShutdown();
		fclose( device );
		device = nullptr;
	}
}

void CDisplayLcdPiLocoBuffer::WriteTopLine(const char *msg) {
	assert( device != nullptr );
	// gotoxy( 0, 0 );
	fprintf( device, "%s%s", CDisplayLcdPiLocoBuffer::CURSOR_HOME, msg );
	fflush( device );
}

void CDisplayLcdPiLocoBuffer::WriteBottomLine(const char *msg) {
	assert( device != nullptr );
	gotoxy( 0, 1 );
	fprintf( device, "%s", msg );
	fflush( device );
}

#endif // __linux__

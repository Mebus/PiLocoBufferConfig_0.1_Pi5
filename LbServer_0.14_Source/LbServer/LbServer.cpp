//-------------------------------------------------------------------
// File name   LbServer.cpp
//             $Id: LbServer.cpp 1314 2023-04-10 10:50:37Z sbormann71 $
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
// Description This is the main program of the LbServer.exe 
//             win32 application. It implements a LoconetOverTcp
//             server that talks to LocoBuffer or Intellibox.
//             This module implements the main() function, command
//             line parameter parsing and initialization.
//-------------------------------------------------------------------
// History     2005-08-27  SBor: Added this header.
//             2006-11-23  IC: Minor mods to compile on linux (Fprintf1),
//                         added -h as a legal help command line argument
//             2012-10-14  Raneri Epifanio: Added flow Control enable parameter
//-------------------------------------------------------------------

#include <string>       /* class std::string */
#include <stdexcept>    /* exception */
#include <stdio.h>      /* ssize_t, printf(), fprintf(), setvbuf(), _IONBF */
#include <stdlib.h>     /* atoi(), strtol(), free() */
#ifdef WITH_JOURNALD
    #include <linux/limits.h>   /* PATH_MAX */
    #include <unistd.h>         /* readlink() */
#endif
#include "log.h" /* log_print(), log_max_priority,
                    LOG_EMERG=0, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG=7 */
#ifdef __unix__
    #include <unistd.h> /* _exit() */
    #include <csignal>  /* signal(), sigaction(), sigemptyset(), sigaddset(),
                           sys_siglist[], SIGABRT, SIGHUB, SIGINT, SIGTERM,
                           SIG_ERR, SIG_IGN, __sighandler_t, struct sigaction */
#endif
#ifdef __linux__
    #include <sys/stat.h> // lstat64(), struct stat64
#endif
#ifdef _WIN32
    #include <windows.h>  // SetConsoleCtrlHandler()
#endif

#include "LoconetServer.h"  /* class CLoconetServer */
#include "LocoBuffer.h"     /* class CLocoBuffer */  
#include "Version.h"        /* VERSION, PROTOCOL */
#include "SerialPort.h"     /* class CSerialPort, 
                               SERIAL_PORT_STRING_LENGTH */
#include "OsSpecific.h"     /* SOCKET_ERROR, WSADATA, WSAStartup(), 
                               WSACleanup() */
#include "DisplayFrontend.h" /* extern Display */
#include "DisplayNormal.h"   /* class CDisplayNormal */
#include "DisplayJournal.h"  /* class DisplayJournal */
#include "DisplayVerbose.h"  /* class CDisplayVerbose */
#include "DisplayLcd.h"      /* class CDisplayLcdPiLocoBuffer,
                                class CDisplayLcdSimul */
#include "LbServerConfig.h"  /* class CLbServerConfig */

using std::string;
using std::exception;

#ifndef WITH_JOURNALD
	int log_max_priority = LOG_NOTICE; // define log level (see: "log.h") when journald is not used
#endif

/**
 * Print help message on console.
 */
static void Help(char* argv0)
{
	printf("\n");
	printf("Call this software this way:");
	printf("\n");
	printf("%s [com_port [tcp_port [com_speed [flow_cntl]]]] [/v"
#ifdef __linux__
	       " | /l"
#endif
	       " | /L | /h" 
#ifdef __unix__
	       " | /d"
#endif
	       "]\n", argv0);
	printf("\n");
#ifdef _WIN32
	printf("     com_port   Integer value that identifies the RS232 port where LocoBuffer\n");
	printf("                is connected to (1=COM1, 2=COM2, ...). Default is '1' for COM1.\n");
	printf("                Alternatively, the name of the device can be passed as string.\n");
#endif
#ifdef __linux__
	printf("     com_port   String value that identifies the serial device to use.\n");
	printf("                Samples are: '/dev/ttyS0', '/dev/ttyUSB0' or '/dev/ttyAMA0'\n");
	printf("                Default is /dev/ttyS0. Try 'ls /dev/tty*' to find values.\n");
	printf("                Alternatively a number (n=0,1,..) for /dev/ttySn can be passed.\n");
#endif
	printf("     tcp_port   Integer value that specifies the TCP port number where\n");
	printf("                LbServer shell listen for connections. Default is 1234.\n");
	printf("     com_speed  Integer value that specifies the baud rate that is used on\n");
	printf("                the RS232 port, may be 19200, 38400, 57600 and 115200.\n");
	printf("                Default is 57600.\n");
	printf("     flow_cntl  Boolean value that enable or not hardware flow control (CTS).\n");
	printf("                true=CTS Flow Control enabled, false=CTS Flow Control disabled.\n");
	printf("                Default is true.\n");
	printf("     /v         Switch enables a lot of debug prints for COM port IO.\n");
#ifdef __linux__
	printf("     /l         Output information to LCD attached (/dev/lcd).\n");
#endif
	printf("     /L         Simulate LCD on console.\n");
#ifdef __unix__
	printf("     /d         Run as demon: Disable SIGHUP (but do not fork).\n");
#endif
	printf("     /h         Print this message and exit.\n");
	printf("\n");
	printf("For more information see LbServer manual on LoconetOverTcp project page:\n");
	printf("http://LoconetOverTcp.SourceForge.net\n");
}

static void HelpHint(char* argv0) {
	printf("Try '%s /h' for more information.\n", argv0);
}

#ifdef WITH_JOURNALD // init code required by JournalD

#ifdef __linux__
	const char* PROC_SELF_EXE = "/proc/self/exe";
#else
	// "/proc/curproc/file" on FreeBSD
	#warning "/proc/self/exe requires Linux"
	const char* PROC_SELF_EXE = nullptr;
#endif

/**
 * Try to get a string that can be used as "absolute file path" as argument
 * to journalctrl.
 * returns argv0 on error.
 */
static const char* getAbsoluteFilePath(const char* argv0) {
    if (PROC_SELF_EXE == nullptr) return argv0;
    char* buf = new char[PATH_MAX + 1];
    ssize_t nbytes = readlink(PROC_SELF_EXE, buf, PATH_MAX);
    if (nbytes == -1) {
        log_print( LOG_WARNING, "could not readlink(\"%s\") "
                   "errno=%i: %s", PROC_SELF_EXE, static_cast<int>(errno),
                   strerror(errno) );
        return argv0;
    }
    if (nbytes == PATH_MAX) {
        log_print( LOG_WARNING, "readlink(\"%s\") truncted", PROC_SELF_EXE );
        return argv0;
    }
    char* retVal = new char[nbytes + 1];
    snprintf(retVal, nbytes + 1, "%.*s", static_cast<int>(nbytes), buf);
    delete buf;
    return retVal;
}

#endif //WITH_JOURNALD

#ifdef _WIN32 // WSA startup and cleanup

static bool startupWSA() {
	WSADATA stWSAData;
	int nRet = WSAStartup(MAKEWORD(1,1), &stWSAData);
	/* WSAStartup() returns error value if failed (0 on success) */
	if (nRet != 0)
	{
		log_print( LOG_ERR, "WSAStartup() failed. nRet=%i", nRet );
		return false; /* No sense continuing if we can't use WinSock */
	}
	/*
	printf("WSADATA.wVersion       = %04Xh\n", stWSAData.wVersion);
	printf("WSADATA.wHighVersion   = %04Xh\n", stWSAData.wHighVersion);
	printf("WSADATA.szDescription  = %s\n",    stWSAData.szDescription);
	printf("WSADATA.szSystemStatus = %s\n",    stWSAData.szSystemStatus);
	printf("WSADATA.iMaxSockets    = %d\n",    stWSAData.iMaxSockets);
	printf("WSADATA.iMaxUdpDg      = %d\n",    stWSAData.iMaxUdpDg);
	printf("WSADATA.lpVendorInfo   = %lXh\n",  (unsigned long)stWSAData.lpVendorInfo);
	*/
	return true;
}

static bool cleanupWSA() {
	/*---------------release WinSock DLL--------------*/
	int nRet = WSACleanup();
	if (nRet == SOCKET_ERROR)
	{
		log_print( LOG_ERR, "WSACleanup() failed" );
		return false;
	}
	return true;
}

#endif

/**
 * Called on normal program exit and when program is terminated by signal or
 * exception. Should close tcp connections and logging and lcd device.
 * Also calls WSACleanup() on windows.
 */
static void cleanup() {
	//TODO: close tcp connections
	Display.DeleteAll();
	#ifdef _WIN32
		cleanupWSA();
	#endif
}

#ifdef __unix__ // Handling of unix signals

/**
 * Called when SIGABRT is received.
 */
void handleSigAbort( int sig ) {
	log_print_and_stderr( LOG_ERR, "Got signal SIGABRT(%i): '%s'. May be an "
	                      "assertion failed.", sig, strsignal(sig) );
	// we return, so the program will be aborted
}

/**
 * Called when SIGHUP, SIGINT or SIGTERM is received.
 */
void handleSigTerminate(int sig) {
	const char* name;
	int rc = 9;
	switch(sig) {
		case SIGHUP : name = "SIGHUP";  break;         //  1
		case SIGINT : name = "SIGINT";  break;         //  2
		case SIGABRT: name = "SIGABRT"; break;         //  6
		case SIGTERM: name = "SIGTERM"; rc = 0; break; // 15
		default     : name = "unknown"; break;
	}
	log_print_and_stderr( LOG_ERR, "Got signal %s(%i): '%s'. Terminating...",
	                      name, sig, strsignal(sig));
	cleanup();
	// give other threads some time to run before exit
	usleep(100*1000); // usleep seems to do the trick. Untested are: sched_yield() or pthread_yield()
	_exit( rc ); // TODO: _exit() or exit()
}

/**
 * Like signal(), but use new sigaction().
 * See Herold, Helmut; Linux,Unix; ISBN 9783827315120
 */
__sighandler_t mySignal(int signr, __sighandler_t sighandler) {
	struct sigaction newHandler, oldHandler;
	int rc;
	newHandler.sa_handler = sighandler;
	sigemptyset(&newHandler.sa_mask);
	newHandler.sa_flags = 0;
	rc = sigaddset( &newHandler.sa_mask, SIGABRT );
	if ( rc == -1 ) {
		log_print_and_stderr( LOG_ERR, "mySignal(%i): sigaddset(SIGABRT) "
		                     "failed. errno=%i: %s", signr, errno,
		                      strerror(errno) );
	}
	rc = sigaddset( &newHandler.sa_mask, SIGHUP );
	if ( rc == -1 ) {
		log_print_and_stderr( LOG_ERR, "mySignal(%i): sigaddset(SIGHUP) "
		                      "failed. errno=%i: %s", signr, errno,
		                      strerror(errno) );
	}
	rc = sigaddset( &newHandler.sa_mask, SIGINT );
	if ( rc == -1 ) {
		log_print_and_stderr( LOG_ERR, "mySignal(%i): sigaddset(SIGINT) "
		                      "failed.  errno=%i: %s", signr, errno,
		                      strerror(errno) );
	}
	rc = sigaddset( &newHandler.sa_mask, SIGTERM );
	if ( rc == -1 ) {
		log_print_and_stderr( LOG_ERR, "mySignal(%i): sigaddset(SIGTERM) "
		                     "failed. errno=%i: %s", signr, errno,
		                      strerror(errno) );
	}
	rc = sigaction( signr, &newHandler, &oldHandler);
	if ( rc == -1 ) {
		log_print_and_stderr( LOG_ERR, "mySignal(%i): sigaction(SIGTERM) "
		                      "failed. errno=%i: %s", signr, errno,
		                      strerror(errno) );
		return SIG_ERR;
	}
	return oldHandler.sa_handler;
}

static void initSignals(bool deamon) {
	if ( mySignal(SIGABRT, handleSigAbort) == SIG_ERR ) {
		log_print_and_stderr( LOG_ERR, "Failed to install signal handler for "
		                      "SIGABRT (%i)", SIGABRT );
	}
	if (deamon) {
		// when running as demon: ignore signal
		if ( mySignal(SIGHUP, SIG_IGN) == SIG_ERR ) {
			log_print_and_stderr( LOG_ERR, "Failed to install signal handler for "
			                      "SIGHUP (%i)", SIGHUP );
		}
	} else {
		// when bound to terminal: abort
		if ( mySignal(SIGHUP, handleSigTerminate) == SIG_ERR ) {
			log_print_and_stderr( LOG_ERR, "Failed to install signal handler for "
			                      "SIGHUP (%i)", SIGHUP );
		}
	}
	if ( mySignal(SIGINT, handleSigTerminate) == SIG_ERR ) {
		log_print_and_stderr( LOG_ERR, "Failed to install signal handler for "
		                      "SIGINT (%i)", SIGINT );
	}
	if ( mySignal(SIGTERM, handleSigTerminate) == SIG_ERR ) {
		log_print_and_stderr( LOG_ERR, "Failed to install signal handler for "
		                      "SIGTERM (%i)", SIGTERM );
	}
}

#endif //__unix__

#ifdef _WIN32

// https://learn.microsoft.com/en-gb/windows/console/registering-a-control-handler-function
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
		log_print_and_stderr(LOG_WARNING, "Ctrl-C event\n\n");
		break;

	case CTRL_CLOSE_EVENT:
		log_print_and_stderr(LOG_WARNING, "Ctrl-Close event\n\n");
		break;

	case CTRL_BREAK_EVENT:
		log_print_and_stderr(LOG_WARNING, "Ctrl-Break event\n\n");
		break;

	case CTRL_LOGOFF_EVENT:
		log_print_and_stderr(LOG_WARNING, "Ctrl-Logoff event\n\n");
		break;

	case CTRL_SHUTDOWN_EVENT:
		log_print_and_stderr(LOG_WARNING, "Ctrl-Shutdown event\n\n");
		break;

	default:
		log_print_and_stderr(LOG_WARNING, "Unknown system event: %d\n\n", fdwCtrlType);
		break;
	}
	cleanup();
	// give other threads some time to run before exit
	Sleep(100); // usleep seems to do the trick. Untested are: sched_yield() or pthread_yield()
	_exit( 9 ); // TODO: _exit() or exit()
	return TRUE;
}

static void initSignals()
{
	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
	{
		log_print_and_stderr(LOG_ERR, "Failed to use SetConsoleCtrlHandler()");
	}
}

#endif // _WIN32


#ifdef __linux__

static bool isLink(const char* fn) {
	struct stat64 buf;
	int rc = lstat64( fn, &buf );
	if (rc == -1) {
		log_print_and_stderr( LOG_ERR, "isLink(%s) failed. errno=%i: %s", fn,
		                      errno, strerror(errno) );
	}
	return S_ISLNK(buf.st_mode);
}

#endif

/**
 * Open serial port and start listening on socket.
 */
static int Run(CLbServerConfig& cfg) {
	CLocoBuffer clLoconet;
	CLoconetServer clServer(clLoconet);

	if (!clLoconet.Open(cfg.serialPort.c_str(), cfg.comSpeed, cfg.ctsFlowControl))
	{
		Display.ErrorMessageF(LOG_ERR, "Could not open serial port %s!", cfg.serialPort.c_str());
		return 2;
	}
	char* realPort = NULL;
	#ifdef __linux__ // resolve any link
		if ( isLink(cfg.serialPort.c_str()) ) {
			realPort = realpath(cfg.serialPort.c_str(), NULL);
			if (realPort == NULL) {
				log_print_and_stderr( LOG_ERR, "realpath(%s) failed. errno=%i: %s",
				                      cfg.serialPort.c_str(), errno, strerror(errno) );
				free(realPort);
				realPort = NULL;
			}
		}
	#endif
	if (realPort==NULL) {
		Display.ErrorMessageF(LOG_NOTICE, "Loconet device port opened on %s "
		                      "with %d baud and %s CTS flow control.",
		                      cfg.serialPort.c_str(), cfg.comSpeed,
		                      cfg.ctsFlowControl?"with":"without");
	} else {
		Display.ErrorMessageF(LOG_NOTICE, "Loconet device port opened on %s "
		                      "-> %s with %d baud and %s CTS flow control.",
		                      cfg.serialPort.c_str(), realPort, cfg.comSpeed,
		                      cfg.ctsFlowControl?"with":"without");
	}
	if (!clServer.StartListenThread(cfg.tcpPort))
	{
		Display.ErrorMessageF(LOG_ERR, "Could not start TCP server!");
		return 3;
	}
	Display.ErrorMessageF(LOG_NOTICE, "TCP server started on port %d.", cfg.tcpPort);

	if (cfg.displayNormal) {
		printf("Output uses single characters for increased performance:\n");
		printf("R=received packet, .=nothing received but event was signaled :-(\n");
		printf("s=send request, S=sent successfully, e=send error\n");
		printf("C=client connected, D=client disconnected\n");
	}

	clServer.Run();
	return 0;
}

int main(int argc, char* argv[])
{
	const char* progName = argv[0]; // name required by journalctl to filter
	#ifdef WITH_JOURNALD
		progName = getAbsoluteFilePath( argv[0] );
		log_print( LOG_NOTICE, "LocoBuffer Server (%s) version %s, for "
		           "protocol version %s, built %s %s",
		           progName, VERSION, PROTOCOL, __DATE__, __TIME__ );
		for (int i = 1; i < argc; i++) log_print( LOG_DEBUG, "argv[%i]=\"%s\"", i, argv[i] );
	#elif defined(__linux__)
		#warning "compiling on Linux without WITH_JOURNALD"
	#endif
	try {
		if( setvbuf( stdout, NULL, _IONBF, 0 ) != 0 )
		{
			log_print( LOG_ERR, "Could not disable buffering on stdout "
			           "(errno=%i: %s)", errno, strerror(errno) );
		}

		printf("LocoBuffer Server version %s, for protocol version %s, built %s,\n", VERSION, PROTOCOL, __DATE__);
		#ifdef _DEBUG
			printf("DEBUG version,\n");
		#endif
		printf("released under GPL by Stefan Bormann, Martin Pischky and others.\n");
		#ifdef WITH_JOURNALD
			printf("\n");
			printf("Try 'journalctl -f %s' to see logging information.\n", progName);
		#endif

		#ifdef _WIN32
			if( !startupWSA() ) return 91;
		#endif

		CLbServerConfig cfg;
		bool ok = cfg.parseFromFile(progName)
				  && cfg.parseFromArguments(argc, argv)
				  && cfg.check();
		if ( cfg.help ) {
			// user has requested help by "/h"
			Help( argv[0] );
			return 1;
		} else if ( !ok ) {
			// some parameters are invalid
			HelpHint( argv[0] );
			return 1;
		}

		#ifdef WITH_JOURNALD
			if (cfg.displayJournal) Display.Add(new DisplayJournal());
		#endif
		if (cfg.displayNormal) Display.Add(new CDisplayNormal());
		if (cfg.displayVerbose) Display.Add(new CDisplayVerbose());
		if (cfg.displayLcdSimul) Display.Add(new CDisplayLcdSimul());
		#ifdef __linux__
			if (cfg.displayLcdPiLocoBuffer) {
				FILE *lcd = CDisplayLcdPiLocoBuffer::openLcdDevice("/dev/lcd");
				Display.Add(new CDisplayLcdPiLocoBuffer(lcd));
			}
		#endif

		#ifdef __unix__
			initSignals(cfg.daemon);
		#endif
		#ifdef _WIN32
			initSignals();
		#endif

		int rc = Run(cfg);

		cleanup();

		printf("\n"); // "." removed
		#ifdef WITH_JOURNALD
			log_print( LOG_NOTICE, "LocoBuffer Server (%s) normal "
			           "termination (rc=%i)", progName, rc );
		#endif
		return rc;
	} catch(exception& ex) {
		cleanup();
		log_print_and_stderr( LOG_ERR, "LocoBuffer Server (%s) terminated by "
		                      "exception: %s", progName, ex.what() );
		return 92;
	} catch(...) {
		cleanup();
		log_print_and_stderr( LOG_ERR, "LocoBuffer Server (%s) terminated by "
		                      "unknown exception", progName );
		return 93;
	}
}

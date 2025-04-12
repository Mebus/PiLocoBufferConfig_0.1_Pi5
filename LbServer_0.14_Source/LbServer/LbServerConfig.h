/*
 * LbServerConfig.h
 * $Id: LbServerConfig.h 1231 2023-02-16 14:24:42Z pischky $
 *
 *  Created on: 10.02.2023
 *      Author: Martin Pischky (martin@pischky.de)
 */

#ifndef _LBSERVER_CONFIG_H_
#define _LBSERVER_CONFIG_H_

#include <string> // class std::string

using std::string;

class CLbServerConfig {
public:
	string serialPort;
	int    tcpPort;
	int    comSpeed;
	bool   ctsFlowControl;
	bool   daemon;
	bool   displayVerbose;
	bool   displayNormal;
	bool   displayLcdPiLocoBuffer;
	bool   displayLcdSimul;
	bool   displayJournal;
	bool   help; // no config value but a commandline switch
public:
	/**
	 * Standard constructor.
	 * Sets default values when no file is read and no commandline arguments
	 * are set.
	 */
	CLbServerConfig() 
	#ifdef _WIN32
		: serialPort("1") // COM1
	#elif defined(__linux__)
		: serialPort("/dev/ttyS0")
	#else
		#error "unkown os"
	#endif
	,   tcpPort(1234)
	,   comSpeed(57600)
	,   ctsFlowControl(true)
	,   daemon(false)
	,   displayVerbose(false)
	,   displayNormal(false)
	,   displayLcdPiLocoBuffer(false)
	,   displayLcdSimul(false)
	#ifdef WITH_JOURNALD
		, displayJournal(true)
	#else
		, displayJournal(false)
	#endif
	,   help(false)
	{
	}
	/**
	 * Destructor.
	 */
	virtual ~CLbServerConfig() {
	}
	/**
	 * Read configuration file.
	 * @param progName "/etc/LbServer.conf" or "./LbServer.exe.config"
	 * @return true: successful
	 */
	bool parseFromFile( const char* progName );
	/**
	 * Parse arguments passed to main().
	 * @param argc pass first parameter of main
	 * @param argv pass second parameter of main
	 * @return true: successful
	 */
	bool parseFromArguments( int argc, char* argv[] );
	/**
	 * Check that config is valid:
	 *  - serialPort is an exiting device name
	 *  - ...
	 * @return true: successful
	 */
	bool check();
private:
	/**
	 * Check that x is "false", "FALSE", "true" or "TRUE".
	 * Sets v, when valid, otherwise v is unchanged.
	 * @param x the string to parse
	 * @param v the value to set (only updated on success)
	 * @return true when x is valid, false otherwise.
	 */
	static bool pBool( string x, bool& v );
	/*
	 * Parse string x to an integer.
	 * The value of x needs to be min <= x <= max.
	 * Sets v, when valid, otherwise v is unchanged.
	 * @param x the string to parse
	 * @param v the value to set (only updated on success)
	 * @param min the minimum accepted value (0..)
	 * @param max the maximum accepted value (0..)
	 * @return true when x is valid, false otherwise.
	 */
	static bool pInt( string x, int& v, unsigned int min, unsigned int max);

public: // enable use in unit test
	static bool parseLine( string line, string& key, string& val);
};

#endif /* _LBSERVER_CONFIG_H_ */

/*
 * LbServerConfig.cpp
 * $Id: LbServerConfig.cpp 1231 2023-02-16 14:24:42Z pischky $
 *
 *  Created on: 10.02.2023
 *      Author: Martin Pischky (martin@pischky.de)
 */


#include <cctype>    // isdigit(), isgraph(), isalnum(), isblank()
#include <cerrno>    // errno
#include <climits>   // INT_MAX
#include <cstdio>    // fopen()
#include <cstdlib>   // strtoul()
#include <fstream>   // std::ifstream
#include <stdexcept> // invalid_argument
#include <string>    // std::string, std::getline
#include "log.h"
#ifdef __linux__
    #include <unistd.h> // access(), F_OK, W_OK, R_OK
#elif defined(_WIN32)
    #include <windows.h> //GetFileAttributesA(), LPCSTR, DWORD, 
                         //INVALID_FILE_ATTRIBUTES, FILE_ATTRIBUTE_DIRECTORY
#endif
#include "LbServerConfig.h"

using std::string;
using std::ifstream;
using std::getline;

bool CLbServerConfig::parseFromArguments( int argc, char* argv[] ) {
	string* args = new string[argc];
	// string args[argc]; //on MSC: error C2057: expected constant expression
	                      //        error C2466: cannot allocate an array of constant size 0
	                      //        error C2133: 'args' : unknown size
	for(int i = 0; i < argc; i++) args[i] = argv[i];
	#ifdef WITH_JOURNALD
		this->displayJournal = true;
	#endif
	// parse switches
	int idx = 1;
	while(idx<argc) {
		string a = args[idx]; // parameter may be "/d", "/v", "/n", "/l", "/L" or "/h"
		if (a.size() == 2 && (a[0] == '/' ||  a[0] == '-')) {
			switch (a[1]) {
				#ifdef __unix__
					case 'd':
					case 'D':
						this->daemon = true;
						break;
				#endif
				case 'v':
				case 'V':
					this->displayVerbose = true;
					break;
				case 'n':
				case 'N':
					this->displayNormal = true;
					break;
				#ifdef __linux__
					case 'l':
						this->displayLcdPiLocoBuffer = true;
						break;
				#endif
				case 'L':
					this->displayLcdSimul = true;
					break;
				case 'h': // print help and exit
				case 'H':
				case '?':
					help = true;
					//delete[] args;
					//return false; // abort parsing
					break;
				default:
					log_print_and_stderr(LOG_ERR, "Unknown switch ignored: \"%s\"\n", a.c_str());
					//throw std::invalid_argument( "Unknown switch ignored" );
					break;
			}
			for(int i = idx; i < argc-1; ++i) args[i] = args[i+1];
			argc--;
		} else {
			++idx; // skip none switch
		}
	}
	// parse positional arguments
	if (argc >= 2) {
		serialPort = args[1];
	}
	if (argc >= 3) {
		if ( !pInt(args[2], this->tcpPort, 1, 65535) ) {
			log_print_and_stderr( LOG_ERR, "tcp_port is invalid. Valid values "
			                      "are from 1 to 65535 (argv[2]=\"%s\")",
			                      args[2].c_str() );
			//throw std::invalid_argument( "tcp_port is invalid" );
			delete[] args;
			return false;
		}
	}
	if (argc >= 4) {
		int speed = 0;
		if ( ! ( pInt(args[3], speed, 19200, 115200)
				 && ( speed == 19200 || speed == 38400
					  || speed == 57600 || speed == 115200) ) )
		{
			log_print_and_stderr( LOG_ERR, "com_speed should be 19200, 38400, "
			                      "57600 or 115200 (argv[3]=\"%s\")",
			                      args[3].c_str() );
			//throw std::invalid_argument( "com_speed should be true or false" );
			delete[] args;
			return false;
		}
		this->comSpeed = speed;
	}
	if (argc >= 5) {
		if ( !pBool(args[4], this->ctsFlowControl) ) {
			log_print_and_stderr( LOG_ERR, "flow_cntl should be true or false "
			                      "(argv[4]=\"%s\")", args[4].c_str() );
			//throw std::invalid_argument( "flow_cntl should be true or false" );
			delete[] args;
			return false;
		}
	}
	if (argc >= 6) {
		log_print_and_stderr( LOG_ERR, "to many arguments" );
		//throw std::invalid_argument( "to many args" );
		delete[] args;
		return false;
	}
	delete[] args;
	return true;
}

bool CLbServerConfig::parseFromFile(const char* progName) {
	#ifdef _WIN32
		string fileName = progName;
		fileName += ".conf"; // Windows does not like ".config"
		DWORD attrib = GetFileAttributesA( fileName.c_str() );
		bool exists = (attrib != INVALID_FILE_ATTRIBUTES &&
		               !(attrib & FILE_ATTRIBUTE_DIRECTORY));
	#elif defined(__linux__)
		string fileName = "/etc/lbserver.conf";
		bool exists = true;
		int rc = access(fileName.c_str(), F_OK); // test on files exists
		if (rc != 0) exists = false;
		rc = access(fileName.c_str(), R_OK); // test on read access
		if (rc != 0) exists = false;
	#else
		#error "unknown os"
	#endif
	if (!exists) {
		log_print_and_stderr( LOG_WARNING, "Could not read %s. Using defaults.", fileName.c_str() );
		return true; // !!!
	}
	ifstream file( fileName.c_str(), ifstream::in );
	if ( !file.good() ) {
		log_print_and_stderr( LOG_ERR, "CLbServerConfig::parseFromFile(%s) failed", progName );
		return false;
	}
	log_print_and_stderr( LOG_NOTICE, "Reading configuration from %s.", fileName.c_str() );
	string line;
	string key;
	string val;
	do {
	    getline(file, line);
	    if (parseLine(line, key, val)) {
			if (key == "com_port") {
				serialPort = val;
			} else if (key == "tcp_port") {
				if ( !pInt(val, this->tcpPort, 1, 65535) ) {
					log_print_and_stderr( LOG_ERR, "tcp_port is invalid. Valid values "
										  "are from 1 to 65535 (val=\"%s\")",
										  val.c_str() );
					return false;
				}
			} else if (key == "com_speed") {
				int speed = 0;
				if ( ! ( pInt(val, speed, 19200, 115200)
						 && ( speed == 19200 || speed == 38400
							  || speed == 57600 || speed == 115200) ) )
				{
					log_print_and_stderr( LOG_ERR, "com_speed should be 19200, 38400, "
					                      "57600 or 115200 (val=\"%s\")",
										  val.c_str() );
					return false;
				}
				this->comSpeed = speed;
			} else if (key == "flow_cntl") {
				if ( !pBool(val, this->ctsFlowControl) ) {
					log_print_and_stderr( LOG_ERR, "flow_cntl should be true or false "
					                      "(val=\"%s\")", val.c_str() );
					return false;
				}
			} else if (key == "displayJournal") {
				if ( !pBool(val, this->displayJournal) ) {
					log_print_and_stderr( LOG_ERR, "displayJournal should be true or false "
					                      "(val=\"%s\")", val.c_str() );
					return false;
				}
			} else if (key == "displayNormal") {
				if ( !pBool(val, this->displayNormal) ) {
					log_print_and_stderr( LOG_ERR, "displayNormal should be true or false "
					                      "(val=\"%s\")", val.c_str() );
					return false;
				}
			} else if (key == "displayVerbose") {
				if ( !pBool(val, this->displayVerbose) ) {
					log_print_and_stderr( LOG_ERR, "displayVerbose should be true or false "
					                      "(val=\"%s\")", val.c_str() );
					return false;
				}
			} else if (key == "displayLcdSimul") {
				if ( !pBool(val, this->displayLcdSimul) ) {
					log_print_and_stderr( LOG_ERR, "displayLcdSimul should be true or false "
					                      "(val=\"%s\")", val.c_str() );
					return false;
				}
			} else if (key == "displayLcdPiLocoBuffer") {
				if ( !pBool(val, this->displayLcdPiLocoBuffer) ) {
					log_print_and_stderr( LOG_ERR, "displayLcdPiLocoBuffer should be true or false "
					                      "(val=\"%s\")", val.c_str() );
					return false;
				}
			} else {
				log_print_and_stderr( LOG_ERR, "Unknown key (\"%s\") in file.",
									  key.c_str() );
				return false;
			}
	    	if (file.eof()) break;
	    }
	} while( !file.eof() );
	file.close();
	return true;
}

bool CLbServerConfig::check() {
	if (!displayVerbose && !displayLcdSimul && !displayLcdPiLocoBuffer) {
		displayNormal = true;
	}

	#ifdef _WIN32
		const int minComPort = 1; // "COM1" is the first
	#elif defined(__linux__)
		const int minComPort = 0; // "/dev/ttyS0" is the first
	#else
		#error "unknown os"
	#endif
	int portNumber;
	if ( pInt(serialPort, portNumber, minComPort, 256)) {
		// numeric value passed: convert to device name
		char temp[16];
		#ifdef _WIN32
			sprintf(temp, "\\\\.\\COM%d", portNumber);
		#else //__linux__
			snprintf(temp, sizeof(temp), "/dev/ttyS%d", portNumber);
		#endif
		serialPort = temp;
	} else {
		// other value passed: leave serialPort as before
	}

	#ifdef _WIN32
		// not needed, but we can added someting like:
		//		TCHAR targetPath[1000];
		//		WORD rc = QueryDosDevice( serialPort.c_str(), targetPath, 1000)
		//		if (rc==0) {
		//			// fail. See GetLastError()
		//		} else {
		//			// success
		//		}
	#elif defined(__linux__)
		int rc;
		rc = access(serialPort.c_str(), F_OK); // test on files exists
		if (rc != 0) {
			log_print_and_stderr( LOG_ERR, "Serial port does not exists: %s", serialPort.c_str() );
			//throw std::invalid_argument( "serial port does not exists" );
			return false;
		}
		rc = access(serialPort.c_str(), W_OK|R_OK); // test on write and read access
		if (rc != 0) {
			log_print_and_stderr( LOG_ERR, "Permission denied to serial port: %s. "
								  "Try 'chmod a+rw %s' as root.", serialPort.c_str(), serialPort.c_str() );
			//throw std::invalid_argument( "Permission denied to serial port" );
			return false;
		}
	#else
		#error "unknown os"
	#endif

	return true;
}

bool CLbServerConfig::pBool( string x, bool& v) {
	if (x == "true" || x == "TRUE" ) {
		v = true;
		return true;
	}
	if (x == "false" || x == "FALSE" ) {
		v = false;
		return true;
	}
	return false;
}

bool CLbServerConfig::pInt( string x, int& v, unsigned int min, unsigned int max) {
	for( string::size_type i = 0; i < x.size(); ++i ) {
		if( !isdigit(x[i]) ) return false;
	}
	char* end = NULL;
	errno = 0;
	unsigned long y = strtoul(x.c_str(), &end, 10);
	if (errno != 0) return false;
	if (y == 0) {
        // error or really 0 ?
        for( string::size_type i = 0; i < x.size(); ++i ) {
            if( x[i] != '0' ) return false;
        }
	}
	if (y > INT_MAX) return false;
	if (y < min) return false;
	if (y > max) return false;
	v = static_cast<int>(y);
	return true;
}

#ifndef isblank
    static int isblank( char x ) {
        return x == ' ' || x == '\t';
    }
#endif

bool CLbServerConfig::parseLine( string line, string& key, string& val) {
	// " key = value # comment "
	key = "";
	val = "";
	string::size_type idx = 0;

	while ( idx < line.size() && isblank(line[idx]) ) ++idx;
	if ( idx >= line.size() ) return false;
	if ( line[idx] == '#' ) return false;

	string k = "";
	while ( idx < line.size() && (isalnum(line[idx]) || line[idx]=='_') ) {
		k += line[idx];
		idx++;
	}
	if ( idx >= line.size() ) return false;
	if ( line[idx] == '#' ) return false;

	while ( idx < line.size() && isblank(line[idx]) ) ++idx;
	if ( idx >= line.size() ) return false;
	if ( line[idx] == '#' ) return false;

	if ( line[idx] == '=' ) {
	    idx++;
	} else {
	    return false;
	}

	while ( idx < line.size() && isblank(line[idx]) ) ++idx;
	if ( idx >= line.size() ) return false;
	if ( line[idx] == '#' ) return false;

	string v = "";
	while ( idx < line.size() && isgraph(line[idx]) && line[idx] != '#') {
		v += line[idx];
		idx++;
	}

	while ( idx < line.size() && isblank(line[idx]) ) ++idx;
	if ( idx < line.size() && line[idx] != '#' ) return false;

	if (k == "") return false;
	if (v == "") return false;

	key = k;
	val = v;
	return true;
}



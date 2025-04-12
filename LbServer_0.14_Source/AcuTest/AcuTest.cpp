// AcuTest.cpp : Defines the entry point for the console application.
//

#include <string>       /* class std::string */
#include <stdlib.h>     /* atexit() */
#include "acutest.h"    /* TEST_CHECK, TEST_CASE, TEST_MSG, TEST_LIST */
#include "LbServerConfig.h" // UUT: CLbServerConfig
#include "LcdTopics.h"      // UUT: CCounter, CTxRx
#include "log.h"            // log_max_priority

using std::string;

#ifndef WITH_JOURNALD
	int log_max_priority = LOG_DEBUG; // define log level (see: "log.h") when journald is not used
#endif

void TestConnect(void)
{
	CConnect uut;
	TEST_CHECK(0==strcmp(uut.Get(), "  Connections: 0"));
	uut.Connected(12);
	TEST_CHECK(0==strcmp(uut.Get(), "   Connected: 12"));
	uut.Connected(12345);
	TEST_CHECK(0==strcmp(uut.Get(), "Connected: 12345"));
	uut.Connected(123456);
	TEST_CHECK(0==strcmp(uut.Get(), "Connected:123456"));
	uut.Disconnected(321);
	TEST_CHECK(0==strcmp(uut.Get(), "Disconnected:321"));
	uut.Disconnected(654321);
	TEST_CHECK(0==strcmp(uut.Get(), "Disconnect654321"));
	uut.Disconnected(4242424242);
	TEST_CHECK(0==strcmp(uut.Get(), "Discon4242424242"));
}



class TestingTxRx : public CTxRx // inheritance for access to protected m_Rx/m_Tx
{
public:
	void Test(unsigned int tx, unsigned int rx, const char* pcExpected)
	{
		m_Rx = rx;
		m_Tx = tx;
		TEST_CASE(pcExpected);
		TEST_CHECK(0==strcmp(pcExpected, Get()));
		TEST_MSG("Get()=[%s]", Get());
	}
};

void TestTxRx(void)
{
	TestingTxRx uut;
	uut.Test(0, 0,               "TX: 0      RX: 0");
	uut.Test(0, 12,              "TX: 0     RX: 12");
	uut.Test(0, 123,             "TX: 0    RX: 123");
	uut.Test(0, 1234,            "TX: 0   RX: 1234");
	uut.Test(0, 12345,           "TX: 0  RX: 12345");
	uut.Test(12, 123456,         "TX:12  RX:123456");
	uut.Test(123, 123456,        "TX:123 RX:123456");
	uut.Test(1234, 123456,       "TX1234  RX123456");
	uut.Test(12345, 123456,      "TX12345 RX123456");
	uut.Test(123456, 123456,     "T123456  R123456");
	uut.Test(123456, 1234567,    "T123456 R1234567");
	uut.Test(123456, 12345678,   "RX: 12345678    ");
	uut.Test(9, 12345678,        "TX:9 RX:12345678");
	uut.Test(123456, 1234567890, "RX: 1234567890  ");
}



#define TestSetValue(value, expected) { TEST_CASE(#value); uut.SetValue(value); TEST_CHECK_(0==strcmp(uut.Get(), expected), uut.Get()); }

void TestCounter(void)
{
	CCounter uut("ElevenChars");
	TestSetValue(0,          "  ElevenChars: 0");
	TestSetValue(42,         " ElevenChars: 42");
	TestSetValue(123,        "ElevenChars: 123");
	TestSetValue(1234,       "ElevenChars:1234");
	TestSetValue(12345,      "ElevenChars12345");
	TestSetValue(123456,     "ElevenChar123456");
	TestSetValue(1234567,    "ElevenCha1234567");
	TestSetValue(123456789,  "ElevenC123456789");
	TestSetValue(1234567890, "Eleven1234567890");
}



void TestMystring(void)
{
	string s("abcdefghij");
	// printf( "%s\n", s.c_str() );
	TEST_CHECK( 0==strcmp("abcdefghij", s.c_str()) );
	s += 'k';
	s += 'l';
	s += 'm';
	s += 'n';
	// printf( "%s\n", s.c_str() );
	TEST_CHECK( 0==strcmp("abcdefghijklmn", s.c_str()) );
	TEST_CHECK( "abcdefghijklmn" == s );
	TEST_CHECK( "ABCDEFGHIJKLMN" != s );
	string s2 = "Hell";
	s2 += " and high water";
	// printf( "%s\n", s2.c_str() );
	TEST_CHECK( 0==strcmp("Hell and high water", s2.c_str()) );
	string s3 = "qwerty";
	s3 = s3; // @suppress("Assignment to itself")
	TEST_CHECK( 0==strcmp("qwerty", s3.c_str()) );
	string s4 ="the quick brown fox jumped over the lazy dog";
	s4 = s4; // @suppress("Assignment to itself")
	TEST_CHECK( 0==strcmp("the quick brown fox jumped over the lazy dog", s4.c_str()) );
	// printf( "%s %s\n", s3.c_str(), s4.c_str() );
	string s5 = s + ". " + s3 + string(". ") + "Horsefeathers\n";
	// printf( "%s", s5.c_str() );
	TEST_CHECK( 0==strcmp("abcdefghijklmn. qwerty. Horsefeathers\n", s5.c_str()) );
	string s6;
	s6 = "1234567890";
	// printf( "%s %i %i\n", s6.c_str(), s6.size(), s6.capacity() );
	TEST_CHECK( 0==strcmp("1234567890", s6.c_str()) );
	TEST_CHECK( 10 == s6.size() );
	TEST_CHECK( 15 <= s6.capacity() );
	s6 = "123456789012345678901234567890";
	// printf( "%s %i %i\n", s6.c_str(), s6.size(), s6.capacity() );
	TEST_CHECK( 0==strcmp("123456789012345678901234567890", s6.c_str()) );
	TEST_CHECK( 30 == s6.size() );
	TEST_CHECK( 30 <= s6.capacity() );
}

void TestLbServerConfigDefaults(void) 
{
    CLbServerConfig cfg;
    #ifdef _WIN32
        TEST_CHECK( cfg.serialPort == "1" );
	#elif defined(__linux__)
        TEST_CHECK( cfg.serialPort == "/dev/ttyS0" );
    #else
        #error "unknown os"
	#endif
	TEST_CHECK( cfg.tcpPort == 1234 );
	TEST_CHECK( cfg.comSpeed == 57600 );
	TEST_CHECK( cfg.ctsFlowControl == true );
	TEST_CHECK( cfg.daemon == false );
	TEST_CHECK( cfg.displayVerbose == false );
	TEST_CHECK( cfg.displayNormal == false );
	TEST_CHECK( cfg.displayLcdPiLocoBuffer == false );
	TEST_CHECK( cfg.displayLcdSimul == false );
	#ifdef WITH_JOURNALD
		TEST_CHECK( cfg.displayJournal == true );
	#else
		TEST_CHECK( cfg.displayJournal == false );
	#endif
	TEST_CHECK( cfg.help == false );
}

void TestLbServerConfigParseFromArguments(void) 
{
    {
        CLbServerConfig cfg1;
        char* argv1[1] = { "LbServer.exe" };
        cfg1.parseFromArguments( 1, argv1 );
        #ifdef _WIN32
            TEST_CHECK( cfg1.serialPort == "1" );
	    #elif defined (__linux__)
            TEST_CHECK( cfg1.serialPort == "/dev/ttyS0" );
        #else
            #error "unknown os"
	    #endif
	    TEST_CHECK( cfg1.tcpPort == 1234 );
	    TEST_CHECK( cfg1.comSpeed == 57600 );
	    TEST_CHECK( cfg1.ctsFlowControl == true );
	    TEST_CHECK( cfg1.daemon == false );
	    TEST_CHECK( cfg1.displayVerbose == false );
	    TEST_CHECK( cfg1.displayNormal == false );
	    TEST_CHECK( cfg1.displayLcdPiLocoBuffer == false );
	    TEST_CHECK( cfg1.displayLcdSimul == false );
	    #ifdef WITH_JOURNALD
		    TEST_CHECK( cfg1.displayJournal == true );
	    #else
		    TEST_CHECK( cfg1.displayJournal == false );
	    #endif
	    TEST_CHECK( cfg1.help == false );
	}
	{
        CLbServerConfig cfg2;
        char* argv2[8] = { "LbServer.exe", "-h", "/dev/ttyX", "2345", "115200", "false", "-v", "-L" };
        cfg2.parseFromArguments( 8, argv2 );
        TEST_CHECK( cfg2.serialPort == "/dev/ttyX" );
	    TEST_CHECK( cfg2.tcpPort == 2345 );
	    TEST_CHECK( cfg2.comSpeed == 115200 );
	    TEST_CHECK( cfg2.ctsFlowControl == false );
	    TEST_CHECK( cfg2.daemon == false );
	    TEST_CHECK( cfg2.displayVerbose == true );
	    TEST_CHECK( cfg2.displayNormal == false );
	    TEST_CHECK( cfg2.displayLcdPiLocoBuffer == false );
	    TEST_CHECK( cfg2.displayLcdSimul == true );
	    #ifdef WITH_JOURNALD
		    TEST_CHECK( cfg2.displayJournal == true );
	    #else
		    TEST_CHECK( cfg2.displayJournal == false );
	    #endif
	    TEST_CHECK( cfg2.help == true );
	}	
	{
	    CLbServerConfig cfg3;
        char* argv3[2] = { "LbServer.exe", "/dev/ttyS9" };
        cfg3.parseFromArguments( 2, argv3 );
        TEST_CHECK( cfg3.serialPort == "/dev/ttyS9" );
    }
	{
	    CLbServerConfig cfg4;
        char* argv4[2] = { "LbServer.exe", "1" };
        cfg4.parseFromArguments( 2, argv4 );
        #ifdef _WIN32
            TEST_CHECK( cfg4.serialPort == "1" ); // check will set: "\\\\.\\COM1"
	    #elif defined(__linux__)
            TEST_CHECK( cfg4.serialPort == "1" ); // check will set: "/dev/ttyS1"
        #else
            #error "unknown os"
	    #endif
    }
}

void TestLbServerConfigParseLine(void) {
    string key;
    string val;
    bool res = CLbServerConfig::parseLine( "", key, val );
    TEST_CHECK( res == false );
    TEST_CHECK( key == "" );
    TEST_CHECK( val == "" );
    
    res = CLbServerConfig::parseLine( "#", key, val );
    TEST_CHECK( res == false );
    TEST_CHECK( key == "" );
    TEST_CHECK( val == "" );
    
    res = CLbServerConfig::parseLine( " # dgd#\\-?..234234.", key, val );
    TEST_CHECK( res == false );
    TEST_CHECK( key == "" );
    TEST_CHECK( val == "" );
    
    res = CLbServerConfig::parseLine( " key # comment", key, val );
    TEST_CHECK( res == false );
    TEST_CHECK( key == "" );
    TEST_CHECK( val == "" );
    
    res = CLbServerConfig::parseLine( " key val # comment", key, val );
    TEST_CHECK( res == false );
    TEST_CHECK( key == "" );
    TEST_CHECK( val == "" );

    res = CLbServerConfig::parseLine( " key = \\val/../+_$ # comment", key, val );
    TEST_CHECK( res == true );
    TEST_CHECK( key == "key" );
    TEST_CHECK( val == "\\val/../+_$" );

    res = CLbServerConfig::parseLine( " KeY_2 = /dev/tty # comment", key, val );
    TEST_CHECK( res == true );
    TEST_CHECK( key == "KeY_2" );
    TEST_CHECK( val == "/dev/tty" );

    res = CLbServerConfig::parseLine( " key = val  ", key, val );
    TEST_CHECK( res == true );
    TEST_CHECK( key == "key" );
    TEST_CHECK( val == "val" );
    
    res = CLbServerConfig::parseLine( "key=val", key, val );
    TEST_CHECK( res == true );
    TEST_CHECK( key == "key" );
    TEST_CHECK( val == "val" );
    
    res = CLbServerConfig::parseLine( "tcp_port=2345", key, val );
    TEST_CHECK( res == true );
    TEST_CHECK( key == "tcp_port" );
    TEST_CHECK( val == "2345" );    
}


void brrr(void)
{
	printf("Press <enter> key\n");
	getchar();
}

void ScheduleAtExit(void)
{
	atexit(brrr);
}


TEST_LIST = 
{
	{"TestConnect", TestConnect},
	{"TestTxRx", TestTxRx},
	{"TestCounter", TestCounter},
	{"TestMystring", TestMystring},
	{"TestLbServerConfigDefaults", TestLbServerConfigDefaults},
	{"TestLbServerConfigParseFromArguments", TestLbServerConfigParseFromArguments},
	{"TestLbServerConfigParseLine", TestLbServerConfigParseLine},
	{"ScheduleAtExit", ScheduleAtExit},
	{NULL, NULL}
};

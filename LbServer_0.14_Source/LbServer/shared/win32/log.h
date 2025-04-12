/*
 * log.h - logging for windows implementation
 *
 * $Id: log.h 1262 2023-02-27 14:33:22Z pischky $
 *
 */

#ifndef _LOG_H_INCLUDED_
#define _LOG_H_INCLUDED_

#include <cstdio>               /* fprintf(), stderr */
#include <windows.h>            /* GetCurrentThreadId() */

enum Priority {
	LOG_EMERG   = 0,
	LOG_ALERT   = 1,
	LOG_CRIT    = 2,
	LOG_ERR     = 3,
	LOG_WARNING = 4,
	LOG_NOTICE  = 5,
	LOG_INFO    = 6,
	LOG_DEBUG   = 7
};

extern int log_max_priority;
// in main use once:
// #ifndef WITH_JOURNALD
//     int log_max_priority = LOG_ERROR; // define log level
// #endif

#if defined(__cplusplus) && __cplusplus >= 202002L
	// C++20 supports __VA_OPT__
	#define log_print( PRIORITY, FORMAT, ... ) \
			if (PRIORITY <= log_max_priority) fprintf( stderr, "\n" FORMAT "\n", __VA_OPT__(,) __VA_ARGS__ )
	#define log_print_and_stderr( PRIORITY, FORMAT, ... ) \
	        log_print( PRIORITY, FORMAT, __VA_OPT__(,) __VA_ARGS__ )
#elif defined(__GNUC__)
	// fallback to old gcc extension
	#define log_print( PRIORITY, FORMAT, ... ) \
			if (PRIORITY <= log_max_priority) fprintf( stderr, "\n" FORMAT "\n" , ##__VA_ARGS__ )
	#define log_print_and_stderr( PRIORITY, FORMAT, ... ) \
	        log_print( PRIORITY, FORMAT, ##__VA_ARGS__ )
#else
	// older versions require at least one argument so include FORMAT
	#define log_print( PRIORITY, ... ) \
			do {                                    \
			    if (PRIORITY <= log_max_priority) { \
			        fprintf( stderr, "\n" );        \
			        fprintf( stderr, __VA_ARGS__ ); \
			        fprintf( stderr, "\n" );        \
			    }                                   \
			} while(0)
	#define log_print_and_stderr( PRIORITY, ... )   \
	        log_print( PRIORITY, __VA_ARGS__ )
#endif

#define tid() \
		GetCurrentThreadId()

#endif /* _LOG_H_INCLUDED_ */

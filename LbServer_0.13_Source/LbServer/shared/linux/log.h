/*
 * log.h - logging for linux implementation
 *
 * $Id: log.h 1255 2023-02-22 16:12:00Z pischky $
 *
 */

#ifndef _LOG_H_INCLUDED_
#define _LOG_H_INCLUDED_

#undef WITH_TID // hack to output thread id: use %lx to print pthread_self()
#undef WITH_TID_AND_TNAME // hack to output thread name (select only one)

#ifdef WITH_JOURNALD
	#include <syslog.h>             /* LOG_EMERG=0, LOG_ALERT, LOG_CRIT,
	                                   LOG_ERR, LOG_WARNING, LOG_NOTICE,
	                                   LOG_INFO, LOG_DEBUG=7 */
	// on RaspiOS install package with "apt install libsystemd-dev"
	#include <systemd/sd-journal.h> /* sd_journal_print() */
	#include <pthread.h>            /* pthread_self() */
#else
	#include <cstdio>               /* fprintf(), stderr */
#endif

// Note: we need to use macros to capture __FILE__, __LINE__ and  __func__

#ifdef WITH_JOURNALD

	#define tid() \
	        pthread_self()

	#if defined(__cplusplus) && __cplusplus >= 202002L
		// C++20 supports __VA_OPT__ (untested)
		#ifdef WITH_TID
			#define log_print( PRIORITY, FORMAT, ... ) \
			        sd_journal_print( PRIORITY, "{%lx} " FORMAT, tid(), __VA_OPT__(,) __VA_ARGS__ )
			#define log_print_and_stderr( PRIORITY, FORMAT, ... ) \
			        do {                                                                                 \
			            sd_journal_print( PRIORITY, "{%lx} " FORMAT, tid(), __VA_OPT__(,) __VA_ARGS__ ); \
			            fprinf( stderr, FORMAT "\n", __VA_OPT__(,) __VA_ARGS__ );                        \
			        } while(0)
		#else
			#define log_print( PRIORITY, FORMAT, ... ) \
			        sd_journal_print( PRIORITY, FORMAT, __VA_OPT__(,) __VA_ARGS__ )
			#define log_print_and_stderr( PRIORITY, FORMAT, ... ) \
			        do {                                                                 \
			            sd_journal_print( PRIORITY, FORMAT, __VA_OPT__(,) __VA_ARGS__ ); \
			            fprinf( stderr, FORMAT "\n", __VA_OPT__(,) __VA_ARGS__ );        \
			        } while(0)
		#endif
	#elif defined(__GNUC__)
		// fallback to old gcc extension
		#ifdef WITH_TID
			#define log_print( PRIORITY, FORMAT, ... ) \
			        sd_journal_print( PRIORITY, "{%lx} " FORMAT, tid(), ##__VA_ARGS__ )
			#define log_print_and_stderr( PRIORITY, FORMAT, ... )                            \
			        do {                                                                     \
			            sd_journal_print( PRIORITY, "{%lx} " FORMAT, tid(), ##__VA_ARGS__ ); \
			            fprinf( stderr, FORMAT "\n", ##__VA_ARGS__ );                        \
			        } while(0)
		#elif defined(WITH_TID_AND_TNAME)
			#define log_print( PRIORITY, FORMAT, ... ) \
			        do {                                                                                 \
			            char buf[16] = "";                                                               \
			            if ( pthread_getname_np(pthread_self(), buf, sizeof(buf)) ) {                    \
			                sd_journal_print( PRIORITY, "{%lx:%s} " FORMAT, tid(), "?", ##__VA_ARGS__ ); \
			            } else {                                                                         \
			                sd_journal_print( PRIORITY, "{%lx:%s} " FORMAT, tid(), buf, ##__VA_ARGS__ ); \
			            }                                                                                \
			        } while(0)
			#define log_print_and_stderr( PRIORITY, FORMAT, ... ) \
			        do {                                                                                 \
			            char buf[16] = "";                                                               \
			            if ( pthread_getname_np(pthread_self(), buf, sizeof(buf)) ) {                    \
			                sd_journal_print( PRIORITY, "{%lx:%s} " FORMAT, tid(), "?", ##__VA_ARGS__ ); \
			            } else {                                                                         \
			                sd_journal_print( PRIORITY, "{%lx:%s} " FORMAT, tid(), buf, ##__VA_ARGS__ ); \
			            };                                                                               \
			            fprintf( stderr, FORMAT "\n", ##__VA_ARGS__ );                                   \
			        } while(0)
		#else
			#define log_print( PRIORITY, FORMAT, ... ) \
			        sd_journal_print( PRIORITY, FORMAT, ##__VA_ARGS__ )
			#define log_print_and_stderr( PRIORITY, FORMAT, ... )            \
			        do {                                                     \
			            sd_journal_print( PRIORITY, FORMAT, ##__VA_ARGS__ ); \
			            fprintf( stderr, FORMAT "\n", ##__VA_ARGS__ );       \
			        } while(0)
		#endif
	#else
		// older versions require at least one argument (untested)
		#ifdef WITH_TID
			#define log_print( PRIORITY, FORMAT, ... ) \
			        sd_journal_print( PRIORITY, "{%lx} " FORMAT, tid() __VA_ARGS__ )
			#error "define 'log_print_and_stderr' or use newer compiler"
		#elif defined(WITH_TID_AND_TNAME)
			#error "define 'log_print' or use newer compiler"
			#error "define 'log_print_and_stderr' or use newer compiler"
		#else
			#define log_print( PRIORITY, ... ) \
			        sd_journal_print( PRIORITY, __VA_ARGS__ )
			#error "define 'log_print_and_stderr' or use newer compiler"
		#endif
	#endif

#else

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
		#error "define 'log_print_and_stderr'"
	#elif defined(__GNUC__)
		// fallback to old gcc extension
		#define log_print( PRIORITY, FORMAT, ... ) \
		        if (PRIORITY <= log_max_priority) fprintf( stderr, "\n" FORMAT "\n" , ##__VA_ARGS__ )
		#error "define 'log_print_and_stderr'"
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
		#error "define 'log_print_and_stderr'"
	#endif

	#define tid() \
	        pthread_self()

#endif

#endif /* _LOG_H_INCLUDED_ */

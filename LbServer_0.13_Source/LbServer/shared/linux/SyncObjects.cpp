// SyncObjects.cpp: Implementierung der Klasse CAbstractSyncObject, CSyncMutex.
// $Id: SyncObjects.cpp 1230 2023-02-16 09:53:31Z pischky $
//
//////////////////////////////////////////////////////////////////////

#include <cassert>      // assert()
#include <cerrno>       // EAGAIN
#include <stdexcept> 	// range_error
#include <pthread.h>	// pthread_mutex_init(), pthread_mutex_lock(), pthread_mutex_unlock()
#include <signal.h> 	// SIGRTMIN, SIGRTMAX, SIG_BLOCK,
						// sigtimedwait(), sigwaitinfo(),
						// sigemptyset(), sigaddset(), sigprocmask()
#include <stdio.h> 		// perror(), fprintf()
#include <string.h>		// strerror()
#include <time.h>		// struct timespec
#include "SyncObjects.h"
#include "log.h" /* log_print(), tid(), LOG_EMERG=0, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG=7 */
#include <sys/time.h>   // gettimeofday(), struct timeval
#include <time.h>		// struct timespec

using std::range_error;

//////////////////////////////////////////////////////////////////////

CAbstractSyncObject::CAbstractSyncObject()
:	condition_triggered(false)
{
	log_print(LOG_DEBUG, "CAbstractSyncObject()" );
	int rc;
	rc = pthread_mutex_init(&condition_mutex, nullptr);
	if (rc != 0) log_print(LOG_ERR, "CAbstractSyncObject() fail on pthread_mutex_init (rc=%i)", rc);
	rc = pthread_cond_init(&condition_cond, nullptr);
	if (rc != 0) log_print(LOG_ERR, "CAbstractSyncObject() fail on pthread_cond_init (rc=%i)", rc);
}

CAbstractSyncObject::~CAbstractSyncObject()
{
	log_print(LOG_DEBUG, "~CAbstractSyncObject()" );
	int rc;
	rc = pthread_cond_destroy(&condition_cond);
	if (rc != 0) log_print(LOG_ERR, "~CAbstractSyncObject() fail on pthread_cond_destroy (rc=%i)", rc);
	rc = pthread_mutex_destroy(&condition_mutex);
	if (rc != 0) log_print(LOG_ERR, "~CAbstractSyncObject() fail on pthread_mutex_destroy (rc=%i)", rc);
}

// Suspend process until signal is received or times out.
// Returns true if signal is received, false if times out
bool CAbstractSyncObject::Wait(unsigned long ulMilliseconds)
{
	//log_print(LOG_DEBUG, "Wait(%lu)", ulMilliseconds );
	bool retVal = false;
	int rc;
	rc = pthread_mutex_lock( &condition_mutex );
	if (rc != 0) {
		log_print(LOG_ERR, "Wait() fail on pthread_mutex_lock (rc=%i: %s)", rc, strerror(rc));
	}
	struct timeval now;
	rc = gettimeofday(&now, nullptr);
	if (rc != 0) {
		log_print( LOG_ERR, "Wait() fail on gettimeofday() "
		           "(errno=%i: %s)", errno, strerror(errno) );
	}
	long int seconds = now.tv_sec + ulMilliseconds/1000;
	long int nanos = now.tv_usec * 1000 + (ulMilliseconds % 1000) * 1000'000;
	if ( nanos > 999'999'999 ) {
		seconds += nanos / 1000'000'000;
		nanos %= 1000'000'000;
	}
	struct timespec abstime;
	abstime.tv_sec = seconds;
	abstime.tv_nsec = nanos;
	assert(0 <= abstime.tv_nsec && abstime.tv_nsec <= 999'999'999);
	rc = 0;
	while (rc == 0 && !condition_triggered) {
		rc = pthread_cond_timedwait(&condition_cond, &condition_mutex, &abstime);
		if (rc != 0 && rc != ETIMEDOUT) log_print(LOG_ERR, "Wait() fail on pthread_cond_timedwait (rc=%i: %s)", rc, strerror(rc));
	}
	if (rc == ETIMEDOUT) {
		retVal = false;
	} if (rc == 0) {
		retVal = true;
	}
	// condition_triggered = false // ???
	rc = pthread_mutex_unlock( &condition_mutex );
	if (rc != 0) log_print(LOG_ERR, "Wait() fail on pthread_mutex_unlock (rc=%i: %s)", rc, strerror(rc));
	//log_print(LOG_DEBUG, "Wait(%lu): retVal=%s", ulMilliseconds, retVal?"true":"false" );
	return retVal;
}

bool CAbstractSyncObject::WaitInfinite()
{
	//log_print(LOG_DEBUG, "WaitInfinite()" );
	// see https://stackoverflow.com/questions/178114 answer by ephemient on Oct 7, 2008 at 15:15
	bool retVal = false;
	int rc;
	rc = pthread_mutex_lock( &condition_mutex );
	if (rc != 0) {
		log_print(LOG_ERR, "WaitInfinite() fail on pthread_mutex_lock (rc=%i: %s)", rc, strerror(rc));
	}
	rc = 0;
	while (rc == 0 && !condition_triggered) {
		rc = pthread_cond_wait(&condition_cond, &condition_mutex);
		if (rc != 0) log_print(LOG_ERR, "WaitInfinite() fail on pthread_cond_wait (rc=%i: %s)", rc, strerror(rc));
	}
	if (rc == 0) {
		retVal = true;
	}
	// condition_triggered = false // ???
	rc = pthread_mutex_unlock( &condition_mutex );
	if (rc != 0) log_print(LOG_ERR, "Wait() fail on pthread_mutex_unlock (rc=%i: %s)", rc, strerror(rc));
	return retVal;
}


CSyncEvent::CSyncEvent()
{
	log_print(LOG_DEBUG, "CSyncEvent()" );
}

void CSyncEvent::Reset()
{
	log_print(LOG_DEBUG, "Reset()" );
	int rc;
	rc = pthread_mutex_lock( &condition_mutex );
	if (rc != 0) {
		log_print(LOG_ERR, "Reset() fail on pthread_mutex_lock (rc=%i: %s)", rc, strerror(rc));
	}
	condition_triggered = false;
	rc = pthread_mutex_unlock( &condition_mutex );
	if (rc != 0) log_print(LOG_ERR, "Reset() fail on pthread_mutex_unlock (rc=%i: %s)", rc, strerror(rc));
}

void CSyncEvent::Set()
{
	log_print(LOG_DEBUG, "Set()" );
	int rc;
	rc = pthread_mutex_lock( &condition_mutex );
	if (rc != 0) log_print(LOG_ERR, "Set() fail on pthread_mutex_lock (rc=%i)", rc);
	condition_triggered = true;
	rc = pthread_cond_signal( &condition_cond );
	if (rc != 0) log_print(LOG_ERR, "Set() fail on pthread_cond_signal (rc=%i)", rc);
	rc = pthread_mutex_unlock( &condition_mutex );
	if (rc != 0) log_print(LOG_ERR, "Set() fail on pthread_mutex_unlock (rc=%i)", rc);
}

//////////////////////////////////////////////////////////////////////

CSyncMutex::CSyncMutex()
{
	pthread_mutex_init(&m_rclOurMutex, NULL);
}

bool CSyncMutex::Entry(unsigned long ulMilliseconds)
{
	if (ulMilliseconds)
	{
		//TODO pthread_mutex_timedlock ??
	}
	int rc = pthread_mutex_lock(&m_rclOurMutex);
	if (rc != 0) {
		fprintf(stderr, "\nCSyncMutex::Entry(): pthread_mutex_lock failed, "
				"%s\n", strerror(rc));
	}
	return rc == 0;
}

void CSyncMutex::Exit()
{
	int rc = pthread_mutex_unlock(&m_rclOurMutex);
	if (rc != 0) {
		fprintf(stderr, "\nCSyncMutex::Exit(): pthread_mutex_unlock failed, "
				"%s\n", strerror(rc));
	}
}

//////////////////////////////////////////////////////////////////////

CMutexUser::CMutexUser(CSyncMutex &rclMutex)
:	m_rclOurMutex(rclMutex)
{
	m_rclOurMutex.Entry(0);
}

CMutexUser::~CMutexUser()
{
	m_rclOurMutex.Exit();
}

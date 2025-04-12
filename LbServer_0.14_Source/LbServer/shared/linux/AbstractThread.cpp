// AbstractThread.cpp: Implementierung der Klasse CAbstractThread.
// $Id: AbstractThread.cpp 1318 2023-04-15 10:37:27Z pischky $
//
//////////////////////////////////////////////////////////////////////

#include <pthread.h> /* pthread_create(), pthread_cancel(), pthread_join() */
#include <signal.h> /* sigfillset(), pthread_sigmask(), sigset_t, SIG_SETMASK */
#include <cstring> /* strerror(), strcmp(), strncpy() */
#include "log.h" /* log_print(), tid(), LOG_EMERG=0, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG=7 */
#include "AbstractThread.h"


void *Thread(void *pvParam)
{
	log_print( LOG_DEBUG, "Thread(%p)", pvParam );
	CAbstractThread *pclMyself = (CAbstractThread *)pvParam;

	pclMyself->ThreadMain();
	pclMyself->m_pthread = 0; //FIXME: Why ??? This is bad when we want to join

	if (pclMyself->m_bDeleteMyselfOnExit)
		delete pclMyself;

	return 0;
}



//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CAbstractThread::CAbstractThread()
: m_pthread(0),
  m_threadName(""),
  m_bDeleteMyselfOnExit(false)
{
	log_print( LOG_DEBUG, "CAbstractThread()" );
}


CAbstractThread::~CAbstractThread()
{
	log_print( LOG_DEBUG, "~CAbstractThread(): m_pthread=%lx", m_pthread );
	this->StopThread();
}

bool CAbstractThread::StartThread() {
	return this->StartThread("");
}

bool CAbstractThread::StartThread(const char* threadName)
{
	log_print( LOG_DEBUG, "StartThread(\"%s\"): m_pthread=%lx", threadName, m_pthread );
	if (m_pthread != 0) {
		log_print( LOG_WARNING, "StartThread() failed. m_pthread=%lx", m_pthread );
		return false;
	}

	sigset_t set;
	sigset_t savedSet;
	sigfillset(&set);
	// disable all signals on this thread
	int rc = pthread_sigmask(SIG_SETMASK, &set, &savedSet);
	if (rc != 0) {
		log_print( LOG_ERR, "StartThread() pthread_sigmask failed. rc=%i: %s", rc, strerror(rc) );
		//TODO: abort
	}
	// new thread inherits creating thread's signal mask
	rc = pthread_create(&m_pthread, NULL, &Thread, this);
	// restore signal mask on current thread
	int rc2 = pthread_sigmask(SIG_SETMASK, &savedSet, nullptr);
	if (rc2 != 0) {
		log_print( LOG_ERR, "StartThread() pthread_sigmask failed. rc=%i: %s", rc2, strerror(rc2) );
		//TODO: abort
	}
	if (strcmp(threadName, "") != 0) {
		strncpy(m_threadName, threadName, sizeof(m_threadName)); m_threadName[sizeof(m_threadName) - 1] = '\0';
		rc2 = pthread_setname_np(m_pthread, m_threadName);
		if (rc2 != 0) {
			log_print( LOG_ERR, "StartThread() pthread_setname_np failed. rc=%i: %s", rc2, strerror(rc2) );
			//TODO: abort
		}
	}
	if (rc != 0) {
		log_print( LOG_WARNING, "StartThread() failed. rc=%i: %s", rc, strerror(rc) );
	} else {
		log_print( LOG_DEBUG, "StartThread(): successfully started. "
		           "m_pthread=%lx, m_threadName=\"%s\"", m_pthread, m_threadName );
	}
	return rc == 0;
}


bool CAbstractThread::StopThread()
{
	log_print( LOG_DEBUG, "StopThread() m_pthread=%lx", m_pthread );
	if (m_pthread == 0) return false;
	int rc = pthread_cancel(m_pthread);
	if (rc != 0) {
		log_print(LOG_ERR,"StopThread() pthread_cancel(%lx) failed. rc=%i: %s", m_pthread, rc, strerror(rc) );
	} else {
		m_pthread = 0;
	}
	return rc == 0;
}


bool CAbstractThread::Join()
{
	log_print( LOG_DEBUG, "Join() --start m_pthread=%lx", m_pthread );
	//TODO: if (m_pthread == 0) return true; ???
	int rc = pthread_join(m_pthread, nullptr);
	if (rc != 0) {
		log_print(LOG_ERR,"Join(): pthread_join(%lx) failed. rc=%i: %s", m_pthread, rc, strerror(rc) );
	} else {
		m_pthread = 0; // ???
	}
	bool retVal = (rc == 0);
	log_print( LOG_DEBUG, "Join() --done retVal=%s", retVal?"true":"false" );
	return retVal;
}

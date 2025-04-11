// AbstractThread.h: Schnittstelle fr die Klasse CAbstractThread.
// $Id: AbstractThread.h 1222 2023-02-08 12:32:11Z pischky $
//
//////////////////////////////////////////////////////////////////////

#ifndef _ABSTRACT_THREAD_H_
#define _ABSTRACT_THREAD_H_


#include "OsSpecific.h"


class CAbstractThread  
{
	friend void *Thread(void *pvParam);
//	friend static unsigned long __stdcall Thread(void *pvParam);

public:
	bool StopThread();
	/**
	 * Start thread.
	 * Thread name is left as default (program name).
	 */
	bool StartThread();
	/**
	 * Start thread and set thread name.
	 * @param threadName The name of the thread (up to 15 chars).
	 *                   Use "" to use default (program name).
	 */
	bool StartThread(const char* threadName);
	CAbstractThread();
	virtual ~CAbstractThread();

protected:
	virtual void ThreadMain()=0;
	pthread_t m_pthread;
	/** Thread name displayed in debugger (see: man 3 pthread_setname_np) */
	char m_threadName[16];
	bool m_bDeleteMyselfOnExit;
};

#endif // _ABSTRACT_THREAD_H_

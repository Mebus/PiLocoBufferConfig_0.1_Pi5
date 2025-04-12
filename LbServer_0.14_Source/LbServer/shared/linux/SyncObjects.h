// SyncObjects.h: Schnittstelle fuer die Klasse CSyncMutex.
// $Id: SyncObjects.h 1229 2023-02-15 10:32:33Z pischky $
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SYNCOBJECTS_H_INCLUDED_)
#define _SYNCOBJECTS_H_INCLUDED_

#include <signal.h> 	// sigset_t
#include <pthread.h> 	// pthread_mutex_t, pthread_mutex_t, pthread_cond_t

class CAbstractSyncObject  
{
public:
	CAbstractSyncObject();
	virtual ~CAbstractSyncObject();

	bool Wait(unsigned long ulMilliseconds=1000);
	bool WaitInfinite();
protected:
	//TODO: move to CSyncEvent (but Wait, WaitInfinite use it)
	pthread_mutex_t condition_mutex;
	pthread_cond_t  condition_cond;
	bool            condition_triggered;
};

class CSyncEvent : public CAbstractSyncObject
{
public:
	CSyncEvent();
	virtual ~CSyncEvent()
	{ }

	void Set();
	void Reset();
};


class CSyncMutex : public CAbstractSyncObject
{
public:
	CSyncMutex();
	virtual ~CSyncMutex(){}

	void Exit();
	bool Entry(unsigned long ulMilliseconds);
private:
	pthread_mutex_t m_rclOurMutex;
};


class CMutexUser
{
public:
	CMutexUser(CSyncMutex &rclMutex);
	virtual ~CMutexUser();

private:
	CSyncMutex &m_rclOurMutex;
};


#endif // !defined(_SYNCOBJECTS_H_INCLUDED_)

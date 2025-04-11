// SyncObjects.cpp: Implementierung der Klasse CSyncMutex.
// $Id: SyncObjects.cpp 996 2017-12-09 18:18:22Z pischky $
//
//////////////////////////////////////////////////////////////////////

#include "SyncObjects.h"


#ifndef _WIN32
#error "You are very courageous trying to compile this outside a WIN32 environment! Happy porting!"
#endif


#ifndef ASSERT
#define ASSERT(x)
#define VERIFY(x) (x)
#endif

//////////////////////////////////////////////////////////////////////


CAbstractSyncObject::CAbstractSyncObject()
:	m_hObject(0)
{}

CAbstractSyncObject::~CAbstractSyncObject()
{
	CloseHandle(m_hObject);
}

bool CAbstractSyncObject::WaitForObject(unsigned long ulMilliseconds)
{
	DWORD dwReturn = WaitForSingleObject(m_hObject, ulMilliseconds);
	ASSERT (dwReturn != WAIT_ABANDONED);
	return (dwReturn == WAIT_OBJECT_0);
}


//////////////////////////////////////////////////////////////////////


CSyncEvent::CSyncEvent()
{
	m_hObject = CreateEvent(NULL, FALSE, FALSE, NULL);
}

bool CSyncEvent::Wait(unsigned long ulMilliseconds)
{
	return WaitForObject(ulMilliseconds);
}

bool CSyncEvent::WaitInfinite()
{
	return WaitForObject(INFINITE);
}

void CSyncEvent::Reset()
{
	VERIFY(ResetEvent(m_hObject));
}

void CSyncEvent::Set()
{
	VERIFY(SetEvent(m_hObject));
}


//////////////////////////////////////////////////////////////////////


CSyncMutex::CSyncMutex()
{
	m_hObject = CreateMutex(NULL, FALSE, NULL);
}

bool CSyncMutex::Entry(unsigned long ulMilliseconds)
{
	return WaitForObject(ulMilliseconds);
}

void CSyncMutex::Exit()
{
	ReleaseMutex(m_hObject);
}

//////////////////////////////////////////////////////////////////////

CMutexUser::CMutexUser(CSyncMutex &rclMutex)
:	m_rclOurMutex(rclMutex)
{
	VERIFY(m_rclOurMutex.Entry(INFINITE));	// can not jeopardise to time out becaus lacking return value
}

CMutexUser::~CMutexUser()
{
	m_rclOurMutex.Exit();
}

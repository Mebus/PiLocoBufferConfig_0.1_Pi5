// SyncObjects.h: Schnittstelle für die Klasse CSyncMutex.
// $Id: SyncObjects.h 996 2017-12-09 18:18:22Z pischky $
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SYNCOBJECTS_H_INCLUDED_)
#define _SYNCOBJECTS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "wtypes.h"


class CAbstractSyncObject  
{
public:
	CAbstractSyncObject();
	virtual ~CAbstractSyncObject();

protected:
	HANDLE m_hObject;
	bool WaitForObject(unsigned long ulMilliseconds=2000);
};


class CSyncEvent : public CAbstractSyncObject
{
public:
	bool Wait(unsigned long ulMilliseconds=2000);
	bool WaitInfinite();
	CSyncEvent();
	virtual ~CSyncEvent(){}

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
};


class CMutexUser  
{
public:
	CMutexUser(CSyncMutex &rclMutex);
	virtual ~CMutexUser();

protected:
	CSyncMutex &m_rclOurMutex;
};


#endif // !defined(_SYNCOBJECTS_H_INCLUDED_)

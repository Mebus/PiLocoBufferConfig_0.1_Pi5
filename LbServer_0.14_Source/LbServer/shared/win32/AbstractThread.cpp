// AbstractThread.cpp: Implementierung der Klasse CAbstractThread.
// $Id: AbstractThread.cpp 1317 2023-04-12 21:11:18Z sbormann71 $
//
//////////////////////////////////////////////////////////////////////

#include "AbstractThread.h"


//static unsigned long __stdcall Thread(void *pvParam)
unsigned long __stdcall Thread(void *pvParam)
{
	CAbstractThread *pclMyself = (CAbstractThread *)pvParam;

	pclMyself->ThreadMain();

	CloseHandle(pclMyself->m_hThread);
	pclMyself->m_hThread = 0;

	if (pclMyself->m_bDeleteMyselfOnExit)
		delete pclMyself;

	return 0;
}



//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CAbstractThread::CAbstractThread()
:	m_hThread(0),
	m_bDeleteMyselfOnExit(false)
{}


CAbstractThread::~CAbstractThread()
{
	if (m_hThread != 0)
	{
		(void)StopThread();
		CloseHandle(m_hThread);
		m_hThread = 0;
	}
}


bool CAbstractThread::StartThread(const char* threadName)
{
	if (m_hThread!=0) return false;

	unsigned long ulThreadId;	// what is this for? the function returns a handle!?
	m_hThread = CreateThread(NULL, 0, &Thread, this, 0, &ulThreadId);

	return (m_hThread!=0);
}


bool CAbstractThread::StopThread()
{
	if (m_hThread==0) return false;
	return (TerminateThread(m_hThread, 0)!=0);
}


bool CAbstractThread::Join()
{
	return 0 == WaitForSingleObject(m_hThread, INFINITE);
}

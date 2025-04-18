// AbstractThread.h: Schnittstelle f�r die Klasse CAbstractThread.
// $Id: AbstractThread.h 1222 2023-02-08 12:32:11Z pischky $
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABSTRACTTHREAD_H__A33E942C_161E_4DF7_B537_FBBCE16DD0F3__INCLUDED_)
#define AFX_ABSTRACTTHREAD_H__A33E942C_161E_4DF7_B537_FBBCE16DD0F3__INCLUDED_



#include "winsock.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAbstractThread  
{
	// friend static unsigned long __stdcall Thread(void *pvParam);
	// did not compile with minGW
	friend unsigned long __stdcall Thread(void *pvParam);
public:
	bool StopThread();
	bool StartThread() { return StartThread(""); }
	bool StartThread(const char* threadName);
	CAbstractThread();
	virtual ~CAbstractThread();

protected:
	virtual void ThreadMain()=0;

	HANDLE m_hThread;
	bool m_bDeleteMyselfOnExit;
};

#endif // !defined(AFX_ABSTRACTTHREAD_H__A33E942C_161E_4DF7_B537_FBBCE16DD0F3__INCLUDED_)

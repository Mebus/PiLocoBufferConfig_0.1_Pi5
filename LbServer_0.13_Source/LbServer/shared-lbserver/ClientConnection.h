// ClientConnection.h: Schnittstelle fr die Klasse CClientConnection.
// $Id: ClientConnection.h 1208 2023-02-03 14:05:45Z pischky $
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTCONNECTION_H__00CB6EF7_F027_4350_83C6_790A1AB4AE59__INCLUDED_)
#define AFX_CLIENTCONNECTION_H__00CB6EF7_F027_4350_83C6_790A1AB4AE59__INCLUDED_


#include "AbstractThread.h"
#include "ReceiveLineBuffer.h"

#include "OsSpecific.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CClientConnection;


class CClientConnectionHandler
{
friend class CClientConnection;
public:
	virtual ~CClientConnectionHandler()
	{ }
private:
	virtual void DestroyConnection(CClientConnection *pclDestroyThis)=0;
	virtual void OnClientReceive(CClientConnection *pclFrom, char *pcLine)=0;
};


class CClientConnection : protected  CAbstractThread, protected CReceiveLineBuffer 
{
public:
	SOCKET GetSocket();
	bool Send(const void *pvData, unsigned int uiCount);
	CClientConnection(CClientConnectionHandler &rClientConnectionHandler, SOCKET hSocket);

virtual ~CClientConnection(){}

	bool SendLine(const char *pcLine);
	bool StartReceiveThread();

protected:
	virtual void OnRecvLine(char *pcLine);
	virtual void ThreadMain();
	void Disconnected();
	void AbortConnection(const char *pcReason);
	SOCKET						m_hSocket;
	CClientConnectionHandler	&m_rClientConnectionHandler;
};


#endif // !defined(AFX_CLIENTCONNECTION_H__00CB6EF7_F027_4350_83C6_790A1AB4AE59__INCLUDED_)

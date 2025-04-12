// TcpServer.h: Schnittstelle fr die Klasse CTcpServer.
// $Id: TcpServer.h 1208 2023-02-03 14:05:45Z pischky $
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPSERVER_H__458ED9BF_975C_4CC3_981E_ABD764A466C6__INCLUDED_)
#define AFX_TCPSERVER_H__458ED9BF_975C_4CC3_981E_ABD764A466C6__INCLUDED_


#include "OsSpecific.h"
#include "AbstractThread.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTcpConnectionCreator
{
public:
	virtual ~CTcpConnectionCreator()
	{ }
friend class CTcpServer;
private:
	virtual void OnAccept(SOCKET hNewConnection)=0;
};


class CTcpServer : protected CAbstractThread  
{
public:
	CTcpServer(CTcpConnectionCreator &rTcpConnectionCreator);
	virtual ~CTcpServer();
	bool StartListenThread(int iPort);

protected:
	void SockError(const char *pcMessage);
	virtual void ThreadMain();

	CTcpConnectionCreator &m_rTcpConnectionCreator;
	SOCKET m_hListenSocket;
};

#endif // !defined(AFX_TCPSERVER_H__458ED9BF_975C_4CC3_981E_ABD764A466C6__INCLUDED_)

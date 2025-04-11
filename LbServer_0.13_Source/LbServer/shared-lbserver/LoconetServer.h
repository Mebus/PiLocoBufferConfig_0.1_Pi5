// LoconetServer.h: Schnittstelle fr die Klasse CLoconetServer.
// $Id: LoconetServer.h 994 2017-12-09 17:48:53Z pischky $
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCONETSERVER_H__021D0473_F286_461D_BFB9_B3C3F7B98EC2__INCLUDED_)
#define AFX_LOCONETSERVER_H__021D0473_F286_461D_BFB9_B3C3F7B98EC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "TcpServer.h"
#include "ClientConnection.h"
#include "AbstractLoconetDevice.h"
#include <vector>
#include "SyncObjects.h"
#include "LoconetMessage.h"


class CLoconetServer : 
	private CClientConnectionHandler, 
	private CTcpConnectionCreator  
{
public:
	bool StartListenThread(int iPort);
	void Run();
	CLoconetServer(CAbstractLoconetDevice &rclLoconetDevice);
	virtual ~CLoconetServer();

private:
	CSyncMutex m_clSendMutex;
	CSyncEvent m_clSendEchoEvent;
	void OnRecvSEND(CClientConnection *pclFrom, const char *pcParam);
	bool BroadcastToTcpClients(CClientConnection *pclNotThis, char *pcLine);
	virtual void OnAccept(SOCKET hNewConnection);
	virtual void DestroyConnection(CClientConnection *pclDestroyThis);
	virtual void OnClientReceive(CClientConnection *pclFrom, char *pcLine);

protected:
	CClientConnection * m_pclSendingClient;
	CLoconetMessage *m_pclSentMessage;	// pointer to message that is currently being sent
	CTcpServer m_clAcceptThread;
	std::vector<CClientConnection *> m_apclClientConnections;
	CAbstractLoconetDevice &m_rclLoconetDevice;
};

#endif // !defined(AFX_LOCONETSERVER_H__021D0473_F286_461D_BFB9_B3C3F7B98EC2__INCLUDED_)

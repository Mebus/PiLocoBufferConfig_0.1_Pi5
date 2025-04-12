// TcpServer.cpp: Implementierung der Klasse CTcpServer.
// $Id: TcpServer.cpp 1262 2023-02-27 14:33:22Z pischky $
//
//////////////////////////////////////////////////////////////////////

#ifdef __linux__
#include <string>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include "DisplayFrontend.h"

#include "log.h" /* log_print(), LOG_EMERG=0, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG=7 */
#include "TcpServer.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CTcpServer::CTcpServer(CTcpConnectionCreator &rTcpConnectionCreator)
:	m_rTcpConnectionCreator(rTcpConnectionCreator)
,	m_hListenSocket(INVALID_SOCKET)
{
	log_print( LOG_DEBUG, "CTcpServer(CTcpConnectionCreator)" );
}

CTcpServer::~CTcpServer() {
	#ifdef __linux__
		log_print( LOG_DEBUG, "~CTcpServer(): m_pthread=%lx", m_pthread );
	#else
		log_print( LOG_DEBUG, "~CTcpServer()" );
	#endif
}

bool CTcpServer::StartListenThread(int iPort)
{
	log_print( LOG_DEBUG, "CTcpServer::StartListenThread(%i)", iPort );
	int nRet;
	struct sockaddr_in stLclName;

 	/* Get a TCP socket to use for data connection listen */
	
//	unlink("m_hListenSocket");	// remove any old sockets
	m_hListenSocket = socket (AF_INET, SOCK_STREAM, 0);
	if (m_hListenSocket == INVALID_SOCKET)
	{
		SockError("socket()");
		return false;
	} 

#ifdef __linux__
	int yes = -1;
	setsockopt (m_hListenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));	// lose pesky "address already in use" error message
#endif




	/* Name the local socket with bind() */

	stLclName.sin_family = PF_INET;
	stLclName.sin_port   = (u_short) htons((u_short)iPort);
#ifdef _WIN32
	stLclName.sin_addr.S_un.S_addr   = INADDR_ANY;
#endif
#ifdef __linux__
	stLclName.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
	nRet = bind(m_hListenSocket, (struct sockaddr *)&stLclName, sizeof(stLclName));
	if (nRet == SOCKET_ERROR) 
	{
		CLOSESOCKET(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
		SockError("bind()");
		return false;
	}
	
	/* Listen for incoming connection requests */

	nRet = listen(m_hListenSocket, 5);
	if (nRet == SOCKET_ERROR) 
	{
		CLOSESOCKET(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
		SockError("listen()");
		return false;
	}

	return StartThread("Listen");
}


void CTcpServer::SockError(const char *pcMessage)
{
#ifdef _WIN32
	Display.ErrorMessageF(LOG_ERR, "CTcpServer::SockError(): Error '%s', WSAGetLastError() returns %d", pcMessage, WSAGetLastError());
#endif
#ifdef __linux__
	Display.ErrorMessageF(LOG_ERR, "CTcpServer::SockError(): Error '%s', strerror() returns: '%s'", pcMessage, strerror(errno));
#endif
}


void CTcpServer::ThreadMain()
{
	log_print( LOG_DEBUG, "CTcpServer::ThreadMain()" );
	while (1)
	{
		SOCKET hNewSock;
		SOCKLEN_T nLen = sizeof(struct sockaddr);
		struct sockaddr stName;
  
		hNewSock = accept (m_hListenSocket, &stName, &nLen);
		if (hNewSock == (SOCKET)SOCKET_ERROR) 
		{
			CLOSESOCKET(m_hListenSocket);
			m_hListenSocket = INVALID_SOCKET;
			SockError ("accept");
			return;
		}
#ifdef __linux__
		int yes=-1;
		setsockopt (hNewSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
#endif
		m_rTcpConnectionCreator.OnAccept(hNewSock);
	}
}

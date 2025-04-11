// ClientConnection.cpp: Implementierung der Klasse CClientConnection.
// $Id: ClientConnection.cpp 1222 2023-02-08 12:32:11Z pischky $
//
//////////////////////////////////////////////////////////////////////

#include <string.h> /* strlen() */
#include <stdio.h> 	/* sprintf() */
#include <cstdio>   /* sprintf() */

#include "OsSpecific.h"         /* INVALID_SOCKET, send(), closesocket(), 
                                   close() */
#include "ClientConnection.h"   /* class CClientConnection */
#include "DisplayFrontend.h"
#include "log.h"                /* LOG_NOTICE */


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CClientConnection::CClientConnection(CClientConnectionHandler &rClientConnectionHandler, SOCKET hSocket)
:	m_hSocket(hSocket),
	m_rClientConnectionHandler(rClientConnectionHandler)
{
}


bool CClientConnection::StartReceiveThread()
{
	static int count = 0;
	count = (count+1) % 100000;
	char buf[4+5+1];
	sprintf(buf, "Recv%05i", count);
	return StartThread(buf);
}


bool CClientConnection::Send(const void *pvData, unsigned int uiCount)
{
	//TODO do we need the cast?
	//linux: ssize_t send(int, const void *, size_t, int)
	size_t iSent = send(m_hSocket, (const char *)pvData, (size_t)uiCount, 0);
	return (iSent == uiCount);
}


bool CClientConnection::SendLine(const char *pcLine)
{
	if (!Send(pcLine, (unsigned int)strlen(pcLine))) return false;
	if (!Send("\r\n", 2)) return false;
	return true;
}


void CClientConnection::OnRecvLine(char *pcLine)
{
	m_rClientConnectionHandler.OnClientReceive(this, pcLine);
}


void CClientConnection::ThreadMain()
{
	while (1)
	{
		if (!ProcessReceiveBuffer())
		{
			AbortConnection("buffer overflow");
			m_bDeleteMyselfOnExit = true;
			return;
		}

		int iReceived = ReceiveToBuffer(m_hSocket);

		if (iReceived<=0)
		{
			Disconnected();
			m_bDeleteMyselfOnExit = true;
			return;
		}
	}
}


void CClientConnection::AbortConnection(const char *pcReason)
{
	Display.ErrorMessageF(LOG_NOTICE, "Aborting TCP connection: %s", pcReason);

	/*
	char acBuffer[1000];
	sprintf(acBuffer, "ERROR %s", pcReason); // protocol version 2
	SendLine(acBuffer);
	*/

	Disconnected();
}


void CClientConnection::Disconnected()
{
#ifdef _WIN32
	closesocket(m_hSocket);
#endif
#ifdef __linux__
	close(m_hSocket);
#endif
	m_hSocket = INVALID_SOCKET;
	m_rClientConnectionHandler.DestroyConnection(this);
}


SOCKET CClientConnection::GetSocket()
{
	return m_hSocket;
}

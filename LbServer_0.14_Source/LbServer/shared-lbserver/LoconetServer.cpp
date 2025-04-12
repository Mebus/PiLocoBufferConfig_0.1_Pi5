// LoconetServer.cpp: Implementierung der Klasse CLoconetServer.
// $Id: LoconetServer.cpp 1312 2023-04-09 18:24:12Z sbormann71 $
//
//////////////////////////////////////////////////////////////////////

#include <string.h> /* strchr(), strcmp(), strlen(), strcpy() */ 
#include <stdio.h>  /* sprintf() */

#include "LoconetServer.h"  /* class CLoconetServer */
#include "Version.h"        /* VERSION */
#include "LoconetMessage.h" /* class CLoconetMessage */
#include "OsSpecific.h"     /* SOCKET */
#include "DisplayFrontend.h"/* Display */
#include "log.h"            /* LOG_ERR */


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1500
    #pragma warning( push )
    #pragma warning( disable: 4355 ) // warning C4355: 'this' : used in base member initializer list
#endif

CLoconetServer::CLoconetServer(CAbstractLoconetDevice &rclLoconetDevice)
:	m_pclSendingClient(NULL),
	m_pclSentMessage(NULL),
	m_clAcceptThread(*this),		// ok, I understand the warning, will fix that later
    m_rclLoconetDevice(rclLoconetDevice)
{}

#if _MSC_VER >= 1500
    #pragma warning( pop )
#endif

CLoconetServer::~CLoconetServer()
{}


void CLoconetServer::DestroyConnection(CClientConnection *pclDestroyThis)
{
	for (int i=0; (unsigned)i<m_apclClientConnections.size(); i++)
	{
		if (m_apclClientConnections[i] == pclDestroyThis)
		{
			m_apclClientConnections.erase(m_apclClientConnections.begin()+i);
		}
	}

	Display.Disconnection(m_apclClientConnections.size());
}



void CLoconetServer::OnAccept(SOCKET hNewConnection)
{
	// linux version fails during this new CClientConnection()
	CClientConnection *pclClientConnection = new CClientConnection(*this, hNewConnection);

	if (pclClientConnection->StartReceiveThread())
	{
		m_apclClientConnections.push_back(pclClientConnection);

		Display.Connection(m_apclClientConnections.size());

		char acVersionLine[100];
		sprintf(acVersionLine, "VERSION LbServer version %s built %s", VERSION, __DATE__);
		pclClientConnection->SendLine(acVersionLine);
	}
	else
	{
	Display.ErrorMessage(LOG_ERR, "CLoconetServer::OnAccept():CreateThread failed");
		delete pclClientConnection;
	}
}


void CLoconetServer::Run()
{
	for (;;)
	{
		unsigned char aucBuffer[127];
		char acString[1000];
		unsigned int ui;

		int iLength = m_rclLoconetDevice.ReceivePacket(aucBuffer, sizeof(aucBuffer));
		if (iLength<0) 
			return;

//---------- send receive message to all clients
		strcpy(acString, "RECEIVE");
		for (ui=0; ui<(unsigned int)iLength; ui++)
		{
			sprintf(&acString[strlen(acString)], " %02X", aucBuffer[ui]);
		}

		Display.ReceivedMessage(aucBuffer, (unsigned int)iLength);

		if (!BroadcastToTcpClients(NULL, acString)) 
			return;

//---------- if send is in process, see if this is an echo
		if (m_pclSentMessage!=NULL)
		{
			CLoconetMessage clTmpMessage(aucBuffer, iLength);
			if (m_pclSentMessage->Compare(clTmpMessage))
			{
				m_pclSendingClient->SendLine("SENT OK");	// sending reply to client
				Display.SentSuccess();
				m_clSendEchoEvent.Set();					// releasing blocked client thread to make space for next send request
			}
		}
	}
}


bool CLoconetServer::StartListenThread(int iPort)
{
	return m_clAcceptThread.StartListenThread(iPort);
}


bool CLoconetServer::BroadcastToTcpClients(CClientConnection *pclNotThis, char *pcLine)
{
//	clArrayMutex.Entry();
	for (unsigned int ui=0; ui<m_apclClientConnections.size(); ui++)
	{
		CClientConnection *pclCurrent = m_apclClientConnections[ui];
		if (pclCurrent!=pclNotThis)
		{
			if (!pclCurrent->SendLine(pcLine))
			{
				Display.ErrorMessageF(LOG_WARNING, "Failed to send to connection %u", ui);
			}
		}
	}
//	clArrayMutex.Exit();
	return true; // never failing here bad enough to justify LbServer termination
}


void CLoconetServer::OnClientReceive(CClientConnection *pclFrom, char *pcLine)
{
//----- Find first word
	char *pcParam = strchr(pcLine, ' ');
	if (pcParam!=NULL)
	{
		*pcParam = 0;
		pcParam++;
	}
//	printf("received command '%s' with parameter '%s' from socket %d\n", pcLine, pcParam, pclFrom->GetSocket);
	if      (strcmp(pcLine, "SEND")==0) OnRecvSEND(pclFrom, pcParam);
	else {}
}


void CLoconetServer::OnRecvSEND(CClientConnection *pclFrom, const char *pcParam)
{
	CLoconetMessage clSend;
	const char *pcReturn = clSend.ParseFromString(pcParam);

	if (pcReturn==NULL)	// ok
	{
		CMutexUser clSendMutexUser(m_clSendMutex);

		m_clSendEchoEvent.Reset();
		m_pclSentMessage = &clSend;
		m_pclSendingClient = pclFrom;

		Display.SendMessage(clSend.m_aucMessage, clSend.m_uiByteCount);

		if (m_rclLoconetDevice.SendPacket(clSend.m_aucMessage, clSend.m_uiByteCount))
		{
			if (!m_clSendEchoEvent.Wait())	// we sent successfully to the LocoBuffer, now we want to see the echo that means "sent it on the net"
				pcReturn = "timed out waiting for echo from LocoBuffer";
		}
		else
		{
			pcReturn = "RS232 failed";
		}

		m_pclSentMessage = NULL;
		m_pclSendingClient = NULL;
	}

	if (pcReturn != NULL) // error reported
	{
		Display.SendError(pcReturn);

		char acBuffer[1000] = "SENT ERROR ";
		strcat(acBuffer, pcReturn);
		pclFrom->SendLine(acBuffer);
	}
}


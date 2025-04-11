// ReceiveLineBuffer.cpp: Implementierung der Klasse CReceiveLineBuffer.
// $Id: ReceiveLineBuffer.cpp 1179 2023-01-08 17:25:25Z sbormann71 $
//
//////////////////////////////////////////////////////////////////////



#include "OsSpecific.h"
#include <string.h>
#include "ReceiveLineBuffer.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CReceiveLineBuffer::CReceiveLineBuffer()
:	m_uiBufferFill(0)
{

}


void CReceiveLineBuffer::ProcessLine(char *pcLineEnd)
{
	unsigned int uiIndex = (unsigned int) (pcLineEnd - &m_acBuffer[0]);

	if (uiIndex>0)						// won't process empty lines
	{
		m_acBuffer[uiIndex] = 0;		// terminate line string
		OnRecvLine(&m_acBuffer[0]);
	}
	uiIndex++;							// now index of start of rest of string

	m_uiBufferFill -= uiIndex;
	if (m_uiBufferFill!=0)
		memcpy(&m_acBuffer[0], &m_acBuffer[uiIndex], m_uiBufferFill);
}


bool CReceiveLineBuffer::ProcessReceiveBuffer()
{
//	ASSERT (m_uiBufferFill<=RECV_BUFFER_SIZE-1);	// must be one byte space to carry the string termination
	for (;;)
	{
		char *pc10, *pc13;

		m_acBuffer[m_uiBufferFill] = 0;	// terminate string

		pc10 = strchr(m_acBuffer, 10);
		pc13 = strchr(m_acBuffer, 13);

		if ((pc10==NULL) && (pc13==NULL)) return true;

		if (pc10==NULL)
			ProcessLine(pc13);
		else if (pc13==NULL)
			ProcessLine(pc10);
		else if (pc10>pc13)
			ProcessLine(pc13);
		else
			ProcessLine(pc10);
	}
}


int CReceiveLineBuffer::ReceiveToBuffer(SOCKET Socket)
{
//	ASSERT(m_uiBufferFill<RECV_BUFFER_SIZE);
	int iReceived = recv(Socket, &m_acBuffer[m_uiBufferFill], RECV_BUFFER_SIZE-1-m_uiBufferFill, 0);
	if (iReceived>0) m_uiBufferFill += iReceived;
//	ASSERT(m_uiBufferFill<RECV_BUFFER_SIZE);
	return iReceived;
}

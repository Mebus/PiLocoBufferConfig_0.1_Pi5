// ReceiveLineBuffer.h: Schnittstelle fr die Klasse CReceiveLineBuffer.
// $Id: ReceiveLineBuffer.h 996 2017-12-09 18:18:22Z pischky $
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECEIVELINEBUFFER_H__46D6C82C_DBDC_470B_9EF3_7A80E3F0106F__INCLUDED_)
#define AFX_RECEIVELINEBUFFER_H__46D6C82C_DBDC_470B_9EF3_7A80E3F0106F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "OsSpecific.h"


const int RECV_BUFFER_SIZE=1000;



class CReceiveLineBuffer  
{
public:
	CReceiveLineBuffer();
	virtual ~CReceiveLineBuffer(){}

protected:
	bool ProcessReceiveBuffer();
	int ReceiveToBuffer(SOCKET Socket);

private:
	void ProcessLine(char *pcLineEnd);
	virtual void OnRecvLine(char *pcLine)=0;

	char						m_acBuffer[RECV_BUFFER_SIZE];
	unsigned int				m_uiBufferFill;
};

#endif // !defined(AFX_RECEIVELINEBUFFER_H__46D6C82C_DBDC_470B_9EF3_7A80E3F0106F__INCLUDED_)

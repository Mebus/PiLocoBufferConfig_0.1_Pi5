// AbstractLoconetDevice.h: Schnittstelle fuer die Klasse CAbstractLoconetDevice.
// $Id: AbstractLoconetDevice.h 1186 2023-01-16 09:22:29Z pischky $
//
//////////////////////////////////////////////////////////////////////

#ifndef _ABSTRACT_LOCONET_DEVICE_H_
#define _ABSTRACT_LOCONET_DEVICE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAbstractLoconetDevice  
{
public:	// destruction
	virtual ~CAbstractLoconetDevice(){}
public: // abstract interface
	virtual int ReceivePacket(void *pvData, unsigned int uiMaxLength)=0;
	virtual bool SendPacket(void *pvData, unsigned int uiLength)=0;
};

#endif // _ABSTRACT_LOCONET_DEVICE_H_

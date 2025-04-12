//-------------------------------------------------------------------
// File name   IDisplay.h
//             $Id: DisplayFrontend.cpp 1214 2023-02-06 13:06:17Z pischky $
//-------------------------------------------------------------------
// Author      Stefan Bormann
//-------------------------------------------------------------------
// License:    This piece of code is part of the LoconetOverTcp 
//             project (http://loconetovertcp.sf.net) and therefore 
//             published under the terms of the GNU General Public 
//             Licence (http://www.gnu.org/licenses/gpl.html).
//             LocoNet is owned by Digitrax (http://www.digitrax.com)
//             Commercial use is subject to licensing by Digitrax!
//-------------------------------------------------------------------
// Description This abstract base class serves as interface for all
//             code that hassomething to tell the user.
//             Different implementations output to stdout, LCD display or system journal
//-------------------------------------------------------------------
// History     2022-11-20  SBor: Added
//-------------------------------------------------------------------

#include "DisplayFrontend.h"
#include "log.h" /* LOG_ERROR */

// make the publically accessible interface implementation instance, which forwards generically to the output classes
CDisplayFrontend Display;


void CDisplayFrontend::Generic(EDisplay enDisplay, const void *pvData, size_t uiLength,
                               int priority,            // default: -1
                               const char* freeText     // default: NULL
                              )
{
	for (IDisplayBackend* p=_root; p!=NULL; p=p->_next)
	{
		switch (enDisplay)
		{
			case DispErrorMessage:    p->ErrorMessage(priority, freeText);
				break;
			case DispReceivedFromPort:p->ReceivedFromPort(pvData, uiLength); 
				break;
			case DispReceivedMessage: p->ReceivedMessage(pvData, uiLength); 
				break;
			case DispSendMessage:     p->SendMessage(pvData, uiLength);
				break;
			case DispSentSuccess:     p->SentSuccess();
				break;
			case DispConnection:      p->Connection(uiLength);
				break;
			case DispDisconnection:   p->Disconnection(uiLength);
				break;
			case DispSendError:       p->SendError(freeText);
				break;
			case DispReceiveError:    p->ReceiveError(freeText);
				break;
			default:                  p->ErrorMessage(LOG_ERR, "Unknown enumerator in DisplayFrontend::Generic()");
				break;
		}
	}
}


void HexDump(char* target, const char* prefix, const char* postfix, const void* pvData, size_t uiLength)
{
	strcpy(target, prefix);

	const unsigned char* puc = (const unsigned char*)pvData;

	for (size_t ui=0; ui<uiLength; ui++)
	{
		sprintf(target+strlen(target), "%02X%s", *puc, ui==(uiLength-(unsigned int)1) ? postfix : " ");
		puc++;
	}
}

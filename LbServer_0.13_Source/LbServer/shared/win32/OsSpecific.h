// windows version of OsSpecific.h
// $Id: OsSpecific.h 996 2017-12-09 18:18:22Z pischky $

#ifndef _OS_SPECIFIC_H_
#define _OS_SPECIFIC_H_


/* Use of winsock.h requires linking of -lwsock32 on minGW.
 * 
 * We probably want to be using winsock2.h instead of winsock.h in which case
 * we have link to ws2_32.
 * (see: http://sourceforge.net/mailarchive/message.php?msg_id=13897434)
 */
#include <winsock.h>   // send()


#define CLOSESOCKET(s) closesocket(s)
#define SOCKLEN_T      int


#endif

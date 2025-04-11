// linux version of OsSpecific.h
// $Id: OsSpecific.h 996 2017-12-09 18:18:22Z pischky $

#ifndef _OS_SPECIFIC_H_
#define _OS_SPECIFIC_H_


#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>


#define HANDLE int
#define SOCKET int
#define INVALID_SOCKET (-1)   // windows version for unsigned SOCKET: "(SOCKET)(~0)"
#define SOCKET_ERROR   (-1)
#define CLOSESOCKET(s) close(s)
#define SOCKLEN_T      socklen_t


#endif

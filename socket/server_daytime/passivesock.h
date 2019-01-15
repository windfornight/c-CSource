#include <winsock.h>

#ifndef __PASSIVE_SOCK__H__
#define __PASSIVE_SOCK__H__

//passivesock allocate & bind a server socker using TCP or UDP
SOCKET passivesock(const char* service, const char* transport, int qlen);

SOCKET passiveUDP(const char *service);

SOCKET passiveTCP(const char* service, int qlen);

void errexit(const char*, ...);


#endif
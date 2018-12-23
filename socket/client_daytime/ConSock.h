#include <winsock.h>

#ifndef _CON_SOCK_H_
#define _CON_SOCK_H_




//print an error message and exit
void errexit(const char*, ...);

//connectsock - allocate & connext a socket usiing TCP or UDP
SOCKET connectsock(const char *host, const char *service, const char *transport);

//connect to a specified TCP service on a specified host
SOCKET connectTCP(const char *host, const char *service);

//connect to a specified UDP service on a specified host
SOCKET connectUDP(const char *host, const char *service);


#endif
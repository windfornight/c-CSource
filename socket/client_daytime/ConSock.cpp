#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock.h>

#include "ConSock.h"

SOCKET connectsock(const char *host, const char *service, const char *transport)
{
	struct hostent *phe;  //pointer to host information entry
	struct servent *pse;  //pointer to service information entry
	struct protoent *ppe; //pointer to protocol information entry
	struct sockaddr_in sin; //an Internet endpoint address
	int s, type; //socket descriptor and socket type

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

	/*Map service name to port number */
	if (pse = getservbyname(service, transport))
		sin.sin_port = pse->s_port;
	else if (sin.sin_port = htons((u_short)atoi(service)) == 0)
		errexit("can't get \" %s\" service entry\n", service);

	/*Map host name to IP address, allowing for dotted decimal*/
	if (phe = gethostbyname(host))
		memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
	else if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
		errexit("can't get \" %s \" host entry\n", host);

	/*Map protocol name to protocol number*/
	if ((ppe = getprotobyname(transport)) == 0)
		errexit("can't get \"%s\" protocol entry\n", transport);

	/*Use protocol to choose a socket type*/
	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

	/*Allocate a socket*/
	s = socket(PF_INET, type, ppe->p_proto);

	if (s == INVALID_SOCKET)
		errexit("can't create socket:%d\n", GetLastError());

	/*Connect the socket*/
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
		errexit("can't connext to %s.%s:%d\n", host, service, GetLastError());

	return s;
}

SOCKET connectTCP(const char *host, const char *service)
{
	return connectsock(host, service, "tcp");
}

SOCKET connectUDP(const char *host, const char *service)
{
	return connectsock(host, service, "udp");
}

void errexit(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	WSACleanup();
	exit(1);
}




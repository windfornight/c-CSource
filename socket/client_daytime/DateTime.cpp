#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>

#include "DayTime.h"
#include "ConSock.h"


#define LINELEN 128
#define WSVERS MAKEWORD(2, 0)
#define MSG "what daytime is it?\n"

void TCPtest(int argc, char *argv[])
{
	const char* host = "localhost";
	const char* service = "daytime";

	WSADATA wsadata;
	switch(argc) {
		case 1:
			host = "localhost";
			break;
		case 3:
			service = argv[2];
		case 2:
			host = argv[1];
			break;
		default:
			fprintf(stderr, "usage: TCPdaytime [host [port]]\n");
			exit(1);
	}	

	if (WSAStartup(WSVERS, &wsadata) != 0)
		errexit("WSAStartup failed\n");

	TCPdaytime(host, service);
	WSACleanup();
	return ;
}


void TCPdaytime(const char *host, const char *service)
{
	char buf[LINELEN + 1];  //buffer for one line of text
	SOCKET s;   //socket descriptor
	int cc;   //recv character count

	s = connectTCP(host, service);

	cc = recv(s, buf, LINELEN, 0);

	while (cc != SOCKET_ERROR && cc > 0)
	{
		buf[cc] = '\0';  //ensure null-termination
		(void)fputs(buf, stdout);
		cc = recv(s, buf, LINELEN, 0);
	}

	closesocket(s);
}

void UDPtest(int argc, char *argv[])
{
	const char* host = "localhost";
	const char* service = "daytime";
	WSADATA wsadata;
	switch (argc) {
	case 1:
		host = "localhost";
		break;
	case 3:
		service = argv[2];
	case 2:
		host = argv[1];
		break;
	default:
		fprintf(stderr, "usage:UDPdaytiem [host[port]]\n");
	}

	if (WSAStartup(WSVERS, &wsadata) != 0)
		errexit("WSAStartup failed\n");
	UDPdaytime(host, service);
	WSACleanup();
	return ;
}

void UDPdaytime(const char *host, const char *service)
{
	char buf[LINELEN + 1];
	SOCKET s;
	int n;

	s = connectUDP(host, service);
	(void)send(s, MSG, strlen(MSG), 0);
	n = recv(s, buf, LINELEN, 0);
	if (n == SOCKET_ERROR)
		errexit("revc failed: recv() error %d\n", GetLastError());
	else
	{
		buf[n] = '\0';
		(void)fputs(buf, stdout);
	}

	closesocket(s);
	return ;

}







#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock.h>

#include "ConSock.h"

/*
*	struct hostent
*	{
*	char *h_name;  //主机名，即官方域名
*	char **h_aliases;  //主机所有别名构成的字符串数组，同一IP可绑定多个域名
*	int h_addrtype; //主机IP地址的类型，例如IPV4（AF_INET）还是IPV6
*	int h_length;  //主机IP地址长度，IPV4地址为4，IPV6地址则为16
*	char **h_addr_list;  // 主机的ip地址，以网络字节序存储。若要打印出这个IP，需要调用inet_ntoa()
*	}

*	struct servent {
*	char  *s_name;		//服务名  
*	char **s_aliases;	//服务别名列表
*	int    s_port;		//端口号    
*	char  *s_proto;		//使用的协议
*	};

*	struct protoent {
*	char  *p_name;			//协议规范名
*	char **p_aliases;		//主机的别名 
*	int    p_proto;			//协议号
*	};

*	struct sockaddr_in {
*	short int sin_family;			// 地址族 
*	unsigned short int sin_port;	//端口号 
*	struct in_addr sin_addr;		// Internet地址 
*	unsigned char sin_zero[8];		// 与struct sockaddr一样的长度 
*	};

*/

// localhost  daytime   tcp
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




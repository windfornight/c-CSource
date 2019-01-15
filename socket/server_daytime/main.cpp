#include <stdio.h>
#include <time.h>

#include "passivesock.h"

#define WSVERS MAKEWORD(2, 0)

char buf[256];

int main(int argc, char* argv[])
{
	struct sockaddr_in fsin;
	char *service = "daytime";
	SOCKET sock;
	int alen;
	char *pts;
	time_t now;
	WSADATA wsadata;

	switch(argc)
	{
	case 1:
		break;
	case 2:
		service = argv[1];
		break;
	default:
		errexit("usage:UDPdaytimed [port]\n");
	}
	
	if(WSAStartup(WSVERS, &wsadata) != 0)
		errexit("WSAStartup failed\n");

	sock = passiveUDP(service);

	while(1)
	{
		alen = sizeof(struct sockaddr);
		if(recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&fsin, &alen) == SOCKET_ERROR)
			errexit("recvfrom:error %d\n", GetLastError());
		(void)time(&now);
		pts = ctime(&now);
		(void)sendto(sock, pts, strlen(pts), 0, (struct sockaddr*)&fsin, sizeof(fsin));
	}

	return 1;
}
#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "Type.h"

class UdpClient
{
	private:

		INT Sockfd;

		sockaddr_in Sock_addr;

	public:

		UdpClient();
		
		~UdpClient();

		BOOL CreateSocket(CHAR *ip, WORD port);
								
		UINT Send(const CHAR *buf, UINT buflen);
		
		UINT Recv(CHAR *buf, UINT buflen);
};


#endif

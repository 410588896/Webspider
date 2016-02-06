#include "Socket.h"
#include <iostream>

/******************************
 * Name		:UdpClient
 * Function :UdpClient Constructor
 * Args		:
 * Return	:
 * PS		:
******************************/
UdpClient::UdpClient():Sockfd(-1)
{
	memset (&Sock_addr, 0, sizeof (Sock_addr));
}

/******************************
 * Name		:~UdpClient
 * Function :UdpClient destructor
 * Args		:
 * Return	:
 * PS		:
******************************/
UdpClient::~UdpClient()
{
	if(Sockfd != -1)
		close(Sockfd);
}

/******************************
 * Name		:CreateSocket
 * Function :init socket
 * Args		:ip,port
 * Return	:
 * PS		:
******************************/
BOOL UdpClient::CreateSocket(CHAR *ip, WORD port)
{
	Sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	Sock_addr.sin_family = AF_INET;    
	
	if(!strcmp(ip,"0.0.0.0"))
		Sock_addr.sin_addr.s_addr = htons(INADDR_ANY);
	else
		Sock_addr.sin_addr.s_addr = inet_addr(ip);

	Sock_addr.sin_port = htons(port);    

	if(Sockfd == -1)
	{
		std::cout<<"Socket error!"<<std::endl;
		
		return false;
	}
	else
	{
		std::cout<<"Socket Create success!"<<std::endl;
		
		return true;
	}
}

/******************************
 * Name		:Send
 * Function :send data with udp
 * Args		:buf, buflen
 * Return	:
 * PS		:
******************************/
UINT UdpClient::Send(const CHAR *buf, UINT buflen)
{
	UINT Len = sendto(Sockfd, buf, buflen, 0, (struct sockaddr *)&Sock_addr, sizeof(struct sockaddr_in));
	
	return Len;
}

/******************************
 * Name		:Recv
 * Function :receive data with udp
 * Args		:buf, buflen
 * Return	:
 * PS		:
******************************/
UINT UdpClient::Recv(CHAR *buf, UINT buflen)
{
	UINT len = sizeof(struct sockaddr_in);

//	UINT Len = recvfrom(Sockfd, buf, buflen, MSG_DONTWAIT, (struct sockaddr *)&Sock_addr, &len);  //nonblock
	
	UINT Len = recvfrom(Sockfd, buf, buflen, 0, (struct sockaddr *)&Sock_addr, &len);   
	
	return Len;
}



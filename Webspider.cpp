#include "Webspider.h"

VOID *myprogress(VOID *arg)
{
	//init the variable	
	CHAR ip[IPLEN] = {0};
	CHAR iptest[IPLEN] = {0};
	CHAR url[URLLEN] = {0};
	UINT urllen = 0;
	Automachine *match;
	Mydb *Db;
	BloomFilter *Bf;
	Queue *Urlqueue;
	struct Webspider *webspider = (struct Webspider *)arg;
	match = webspider->match;
	Db = webspider->Db;
	Bf = webspider->Bf;
	Urlqueue = webspider->Urlqueue;
	strcpy(url, webspider->url);
	urllen = webspider->urllen;
	free(webspider);

	//local dns to gain ip
	Getipbyhost(url, ip);
	sprintf(iptest, "ping -c 1 %s", ip);
	if(!strcmp(ip, "0.0.0.0"))
	{
		printf("################%s->%d, IP Wrong!\n", url, urllen);
		return NULL;
	}
	INT ts = system(iptest);
	if(ts != 0)
	{
#ifdef DEBUG 
		printf("################%s->%d, IP Wrong!\n", url, urllen);
		return NULL;
#endif
	}
#ifdef DEBUG
	printf("################%s->%d:%s, IP OK!\n", url, urllen, ip);
#endif
	//construct the request
	INT sockfd;
	struct sockaddr_in servaddr;		
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) 
	{
		perror("socket error.");
		Urlqueue->Enqueue(url, urllen);
		close(sockfd);
		return NULL;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
	if(-1 == connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) 
	{
		//if connect error,drop the url
		perror("connect error.");
		close(sockfd);
		return NULL;;
	}
	Requestsend(sockfd, url, urllen);
	Responserecv(sockfd);	
}

UINT Requestsend(INT sockfd, CHAR *url, UINT urllen)
{
	CHAR request[MAXREQ] = {0};
	CHAR host[URLLEN] = {0};
	CHAR *dir; 
	if(dir = strchr(url,'/'))
	{
		memcpy(host, url, dir - url);
		sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\nAccept: %s\r\nConnection: %s\r\n\r\n", dir + 1, host, USERAGENT, ACCEPT, CONNECTION);		
	}
	else
	{
		memcpy(host, url, urllen);
		sprintf(request, "GET / HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\nAccept: %s\r\nConnection: %s\r\n\r\n", host, USERAGENT, ACCEPT, CONNECTION);
	}
#ifdef DEBUG
	printf("Request:%s\n", request);
#endif
	INT res = send(sockfd, request, strlen(request), 0);	

	return res;
}

UINT Responserecv(INT sockfd)
{
	CHAR response[MAXREQ] = {0};
	INT n = recv(sockfd, response, MAXREQ - 1, 0);
	printf("@@@@@@@@@@@@@@@@%d->%s\n", n, response);
	while(1)
	{
		if(n > 0)
		{
			memset(response, 0, MAXREQ);
			n = recv(sockfd, response, MAXREQ - 1, 0);
	printf("@@@@@@@@@@@@@@@@%d->%s\n", n, response);
		}	
		else if(n < 0)
		{
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
			{
				memset(response, 0, MAXREQ);
				n = recv(sockfd, response, MAXREQ - 1, 0);
			}
			else
			{
				close(sockfd);
				return -1;
			}
		}
		else
		{
			close(sockfd);
			return 0;
		}
	}
	return 0;
}


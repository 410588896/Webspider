#include "Webspider.h"

VOID *myprogress(VOID *arg)
{
	//init the variable	
	CHAR ip[IPLEN] = {0};
	CHAR iptest[IPLEN] = {0};
	CHAR url[URLLEN] = {0};
	UINT urllen = 0;
	Automachine *match;
	BloomFilter *Bf;
	Queue *Urlqueue;
	Queue *Visitedqueue;
	Regex *regex;
	struct Webspider *webspider = (struct Webspider *)arg;
	match = webspider->match;
	Bf = webspider->Bf;
	regex = webspider->regex;
	Urlqueue = webspider->Urlqueue;
	Visitedqueue = webspider->Visitedqueue;
	strcpy(url, webspider->url);
	urllen = webspider->urllen;
	free(webspider);

	//local dns to gain ip
	CHAR *dir;
	CHAR host[URLLEN] = {0};
	if(dir = strchr(url, '/'))
	{
		memcpy(host, url, dir - url);
	}
	else
	{
		memcpy(host, url, urllen);
	}
	Getipbyhost(host, ip);
	sprintf(iptest, "ping -c 1 %s", ip);
	if(!strcmp(ip, "0.0.0.0"))
	{
#ifdef DEBUG
		printf("################%s->%d, IP Wrong!\n", url, urllen);
#endif
		return NULL;
	}
	INT ts = system(iptest);
	if(ts != 0)
	{
#ifdef DEBUG 
		printf("################%s->%d, IP Wrong!\n", url, urllen);
#endif
		return NULL;
	}
#ifdef DEBUG
	printf("################%s->%d:%s, IP OK!\n", url, urllen, ip);
#endif

	//insert the visitedurl
//	CHAR sql[1024] = {0};
//	sprintf(sql, "insert into visitedurl(url) values ('%s')", url);
//	Db->ExecuteSql(sql);

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
	Responserecv(sockfd, match, Bf, Urlqueue, regex);	
	return NULL;
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

UINT Responserecv(INT sockfd, Automachine *match, BloomFilter *Bf, Queue *Urlqueue, Regex *regex)
{
	CHAR response[MAXREQ] = {0};
	INT n = recv(sockfd, response, MAXREQ - 1, 0);
	//respond head handle
	if(n <= 0)
	{
		return 0;
	}
	UINT flag = match->Machine_find(response, n);
	switch(flag)
	{
		case 1:
		case 2:
#ifdef DEBUG
			printf("#######%u->URL Normal!\n", flag);
#endif
			break;
		case 3:
		case 4:
#ifdef DEBUG
			printf("#######%u->URL Wait!\n", flag);
#endif
			sleep(10);
			break;
		case 5:
		case 6:
		case 7:
		case 8:
//get url
//url enqueue
#ifdef DEBUG
			printf("#######%u->URL Redirect!\n", flag);
#endif
			close(sockfd);
			return 0;
		default:
			close(sockfd);
			return 0;	
	}
	if(!strstr(response, "CONTENT-TYPE: TEXT/")) //just handle text mime
	{
		close(sockfd);
		return 0;
	}
	while(1)
	{
		if(n > 0)
		{
			memset(response, 0, MAXREQ);
			n = recv(sockfd, response, MAXREQ - 1, 0);
			INT len = 0;
			CHAR match[1024] = {0};
			INT matchlen = 0;
			for(len = 0; len < n;)
			{
				INT offset = regex->Find(response + len, match, &matchlen);	
				if(-1 == offset)
				{
					break;
				}
				else
				{
#ifdef DEBUG 
					printf("New URL:%s\n", match);
#endif
					if(!Bf->checkBit(match, matchlen))
					{
						Bf->setBit(match, matchlen);
						Urlqueue->Enqueue(match, matchlen);
					}
					len += offset;
				}
			}
		}	
		else if(n < 0)
		{
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
			{
				memset(response, 0, MAXREQ);
				n = recv(sockfd, response, MAXREQ - 1, 0);
				INT len = 0;
				CHAR match[1024] = {0};
				INT matchlen = 0;
				for(len = 0; len < n;)
				{
					INT offset = regex->Find(response + len, match, &matchlen);	
					if(-1 == offset)
					{
						break;
					}
					else
					{
#ifdef DEBUG 
						printf("New URL:%s\n", match);
#endif
						if(!Bf->checkBit(match, matchlen))
						{
							Bf->setBit(match, matchlen);
							Urlqueue->Enqueue(match, matchlen);
						}
						len += offset;
					}
				}
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


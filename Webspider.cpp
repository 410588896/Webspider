#include "Webspider.h"

VOID *Visitedurlhandle(VOID *arg)
{
	Queue **Visitedqueue = (Queue **)arg;
	CHAR sql[256] = {0};
	CHAR url[URLLEN] = {0};
	UINT urllen = 0;
	Mydb Db;
	Db.Initdb();
	for(INT i = 0; i < THREAD_NUM; i++,i = i % THREAD_NUM)	
	{
		memset(url, 0, URLLEN);
		if(Visitedqueue[i]->Dequeue(url, urllen))
		{
			if(strchr(url, '\'')||strchr(url, '\"'))
			{
				continue;
			}
			memset(sql, 0, 256);
			sprintf(sql, "insert into visitedurl(url) values ('%s')", url);
			Db.ExecuteSql(sql);
		}
	}	
}

VOID http_socket(CHAR *ip, CHAR *url, INT urllen, Automachine *match, BloomFilter *Bf, Queue *Urlqueue, Regex *regex)
{
	INT sockfd;
	struct sockaddr_in servaddr;		
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) 
	{
		perror("socket error.");
		Urlqueue->Enqueue(url, urllen);
		close(sockfd);
		return;
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
		return;
	}
	Requestsend(sockfd, url, urllen);
	Responserecv(sockfd, match, Bf, Urlqueue, regex);	
}

VOID *myprogress(VOID *arg)
{
	//init the variable	
	CHAR ip[IPLEN] = {0};
	CHAR iptest[IPLEN] = {0};
	CHAR originurl[URLLEN] = {0};
	UINT originurllen = 0;
	CHAR *url = NULL;
	INT urllen = 0;
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
	free(webspider);
	INT flag = 1;

	while(Urlqueue->Dequeue(originurl, originurllen))
	{
		if(strstr(originurl, "http://"))
		{
			url = originurl + 7;
			urllen = originurllen - 7;
			flag = 1;
		}	
		else if(strstr(originurl, "https://"))
		{
			url = originurl + 8;
			urllen = originurllen - 8;
			flag = 2;
			//sslsocket handle
		}
		else //no http:// or https:// we think it's http://
		{
			url = originurl;
			urllen = originurllen;
			flag = 1;
		}

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
			continue;
		}
		INT ts = system(iptest);
		if(ts != 0)
		{
#ifdef DEBUG 
			printf("################%s->%d, IP Wrong!\n", url, urllen);
#endif
			continue;
		}
#ifdef DEBUG
		printf("################%s->%d:%s, IP OK!\n", url, urllen, ip);
#endif

		//insert the visitedurl
		Visitedqueue->Enqueue(originurl, originurllen);	

		//construct the request
		if(flag == 1)
			http_socket(ip, url, urllen, match, Bf, Urlqueue, regex);
		else if(2 == flag)
			https_socket(ip, url, urllen, match, Bf, Urlqueue, regex);
		else
			continue;
		memset(ip, 0, IPLEN);
		memset(iptest, 0, IPLEN);
		memset(originurl, 0, URLLEN);
	}
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
		sprintf(request, "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\nAccept: %s\r\nConnection: %s\r\n\r\n", dir + 1, host, USERAGENT, ACCEPT, CONNECTION);		
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
	struct timeval timeout={3,0};//3s
	INT ret=setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(const CHAR*)&timeout,sizeof(timeout));
	INT n = recv(sockfd, response, MAXREQ - 1, 0);
	if(strstr(response, "Transfer-Encoding: chunked"))
	{
		//解码chunk
#ifdef DEBUG
		printf("###################chunk depress!\n");
#endif
		return 0;
	}
	else if(strstr(response, "Content-Encoding: gzip"))
	{
		//解压gzip
#ifdef DEBUG
		printf("###################No chunk ,gzip depress!\n");
#endif
		return 0;
	}
	else if(strstr(response, "Content-Encoding: deflate"))
	{
		//解压deflate
#ifdef DEBUG
		printf("###################No chunk ,deflate depress!\n");
#endif
		return 0;
	}

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
					printf("New URL:%s:%d\n", match, matchlen);
#endif
					if(!Bf->checkBit(match + 6, matchlen - 7))
					{
						Bf->setBit(match + 6, matchlen - 7);
						Urlqueue->Enqueue(match + 6, matchlen - 7);
					}
					len += offset;
				}
			}
		}	
		else if(n < 0 && n != -1)
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
						printf("New URL:%s:%d\n", match, matchlen);
#endif
						if(!Bf->checkBit(match + 6, matchlen - 7))
						{
							Bf->setBit(match + 6, matchlen - 7);
							Urlqueue->Enqueue(match + 6, matchlen - 7);
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


#include "Ssl.h"
#include "Webspider.h"

VOID ShowCerts(SSL *ssl)
{
	X509 *cert;
	CHAR *line;

	cert = SSL_get_peer_certificate(ssl);
	if (cert != NULL) 
	{
		printf("数字证书信息:\n");
		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		printf("证书: %s\n", line);
		free(line);
		line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		printf("颁发者: %s\n", line);
		free(line);
		X509_free(cert);
	} 
	else
		printf("无证书信息！\n");
}

VOID https_socket(CHAR *ip, CHAR *url, INT urllen, Automachine *match, BloomFilter *Bf, Queue *Urlqueue, Regex *regex)
{
//init ssl
	SSL_CTX *ctx;
	SSL *ssl;
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	ctx = SSL_CTX_new(SSLv23_client_method());
	if(ctx == NULL) 
	{
		ERR_print_errors_fp(stdout);
		return;
	}

	INT sockfd, len;
	struct sockaddr_in dest;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
#ifdef DEBUG
		perror("Socket create error!");
#endif
		return;
	}
#ifdef DEBUG
	printf("socket created\n");
#endif
	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(443);
	if(inet_aton(ip, (struct in_addr *) &dest.sin_addr.s_addr) == 0) 
	{
#ifdef DEBUG
		perror(ip);
#endif
		return;
	}
	if(connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) 
	{
#ifdef DEBUG
		perror("Connect error!");
#endif
		return;
	}

	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, sockfd);
	//create ssl connect
	if(SSL_connect(ssl) == -1)
		ERR_print_errors_fp(stderr);
	else 
	{
		printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
		ShowCerts(ssl);
	}

	struct timeval timeout={3,0};//3s
	INT ret=setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(const CHAR*)&timeout,sizeof(timeout));

	SSL_Requestsend(ssl, sockfd, url, urllen);

	SSL_Responserecv(ssl, sockfd, match, Bf, Urlqueue, regex);
	
	SSL_shutdown(ssl);
	SSL_free(ssl);
	close(sockfd);
	SSL_CTX_free(ctx);
}

UINT SSL_Requestsend(SSL *ssl, INT sockfd, CHAR *url, UINT urllen)
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
	INT res = SSL_write(ssl, request, strlen(request));

	return res;
}

UINT SSL_Responserecv(SSL *ssl, INT sockfd, Automachine *match, BloomFilter *Bf, Queue *Urlqueue, Regex *regex)
{
	CHAR response[MAXREQ] = {0};
	INT n = SSL_read(ssl, response, MAXREQ - 1);

	CHAR responsehead[1024] = {0};	
	CHAR *rn = strstr(response, "\r\n\r\n");	
	memcpy(responsehead, rn + 4, n - (rn + 4 - response));
	
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
			return 0;
		default:
			return 0;	
	}
	if(!strstr(response, "CONTENT-TYPE: TEXT/")) //just handle text mime
	{
		return 0;
	}

	if(strstr(response, "TRANSFER-ENCODING: CHUNKED"))
	{
		//解码chunk
#ifdef DEBUG
		printf("###################chunked depress!\n");
#endif
		return 0;
	}
	else if(strstr(response, "CONTENT-ENCODING: GZIP"))
	{
		//解压gzip
#ifdef DEBUG
		printf("###################No chunked ,gzip depress!\n");
#endif
		return 0;
	}
	else if(strstr(response, "CONTENT-ENCODING: DEFLATE"))
	{
		//解压deflate
#ifdef DEBUG
		printf("###################No chunked ,deflate depress!\n");
#endif
		return 0;
	}

	while(1)
	{
		if(n > 0)
		{
			memset(response, 0, MAXREQ);
			n = SSL_read(ssl, response, MAXREQ - 1);
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
				n = SSL_read(ssl, response, MAXREQ - 1);
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
				return -1;
			}
		}
		else
		{
			return 0;
		}
	}
	return 0;
}


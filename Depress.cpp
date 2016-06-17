#include "Depress.h"
#include "Webspider.h"

VOID DeChunk(INT sockfd, BloomFilter *Bf, Queue *Urlqueue, Regex *regex, CHAR *responsehead, INT reslen, INT flag)
{
	CHAR response[MAXREQ] = {0};
	CHAR *allresponse = (CHAR *)malloc(reslen * sizeof(CHAR));
	INT alllen = reslen;
	while(1)
	{
		INT n = recv(sockfd, response, MAXREQ, 0);	
		if(n > 0)
		{
			allresponse = (CHAR*)realloc(allresponse, n + reslen);
			alllen += n;
			memset(response, 0, MAXREQ);
			continue;	
		}	
		else if(n < 0 && n != -1)
		{
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
			{
				memset(response, 0, MAXREQ);
				n = recv(sockfd, response, MAXREQ - 1, 0);
				if(n <= 0)
					break;
				else
				{
					allresponse = (CHAR*)realloc(allresponse, n + reslen);
					alllen += n;
					memset(response, 0, MAXREQ);
					continue;		
				}
			}
			else
			{
				close(sockfd);
				break;
			}
		}
		else
		{
			close(sockfd);
			break;
		}
	}
	
	CHAR *decode;
	CHAR *dechunk;
	INT tmplen = alllen;
	switch(flag)
	{
		case 1:
			decode = (CHAR*)malloc(alllen * 2 * sizeof(CHAR));
			alllen = DeChunk((BYTE*)allresponse, alllen, (BYTE*)decode);
			free(allresponse);
			break;
		case 2:
			decode = (CHAR*)malloc(alllen * 10 * sizeof(CHAR));
			gzdecompress((BYTE*)allresponse, (LONG)tmplen, (BYTE *)decode, (LONG *)alllen);
			free(allresponse);
			break;
		case 3:
			decode = (CHAR*)malloc(alllen * 10 * sizeof(CHAR));
			dechunk = (CHAR *)malloc(alllen * 2 * sizeof(CHAR));
			tmplen = DeChunk((BYTE*)allresponse, alllen, (BYTE*)dechunk);
			gzdecompress((BYTE*)dechunk, (LONG)tmplen, (BYTE *)decode, (LONG *)alllen);
			free(allresponse);
			free(dechunk);
			break;
		case 4:
			decode = (CHAR*)malloc(alllen * 10 * sizeof(CHAR));
			deflatedecompress((BYTE*)allresponse, (LONG)tmplen, (BYTE *)decode, (LONG *)alllen);
			free(allresponse);
			break;
		case 5:
			decode = (CHAR*)malloc(alllen * 10 * sizeof(CHAR));
			dechunk = (CHAR *)malloc(alllen * 2 * sizeof(CHAR));
			tmplen = DeChunk((BYTE*)allresponse, alllen, (BYTE*)dechunk);
			deflatedecompress((BYTE*)dechunk, (LONG)tmplen, (BYTE *)decode, (LONG *)alllen);
			free(allresponse);
			free(dechunk);
			break;
		default:
			return;
	}

	INT len = 0;
	CHAR match[1024] = {0};
	INT matchlen = 0;
	for(len = 0; len < alllen;)
	{
		INT offset = regex->Find(decode + len, match, &matchlen);	
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
	free(decode);
}


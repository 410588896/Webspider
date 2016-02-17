#ifndef WEBSPIDER_H
#define WEBSPIDER_H

#include "Type.h"
#include "Mydb.h"
#include "Dns.h"
#include "Match.h"
#include "Socket.h"
#include "Queue.h"
#include "Threadpool.h"
#include "Bloomfilter.h"
#include "Regex.h"

#include <errno.h>

#define USERAGENT "Wget/1.10.2"  
//#define USERAGENT "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_8; en-us) AppleWebKit/534.50 (KHTML, like Gecko) Version/5.1 Safari/534.50"
#define ACCEPT "*/*"  
#define ACCEPTLANGUAGE "zh-cn,zh;q=0.5"  
#define ACCEPTENCODING "gzip,deflate"  
#define ACCEPTCHARSET "utf-8,gb2312;q=0.7,*;q=0.7"  
#define CONNECTION "keep-alive"  

#define MAXSIZE 400000000
#define MAXREQ 1024
#define URLLEN 1024
#define IPLEN 64
#define PORT 80

#define DEBUG

struct Webspider
{
	Automachine *match;
	BloomFilter *Bf;
	Queue *Urlqueue;
	Queue *Visitedqueue;
	Regex *regex;
	CHAR url[URLLEN];
	UINT urllen;		
};

void *myprogress(void *arg);

UINT Requestsend(INT sockfd, CHAR *url, UINT urllen);

UINT Responserecv(INT sockfd, Automachine *match, BloomFilter *Bf, Queue *Urlqueue, Regex *regex);

#endif


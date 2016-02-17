#include "Webspider.h"

INT main(INT args, CHAR *argv[])
{
/*******************initialize**********************/
//The automachine
	Automachine match;

	Httpreadconf(match);
#ifdef DEBUG
	printf("################Automachine init done\n");
#endif

//The database
	Mydb Db;
	
	Db.Initdb();
#ifdef DEBUG
	printf("################Database init done\n");
#endif

//The bloom filter
	BloomFilter Bf(MAXSIZE);
#ifdef DEBUG
	printf("################Bloom Filter init done\n");
#endif

//The queue
	Queue *Urlqueue[16];
	Queue *Visitedqueue[16];
	for(INT i = 0; i < 16; i++)
	{
		Urlqueue[i] = new Queue(1024, 512);
		Visitedqueue[i] = new Queue(1024, 512);
	}

	Db.Urlenqueue(*Urlqueue[0], Bf);
#ifdef DEBUG
	printf("################URL Queue init done\n");
#endif

//The regex
	Regex regex;
	regex.Construct();
#ifdef DEBUG
	printf("################Regex init done\n");
#endif
	
//The thread pool
	Manager manager(16);
	manager.Init();
#ifdef DEBUG
	printf("################Thread Pool init done\n");
#endif
/*****************************************************/

//Begin to request use threadpool  
//can epoll used here? Let's see later
	CHAR url[URLLEN] = {0};
	UINT urllen = 0;
	while(true)
	{
		for(INT i = 0; i < 16; i++,i = i % 16)
		{
			memset(url, 0, URLLEN);
			if(Urlqueue[i]->Dequeue(url, urllen))
			{
				struct Webspider *webspider = (struct Webspider *)malloc(sizeof(struct Webspider));
				memset(webspider, 0, sizeof(struct Webspider));
				webspider->match = &match;
				webspider->Bf = &Bf;
				webspider->Urlqueue = Urlqueue[i];
				webspider->Visitedqueue = Visitedqueue[i];
				webspider->regex = &regex;
				if(strstr(url, "http://"))
				{
					strcpy(webspider->url, url + 7);
					webspider->urllen = urllen - 7;
					manager.AddWorker(myprogress, webspider);
				}	
				else
				{
					strcpy(webspider->url, url + 8);
					webspider->urllen = urllen - 8;
					continue;
					//sslsocket handle
				}
			}	
			else
				continue;
		}
	}

/***********************destroy************************/
	manager.Join();
	match.Machine_destroy();
	for(INT i = 0; i < 16; i++)
	{
		delete Urlqueue[i];
		delete Visitedqueue[i];
	}
	regex.Destroy();	
/*****************************************************/

	return 0;
}



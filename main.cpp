#include "Queue.h"
#include "Mydb.h"
#include "Dns.h"
#include "Threadpool.h"
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
using namespace std;
void *myprocess (void *arg)
{
	printf("hahahahaha,threadid is 0x%x, working on task %d\n", (unsigned int)pthread_self (),*(int *) arg);
	sleep (1);/*休息一秒，延长任务的执行时间*/
	return NULL;
}
int main()
{
	Queue Wqueue(1024,512);
	CHAR url[3][64]={
		"www.baidu.com",\
		"www.sina.com.cn",\
		"www.qq.com"\
	};
	UINT len = 0;
	CHAR buf[1024] = {0};
	Wqueue.Enqueue(url[0], strlen(url[0]));
	Wqueue.Enqueue(url[1], strlen(url[1]));
	Wqueue.Dequeue(buf, len);
	cout<<"URL:"<<buf<<endl;
	Wqueue.Enqueue(url[2], strlen(url[2]));
	memset(buf,0,1024);
	Wqueue.Dequeue(buf, len);
	cout<<"URL:"<<buf<<endl;
	memset(buf,0,1024);
	Wqueue.Dequeue(buf, len);
	cout<<"URL:"<<buf<<endl;
	memset(buf,0,1024);
	Wqueue.Dequeue(buf, len);
	Mydb db;
	db.Initdb();
	//db.ExecuteSql("select * from dns");
	//char abc[256] = "ping -c 1 119.75.218.70";
	CHAR ip[256] = {0};
	Getipbyhost(url[1], ip);
	char abc[256] = {0};
	sprintf(abc,"ping -c 1 %s",ip);
	printf("%s\n",abc);
	//std::cout<<system(abc)<<std::endl;
	INT bbb = system(abc);
	if(bbb == 0)
	{
		printf("ip ok!\n");;
	}
	else
		printf("ip wrong!\n");
	Manager manager(3);
	manager.Init();
	int aaaa[6] = {1,2,3,4,5};
	manager.AddWorker(myprocess,&aaaa[0]);
	manager.AddWorker(myprocess,&aaaa[1]);
	manager.AddWorker(myprocess,&aaaa[2]);
	manager.AddWorker(myprocess,&aaaa[3]);
	manager.AddWorker(myprocess,&aaaa[4]);
	//manager.Destroy();
	manager.Join();
	return 0;
}

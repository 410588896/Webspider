#include "Queue.h"
#include "Mydb.h"
#include "Dns.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
using namespace std;
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
	return 0;
}

#include "Dns.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <iostream>

CHAR DNS_IP[16] =  "114.114.114.114";
const WORD DNS_PORT  = 53;
#define MAXLEN 1024

/******************************
 * Name		:Getipbyhost
 * Function :use host to get ip 
 * Args		:host ip
 * Return	:
 * PS		:
******************************/
VOID Getipbyhost(CHAR *host, CHAR *ip)
{
	Mydb mydb;
	
	mydb.Initdb();	

	CHAR sql[256] = {0};
	
	CHAR ip_tmp[256] = {0};

	CHAR ping[64] = {0};

	INT num = 0;

	INT ret = 0;
	
	sprintf(sql,"select count(*) from dns where host = '%s'", host);

	mydb.Getnum(sql,num);

	if(num != 0)
	{
		memset(sql, 0, 256);

		sprintf(sql,"select `ipv4` from dns where host = '%s'", host);
	
		mydb.Getip(sql,ip_tmp); 

		sprintf(ping, "ping -c 1 %s", ip_tmp);

		ret = system(ping);

		if(ret == 0)	
		{
			strcpy(ip, ip_tmp);
			
			return;
		}
		else
		{
			Getipbydns(host, ip, num, mydb);
		}
	}
	else
	{
			Getipbydns(host, ip, num, mydb);
	}
	
	return;
}

/******************************
 * Name		:Getipbydns
 * Function :use host to get ip by sending dns packet
 * Args		:host ip num
 * Return	:
 * PS		:
******************************/
VOID Getipbydns(CHAR *host, CHAR *ip, INT num, Mydb &Db)
{
	CHAR buffer[MAXLEN] = {0};

	CHAR rebuffer[MAXLEN] = {0};

	CHAR des[128] = {0};

	struct dnshead dhead;

	struct dnshead *redns;

	UdpClient udpsocket;
	
	udpsocket.CreateSocket(DNS_IP, DNS_PORT);

	//build dns packet

	time_t ident;
	
	time(&ident);	
		
	CHAR *p = (CHAR *)&dhead.trans_id;

	*p = ident;

	*(p + 1) = ident>>8;	

	dhead.flag = htons(0x0100);

	dhead.que_num = htons(0x0001);

	dhead.ans_num = 0x0000;

	dhead.trust_count = 0x0000;

	dhead.add_count = 0x0000;

	memcpy(buffer, &dhead, sizeof(struct dnshead));

	INT hostlen = ns_to_tran_ns(des, host);

	memcpy(buffer + sizeof(struct dnshead), des, hostlen);

	WORD tmp = htons(0x0001);	

	WORD *p_tmp = (WORD*)(buffer + sizeof(struct dnshead) + hostlen);

	*p_tmp = tmp;

	*(p_tmp + 1) = tmp;

	udpsocket.Send((const CHAR*)buffer, (UINT)(sizeof(struct dnshead) + 2 * sizeof(WORD) + hostlen));

	udpsocket.Recv(rebuffer, (UINT)(MAXLEN - 1));

	redns = (struct dnshead *)rebuffer;
	
	if((ntohs(redns->flag))&(0x000F))
	{	
		strcpy(ip, "0.0.0.0");
		std::cout<<"Host doesn't exists!"<<std::endl;
		return;
	}
	else
	{
		struct otherhead *oth = (struct otherhead *)(rebuffer + sizeof(struct dnshead));
		if((ntohs(oth->answ) == 0)&&(ntohs(oth->auth) == 0)&&(ntohs(oth->addi) == 0))
		{
			strcpy(ip,"0.0.0.0");
			std::cout<<"Host doesn't exists!"<<std::endl;
			return;
		}
		struct ans_head *anshead = (struct ans_head*)(rebuffer + sizeof(struct dnshead) + hostlen + 4);
		INT len = 0;
		while(ntohs(anshead->type) != 0)
		{
			if(ntohs(anshead->type) == 1)
			{
				INT *ip_tmp = (INT *)(anshead + 1);
				in_addr ipaddr;
				ipaddr. s_addr = *ip_tmp;//no need to change net address to host address because the ip is stored reserved in the dns tree.
				strcpy(ip, inet_ntoa(ipaddr));
				Addiptodb(host, ip, Db);
				return;
			}
			else
			{
				len = ntohs(anshead->len);
				CHAR *tmp = (CHAR *)(anshead + 1);
				tmp = tmp + len;
				anshead = (struct ans_head *)tmp;
			}
		}
		strcpy(ip,"0.0.0.0");
                std::cout<<"Host doesn't exists!"<<std::endl;
		return;
	}

	return;
}

/******************************
 * Name		:Getipbyhost
 * Function :use host to get ip 
 * Args		:host ip
 * Return	:
 * PS		:
******************************/
VOID Addiptodb(CHAR *host, CHAR *ip, Mydb &Db)
{
	INT num = 0;
	
	CHAR sql[256] = {0};
	
	sprintf(sql, "select count(*) from dns where host = '%s'", host);
	
	Db.Getnum(sql, num);
	
	if(0 == num)
	{
		memset(sql, 0, 256);
		sprintf(sql, "insert into dns (host, ipv4) values ('%s', '%s')", host, ip);
		Db.ExecuteSql(sql);
	}
	else
	{
		memset(sql, 0, 256);
		sprintf(sql, "update dns set ipv4 = '%s' where host = '%s'", host, ip);
		Db.ExecuteSql(sql);	
	}
	return;
} 

/******************************
 * Name		:ns_to_tran_ns
 * Function :transport the host to the host used by network
 * Args		:des source
 * Return	:len
 * PS		:
******************************/
INT ns_to_tran_ns(CHAR *des, CHAR *source)//把一个域名转换成网络传输的格式
{
	INT num= 0;

	INT len =0; 

	CHAR *p = source;

	while(*p != '\0')
	{   
		while(*p != '.' && *p != '\0')
		{   
			p++;

			num++;

			if(num>= 64)
			{
				return -1;
			}
		}   
		memcpy(des+len,&num,sizeof(CHAR));

		len += 1;

		memcpy(des+len,source,num);

		len += num;

		p++;

		source = p;

		num = 0;
	}   
	memset(des+len,0,sizeof(CHAR));

	len += 1;

	return len;
}

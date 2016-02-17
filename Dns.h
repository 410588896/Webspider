#ifndef DNS_H
#define DNS_H

#include "Socket.h"
#include "Type.h"
#include "Mydb.h"

struct dnshead
{
	WORD trans_id;
	WORD flag;
	WORD que_num;
	WORD ans_num;
	WORD trust_count;
	WORD add_count;
};

struct otherhead
{
	WORD ques;
	WORD answ;
	WORD auth;
	WORD addi;	
};

struct ans_head
{
	WORD name;
	WORD type;
	WORD clas;
	WORD ttl1;
	WORD ttl2;
	WORD len;
};
	
INT ns_to_tran_ns(CHAR *des, CHAR *source);
VOID Getipbyhost(CHAR *host, CHAR *ip);
VOID Getipbydns(CHAR *host, CHAR *ip, INT num, Mydb &Db);
VOID Addiptodb(CHAR *host, CHAR *ip, Mydb &Db);

#endif


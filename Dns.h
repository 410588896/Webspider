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

INT ns_to_tran_ns(CHAR *des, CHAR *source);
VOID Getipbyhost(CHAR *host, CHAR *ip);
VOID Getipbydns(CHAR *host, CHAR *ip, INT num);

#endif


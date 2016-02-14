#include "Type.h"
#include "Mydb.h"
#include "Dns.h"
#include "Match.h"
#include "Socket.h"
#include "Queue.h"
#include "Threadpool.h"
#include <regex.h>

INT main(INT args, CHAR *argv[])
{
/*******************initialize**********************/
//The automachine
	Automachine match;

	Httpreadconf(match);
	
	return 0;
}

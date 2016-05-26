#ifndef MYDB_H
#define MYDB_H

#include <mysql/mysql.h>

#include "Type.h"
#include "Queue.h"
#include "Bloomfilter.h"

class Mydb
{
	public:
		
		Mydb();
		
		BOOL Initdb();
		
		BOOL ExecuteSql(const char *sql);
		
		BOOL Getip(CHAR *host, CHAR *ip);

		BOOL Getnum(CHAR *host, INT &num);
	
		BOOL Urlenqueue(Queue **Urlqueue, BloomFilter &Bf);

		~Mydb();
	private:
		
		MYSQL *connection;
		
		MYSQL_RES *res;
		
		MYSQL_ROW row;
};

#endif

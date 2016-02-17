#include "Mydb.h"

#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/******************************
 * Name		:Read_conf
 * Function :read the conf file
 * Args		:
 * Return	:
 * PS		:
******************************/
VOID read_conf(CHAR *dbname, CHAR *host, CHAR *user, CHAR *pass)
{
	CHAR buffer[256] = {0};  
	
	std::ifstream in("webspider.conf");  
	
	if(!in.is_open())  
	{ 
		std::cout<<"read conf error"<<std::endl; 
		
		exit (1); 
	}  

	while(!in.eof())  
	{  
		memset(buffer, 0, 256);

		in.getline (buffer,128);

		if(buffer[0] == '#')
			continue;	
		
		if(strstr(buffer, "DBNAME") != NULL)
		{
			CHAR *p_start = NULL;
			
			CHAR *p_end = NULL;

			p_start = strstr(buffer, "=");
			
			p_start = p_start + 2;
			
			p_end = strstr(p_start, ";");
			
			memcpy(dbname, p_start, p_end - p_start);

			continue;
		}
		else if(strstr(buffer, "HOST") != NULL)
		{
			CHAR *p_start = NULL;
			
			CHAR *p_end = NULL;

			p_start = strstr(buffer, "=");
			
			p_start = p_start + 2;
			
			p_end = strstr(p_start, ";");
			
			memcpy(host, p_start, p_end - p_start);

			continue;
		}
		else if(strstr(buffer, "DBUSER") != NULL)
		{
			CHAR *p_start = NULL;
			
			CHAR *p_end = NULL;

			p_start = strstr(buffer, "=");
			
			p_start = p_start + 2;
			
			p_end = strstr(p_start, ";");
			
			memcpy(user, p_start, p_end - p_start);

			continue;
		}
		else if(strstr(buffer, "DBPASS") != NULL)
		{
			CHAR *p_start = NULL;
			
			CHAR *p_end = NULL;

			p_start = strstr(buffer, "=");
			
			p_start = p_start + 2;
			
			p_end = strstr(p_start, ";");
			
			memcpy(pass, p_start, p_end - p_start);

			continue;
		}
	}  
	in.close(); 
}

/******************************
 * Name		:Mydb
 * Function :Mydb Constructor
 * Args		:
 * Return	:
 * PS		:
******************************/
Mydb::Mydb()
{	
	//init db handle
	connection = mysql_init(NULL);

	if(connection == NULL)
	{
		std::cout<<"mysql_init error!"<<std::endl;
		
		exit(1);
	}
}

/******************************
 * Name		:~Mydb
 * Function :Mydb destructor
 * Args		:
 * Return	:
 * PS		:
******************************/

Mydb::~Mydb()
{	
	//close db handle
	if(connection != NULL)
	{
		mysql_close(connection);
		connection = NULL;
	}
}

/******************************
 * Name		:Initdb
 * Function :connect the db
 * Args		:host user dbname dbpass
 * Return	:
 * PS		:
******************************/
BOOL Mydb::Initdb()
{	
	CHAR dbname[256] = {0};

	CHAR host[256] = {0};

	CHAR user[256] = {0};

	CHAR dbpass[256] = {0};

	read_conf(dbname, host, user, dbpass);
	
	connection = mysql_real_connect(connection , host, user, dbpass, dbname, 0 , NULL , 0);

	if(connection == NULL)
	{
		std::cout<<"mysql connect error!"<<std::endl;
		exit(1);
	}
	else
	{
		std::cout<<"host:"<<host<<std::endl;
		std::cout<<"user:"<<user<<std::endl;
		std::cout<<"dbpass:"<<dbpass<<std::endl;
		std::cout<<"dbname:"<<dbname<<std::endl;
		std::cout<<"mysql connect success!"<<std::endl;
	}

	return true;
}

/******************************
 * Name		:Queue
 * Function :Queue Constructor
 * Args		:Size
 * Return	:
 * PS		:
******************************/
BOOL Mydb::ExecuteSql(const char *sql)
{
	if(mysql_query(connection, "set names utf8"))
	{
        	fprintf(stderr, "%d: %s\n",mysql_errno(connection), mysql_error(connection));
    	}

	INT t = mysql_query(connection, sql);
	
	if(t)
	{
		std::cout<<"Execute sql Error making query:"<<mysql_error(connection)<<std::endl;
//		this.Initdb();	
exit(1);
	}
	else
	{	
#ifdef DEBUG
		printf("Insert success!\n");
#endif
	}
	return true;
}

/******************************
 * Name		:Getip
 * Function :get ip with mysql
 * Args		:host ip
 * Return	:
 * PS		:
******************************/
BOOL Mydb::Getip(CHAR *sql, CHAR *ip)
{
	if(mysql_query(connection, "set names utf8"))
	{
        	fprintf(stderr, "%d: %s\n",mysql_errno(connection), mysql_error(connection));
  	}

	INT t = mysql_query(connection, sql);
	
	if(t)
	{
		std::cout<<"Get ip Error making query:"<<mysql_error(connection)<<std::endl;
		
		exit(1);
	}
	else
	{	
		//初始化逐行的结果集检索
		res = mysql_use_result(connection);
		
		if(res)
		{
			//mysql_field_count(connection)   返回作用在连接上的最近查询的列数
			for(INT i = 0 ;i < mysql_field_count(connection); i++)
			{	
				//检索一个结果集合的下一行
				row = mysql_fetch_row(res);		
				if(row <= 0)
				{
					break;
				}
				//mysql_num_fields(res)  函数返回结果集中字段的数
				for(INT r = 0; r < mysql_num_fields(res); r++)
				{
					strcpy(ip, row[r]);
				}
			}
		}
		//释放结果集使用的内存
		mysql_free_result(res);
	}
	return true;
}

/******************************
 * Name		:Urlenqueue
 * Function :get url with mysql
 * Args		:
 * Return	:
 * PS		:
******************************/
BOOL Mydb::Urlenqueue(Queue &Urlqueue, BloomFilter &Bf)
{
	CHAR sql[1024] = "select url from url";
	if(mysql_query(connection, "set names utf8"))
	{
 	       fprintf(stderr, "%d: %s\n",mysql_errno(connection), mysql_error(connection));
    	}	

	INT t = mysql_query(connection, sql);
	
	if(t)
	{
		std::cout<<"Url enqueue Error making query:"<<mysql_error(connection)<<std::endl;
		
		exit(1);
	}
	else
	{	
		//初始化逐行的结果集检索
		res = mysql_use_result(connection);
		
		if(res)
		{
			//mysql_field_count(connection)   返回作用在连接上的最近查询的列数
			for(INT i = 0 ;i < mysql_field_count(connection); i++)
			{	
				//检索一个结果集合的下一行
				while(row = mysql_fetch_row(res))
				{
					if(row <= 0)
					{
						break;
					}
					//mysql_num_fields(res)  函数返回结果集中字段的数
					for(INT r = 0; r < mysql_num_fields(res); r++)
					{
						Urlqueue.Enqueue(row[r], strlen(row[r]));
						Bf.setBit(row[r], strlen(row[r]));
						std::cout<<"Enqueue:"<<row[r]<<std::endl;		
					}
				}
			}
		}
		//释放结果集使用的内存
		mysql_free_result(res);
	}
	return true;
}

/******************************
 * Name		:Getnum
 * Function :get host num with mysql
 * Args		:host num
 * Return	:
 * PS		:
******************************/
BOOL Mydb::Getnum(CHAR *sql, INT &num)
{
	if(mysql_query(connection, "set names utf8"))
	{
 	       fprintf(stderr, "%d: %s\n",mysql_errno(connection), mysql_error(connection));
    	}

	INT t = mysql_query(connection, sql);
	
	if(t)
	{
		std::cout<<"Get num Error making query:"<<mysql_error(connection)<<std::endl;
		
		exit(1);
	}
	else
	{	
		//初始化逐行的结果集检索
		res = mysql_use_result(connection);
		
		if(res)
		{
			//mysql_field_count(connection)   返回作用在连接上的最近查询的列数
			for(INT i = 0 ;i < mysql_field_count(connection); i++)
			{	
				//检索一个结果集合的下一行
				row = mysql_fetch_row(res);		
				if(row <= 0)
				{
					break;
				}
				//mysql_num_fields(res)  函数返回结果集中字段的数
				for(INT r = 0; r < mysql_num_fields(res); r++)
				{
					num = atoi(row[r]);
				}
			}
		}
		//释放结果集使用的内存
		mysql_free_result(res);
	}
	return true;
}


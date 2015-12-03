#include <stdlib.h>

#include "Threadpool.h"

//This thread pool doesn't support expand or shrink
//the number of threads dynamically. I will add the function in
//the lately version.The Maxnum and Minnum are used
//to support the dynamic change of the threads' number.
//Thank you for using.


/******************************
 * Name		:Manager
 * Function :Manager Constructor
 * Args		:num
 * Return	:
 * PS		:
******************************/
Manager::Manager(INT num)
{
	Maxnum = 1024;Minnum = 16;
	if(num >= Maxnum)
		Totalnum = Maxnum;
	else if(num < Minnum)
		Totalnum = Minnum;
	else
		Totalnum = num;

	Freenum = Totalnum;

	Busynum = 0;

	Pid = new pthread_t[Totalnum];

	Head = (CTask *)malloc(sizeof(CTask));
}

/******************************
 * Name		:~Manager 
 * Function :Manager destructor
 * Args		:
 * Return	:
 * PS		:
******************************/
Manager::~Manager()
{
	delete Pid;	

	free(Head);
}

/******************************
* Name		:Init 
* Function  :init the thread pool
* Args		:
* Return	:
* PS		:
******************************/
INT Manager::Init()
{
	pthread_mutex_init(&queue_lock, NULL);
	
	pthread_cond_init(&queue_ready, NULL);


}

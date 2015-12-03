#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "Type.h"
#include <pthread.h>

//task queue
typedef struct worker
{
  	VOID *(*process)(VOID *arg);

	VOID *arg;

	struct worker *next;
}CTask;

//the manager of thread pool
class Manager
{
	private:
		
		pthread_t *Pid;

		pthread_mutex_t queue_lock;  
		
		pthread_cond_t queue_ready;  

		INT Totalnum;

		INT Maxnum;

		INT Minnum;

		INT Freenum;

		INT Busynum;

		CTask *Head;	
	
	public:
	
		Manager(INT num = 16);

		~Manager();

		INT Init();

		INT AddWorker(VOID *(*process)(VOID *arg), VOID *arg);

		INT Destroy();

};


#endif

#include <stdlib.h>
#include <stdio.h>

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

	Shutdown = false;

	Tasknum = 0;

	Pid = new pthread_t[Totalnum];
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

	Head = NULL;

	for (INT i = 0; i < Totalnum; i++)
	{ 
		pthread_create(&Pid[i], NULL, thread_routine, (VOID*)this);
	}
}

/******************************
* Name		:AddWorker
* Function  :add new task to the queue 
* Args		:process arg
* Return	:
* PS		:
******************************/
INT Manager::AddWorker(VOID *(*process)(VOID *arg), VOID *arg)
{
	CTask *newtask = (CTask *)malloc(sizeof(CTask));

	newtask->process = process;

	newtask->arg = arg;

	newtask->next = NULL;

	pthread_mutex_lock(&queue_lock);

	CTask *tmp = Head;

	if(Head != NULL)
	{
		while (tmp->next != NULL)
			tmp = tmp->next;
		
		tmp->next = newtask;
	}
	else
		Head = newtask;

	Tasknum++;

	pthread_mutex_unlock(&queue_lock);

	pthread_cond_signal(&queue_ready);

	return 0;
}

/******************************
* Name		:thread_routine 
* Function  :fun to excute the task
* Args		:arg
* Return	:
* PS		:
******************************/
VOID *thread_routine(VOID *arg)
{
	Manager *manager = (Manager*)arg;

	printf ("starting thread 0x%x\n", (UINT)pthread_self());
	while (1)
	{
		pthread_mutex_lock(&(manager->queue_lock));
		
		while ((manager->Tasknum) == 0 && !(manager->Shutdown))
		{
			printf("thread 0x%x is waiting\n", (UINT)pthread_self());

			pthread_cond_wait (&(manager->queue_ready), &(manager->queue_lock));
		}
		
		if (manager->Shutdown)
		{
			pthread_mutex_unlock (&(manager->queue_lock));
			
			printf("thread 0x%x will exit\n", (UINT)pthread_self());
			
			pthread_exit(NULL);
		}
		printf("thread 0x%x is starting to work\n", (UINT)pthread_self());
		
		manager->Tasknum--;

		CTask *worker = manager->Head;

		manager->Head = worker->next;

		pthread_mutex_unlock (&(manager->queue_lock));
		
		(*(worker->process))(worker->arg);
		
		free (worker);
		
		worker = NULL;
	}

	pthread_exit (NULL);
}

/******************************
* Name		:Destroy
* Function  :destroy the thread pool
* Args		:
* Return	:
* PS		:
******************************/
INT Manager::Destroy()
{
	if(Shutdown)
		return -1;

	Shutdown = 1;

	pthread_cond_broadcast (&queue_ready);

	for (INT i = 0; i < Totalnum; i++)
		pthread_join(Pid[i], NULL);

	CTask *tmp = NULL;
	
	while(Head != NULL)
	{
		tmp = Head;

		Head = Head->next;
		
		free (tmp);
	}
	
	pthread_mutex_destroy(&queue_lock);
	
	pthread_cond_destroy(&queue_ready);

	return 0;	
}

/******************************
* Name		:Join
* Function  :recover the thread pool
* Args		:
* Return	:
* PS		:
******************************/
VOID Manager::Join()
{
	for(INT i = 0; i < Totalnum; i++)
		pthread_join(Pid[i], NULL);
}


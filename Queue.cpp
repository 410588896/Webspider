#include "Queue.h"

#include <iostream>

#include <malloc.h>

#include <stdlib.h>

#include <string.h>

/******************************
 * Name		:Queue
 * Function :Queue Constructor
 * Args		:Size
 * Return	:
 * PS		:
******************************/
Queue::Queue(UINT Qsize, UINT PQsize)
{
	Maxsize = Qsize;

	Psize = PQsize;
	
	Queptr = (CHAR*)malloc(Maxsize * Psize);

	memset(Queptr, 0, Maxsize * Psize * sizeof(CHAR));

	Front = Rear = 0;
}

/******************************
 * Name		:~Queue
 * Function :Queue Destructor
 * Args		:
 * Return	:
 * PS		:
******************************/
Queue::~Queue()
{
	if(Queptr)
		free(Queptr);
}

/******************************
 * Name		:IsFull
 * Function :judge queue full
 * Args		:
 * Return	:True or False
 * PS		:
******************************/
BOOL Queue::IsFull()
{
	if((Rear + 1) % Maxsize == Front)
		return true;
	else
		return false;
}

/******************************
 * Name		:Enqueue
 * Function :Queue Insert
 * Args		:
 * Return	:True or False
 * PS		:
******************************/
BOOL Queue::IsEmpty()
{
	if(Rear == Front)
		return true;
	else 
		return false;
}

/******************************
 * Name		:Enqueue
 * Function :Queue Insert
 * Args		:Url, Len
 * Return	:True or False
 * PS		:
 * |		   |
 * |Url Length |
 * |-----------|
 * |		   |	
 * |Url content|
 * |           |
******************************/
VOID Queue::Enqueue(CHAR *Url, UINT Len)
{
	if(!IsFull())
	{
		UINT *plen = (UINT *)(Queptr + Rear * Psize);
		*plen = Len;
		memcpy((Queptr + Rear * Psize + sizeof(UINT)), Url, Len);
		Rear = (Rear + 1) % Maxsize;
	}
	else
	{
		Maxsize = Maxsize + 16; 
		Queptr = (CHAR*)realloc(Queptr, Maxsize * Psize * sizeof(CHAR));
		if(Queptr == NULL)	
		{
			std::cout<<"Queue is full and can not expand!"<<std::endl;
			exit(-1);
		}
	}
}

/******************************
 * Name		:Dequeue
 * Function :Queue delete
 * Args		:
 * Return	:True or False
 * PS		:
******************************/
BOOL Queue::Dequeue(CHAR *Url, UINT &Len)
{
	if(!IsEmpty())
	{
		UINT *plen = (UINT*)(Queptr + Front * Psize);
		Len = *plen;
		memcpy(Url, Queptr + Front * Psize + sizeof(UINT), Len);
		Front = (Front + 1) % Maxsize;
		return true;
	}		
	else
	{
		std::cout<<"The queue is empty!"<<std::endl;
		return false;
	}
}

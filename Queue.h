//queue define

#ifndef QUEUE_H

#define QUEUE_H

#include "Type.h"

class Queue
{
	private:
		
		//queue head

		UINT Front;

		//queue tail

		UINT Rear;

		UINT Maxsize;

		UINT Psize;

		CHAR *Queptr;

	public:

		Queue(UINT Qsize = 1024, UINT PQsize = 128);

		~Queue();
			
		VOID Enqueue(CHAR *buf, UINT Len);

		BOOL Dequeue(CHAR *buf, UINT &Len);

		BOOL IsFull();

		BOOL IsEmpty();	
};

#endif


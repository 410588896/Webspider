#ifndef MATCH_H
#define MATCH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Type.h"

typedef struct pModel Model;
typedef struct pNode Node;

struct pModel
{
	Node *Parent;
	Node *Next;
	Node *Tail;
	Model *Fail;
	Model *Cparent;
	UINT No;
	UINT End;
};

struct pNode
{
	CHAR Str;
	Node *Next;
	Model *Nmodel;
	Model *Pmodel;
};

struct Modelqueue
{
	Model *Current;
	struct Modelqueue *Next;
	struct Modelqueue *Prev;		
};

class MQueue
{
	private:
		struct Modelqueue *Moqueue;
		Modelqueue *Head;
		Modelqueue *Tail;
	public:
		MQueue();
		
		~MQueue();

		VOID Push(Model *Node);
			
		Model *Pop();
};

class Automachine 
{
	private:
		Model *root;
	
	public:
		Automachine();

		~Automachine();

		VOID Machine_prepare(UINT flag, CHAR *gjz, UINT Len);

		VOID Machine_construct();

		UINT Machine_find(CHAR *Str, UINT Len);

		VOID Machine_destroy();
};

VOID Httpreadconf(Automachine &match);

#endif


#include "Match.h"

#define NO_CASE_SENSITIVE

/*
**This version supports no-case sensitive 
**can find mutiple models
**just support english words
**haven't support chinese and other characters
**haven't do the same string detect this will be done later
*/

/******************************
 * Name		:Automachine
 * Function :Automachine Constructor
 * Args		:
 * Return	:
 * PS		:
******************************/
Automachine::Automachine()
{
	root = (Model *)malloc(sizeof(Model));
	memset(root, 0, sizeof(Model));
	root->Fail = root;
}

/******************************
 * Name		:Automachine
 * Function :Automachine destructor
 * Args		:
 * Return	:
 * PS		:
******************************/
Automachine::~Automachine()
{
	free(root);

	root = NULL;
}

/******************************
 * Name		:Machine_prepare
 * Function :Automachine prepare
 * Args		:
 * Return	:
 * PS		:
******************************/
VOID Automachine::Machine_prepare(UINT flag, CHAR *gjz, UINT Len)
{
#ifdef NO_CASE_SENSITIVE
	for(INT i = 0; i < Len; i++)
	{
		if((gjz[i] >= 'a') && (gjz[i] <= 'z'))
		{
			gjz[i] = gjz[i] - 'a' + 'A';
		}
	}
#endif	

//construct trie
	Model *parent = root;
	INT i = 0;
	for(i = 0; i < Len; i++)
	{
		Node *Ptmp = parent->Next;
		while(Ptmp)
		{
			if(Ptmp->Str == gjz[i])//duplicate removal
				break;
			Ptmp = Ptmp->Next;
		}
		if(Ptmp)
		{
			parent = Ptmp->Nmodel;
			continue;
		}
		Node *tmp = (Node *)malloc(sizeof(Node));
		
		memset(tmp, 0, sizeof(Node));

		tmp->Str = gjz[i];		
		
		tmp->Next = NULL;

/*sort needs to be done later
**if we do this we need another Prev pointer
*****************************************************************		
		if(parent->Tail)
		{
			if(parent->Tail->Str > tmp->Str)
			{
				parent->Tail->Prev->Next = tmp;
				tmp->Prev = parent->Tail->Prev;
				tmp->Next = parent->Tail;
				parent->Tail->Prev = tmp;	
				parent->Tail->Next = NULL;
			}
			else
			{
				parent->Tail->Next = tmp;
				tmp->Prev = parent->Tail;
				tmp->Next = NULL;
				parent->Tail = tmp;
			}
		}
		else
		{
			tmp->Next = NULL;
			tmp->Prev = NULL;
			parent->Tail = tmp;
		}
****************************************************************/	
		if(parent->Tail == NULL)
		{
			parent->Next = tmp;
			parent->Tail = tmp;
		}
		else
		{
			parent->Tail->Next = tmp;
			parent->Tail = tmp;
		}

		tmp->Pmodel = parent;
	
		Model *nmodel = (Model *)malloc(sizeof(Model));
		
		memset(nmodel, 0, sizeof(Model));

		tmp->Nmodel = nmodel;
		
		nmodel->Parent = tmp;

		nmodel->Cparent = parent;

		parent = nmodel;	

		if(i == Len - 1)
		{
			nmodel->End = 1;
			nmodel->No = flag;
		}
	}
}

/******************************
 * Name		:Machine_construct
 * Function :Automachine construct
 * Args		:
 * Return	:
 * PS		:construct the fail pointer
******************************/
VOID Automachine::Machine_construct()
{
	MQueue queue;

	Model *Current;
	
	Model *tmp = root;

	Node *Ntmp = tmp->Next;
	
	while(Ntmp)		
	{
		Ntmp->Nmodel->Fail = root;
		queue.Push(Ntmp->Nmodel);
		Ntmp = Ntmp->Next;
	}
	//out queue
	while(Current = queue.Pop())
	{
		//in queue
		Ntmp = Current->Next;
		while(Ntmp)
		{
			queue.Push(Ntmp->Nmodel);
			Ntmp = Ntmp->Next;			
		}	
		if(Current->Fail == NULL)
		{
			Ntmp = Current->Cparent->Next;
			while(Ntmp)
			{
				if(Ntmp->Str == Current->Parent->Str)
				{
					Current->Fail = Ntmp->Nmodel;		
					break;
				}
				Ntmp = Ntmp->Next;
			}
			if(Ntmp == NULL)
			{
				Current->Fail = Current->Cparent->Fail;
			}
		}
	}
}

/******************************
 * Name		:Machine_find
 * Function :Automachine find
 * Args		:
 * Return	:
 * PS		:find the string
******************************/
UINT Automachine::Machine_find(CHAR *Str, UINT Len)
{
#ifdef NO_CASE_SENSITIVE
	for(INT i = 0; i < Len; i++)
	{
		if((Str[i] >= 'a') && (Str[i] <= 'z'))
		{
			Str[i] = Str[i] - 'a' + 'A';
		}
	}
#endif	
	Model *tmp = root;

	Node *Ntmp = tmp->Next;

	Model *Mtmp;

	for(INT i = 0; i < Len; i++)
	{
		Mtmp = Ntmp->Pmodel;
		while(Ntmp)
		{
			if(Ntmp->Str == Str[i])
			{
				if(Ntmp->Nmodel->End == 1)
					return Ntmp->Nmodel->No;//find
				else 
					break;//next
			}
			Ntmp = Ntmp->Next;
		}
		if(Ntmp == NULL)//didn't find, go fail pointer
		{
			Ntmp = Mtmp->Fail->Parent;
			if(Ntmp)
			{
				if(Ntmp->Str == Str[i])
				{
					if(Ntmp->Nmodel->End == 1)	
						return Ntmp->Nmodel->No;//find
					else
						Ntmp = Ntmp->Nmodel->Next;			
				}
				else
					Ntmp = tmp->Next;	
			}
			else
				Ntmp = tmp->Next;
		}
		else//find the str and do the next
			Ntmp = Ntmp->Nmodel->Next;
	}	
	return 0;
}

/******************************
 * Name		:Machine_destroy
 * Function :Automachine destroy
 * Args		:
 * Return	:
 * PS		:
******************************/
VOID Automachine::Machine_destroy()
{
	MQueue queue;

	Model *Current;
	
	Model *tmp = root;

	Node *Ntmp = tmp->Next;

	Node *Dtmp;
	
	while(Ntmp)		
	{
		queue.Push(Ntmp->Nmodel);
		Ntmp = Ntmp->Next;
	}

	while(Current = queue.Pop())
	{
		//in queue
		Ntmp = Current->Next;
		while(Ntmp)
		{
			queue.Push(Ntmp->Nmodel);
			Dtmp = Ntmp;
			Ntmp = Ntmp->Next;
			free(Dtmp);			
		}
		free(Current);	
	}
	
}

/******************************
 * Name		:MQueue
 * Function :MQueue construct
 * Args		:
 * Return	:
 * PS		:
******************************/
MQueue::MQueue()
{
	Moqueue = (struct Modelqueue *)malloc(sizeof(struct Modelqueue));
	memset(Moqueue, 0, sizeof(struct Modelqueue));
	Head = Tail = Moqueue;
}

/******************************
 * Name		:~MQueue
 * Function :MQueue deconstruct
 * Args		:
 * Return	:
 * PS		:construct the fail pointer
******************************/
MQueue::~MQueue()
{
	if(Head)
	{
		free(Head);
		Head = Tail = Moqueue = NULL;	
	}
}

/******************************
 * Name		:Push
 * Function :queue push
 * Args		:
 * Return	:
 * PS		:head in and tail out
******************************/
VOID MQueue::Push(Model *Node)
{
	Head->Current = Node;
	Modelqueue *tmp = (struct Modelqueue *)malloc(sizeof(struct Modelqueue));
	memset(tmp, 0, sizeof(struct Modelqueue));
	Head->Prev = tmp;
	tmp->Next = Head;
	Head = tmp;				
}

/******************************
 * Name		:Pop
 * Function :queue pop
 * Args		:
 * Return	:
 * PS		:head in and tail out
******************************/
Model* MQueue::Pop()
{
	if(Tail == Head)
		return NULL;
	else
	{
		Model *re = Tail->Current;
		struct Modelqueue *tmp = Tail;
		Tail->Prev->Next = NULL;
		Tail = Tail->Prev;
		free(tmp);
		tmp = NULL;
		return re;	
	}
}


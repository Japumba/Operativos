#ifndef FILE_SINGLY_LIST
#define FILE_SINGLY_LIST

#include <stdlib.h>

typedef struct NODE
{
	void* valueptr;
	struct NODE *next;
}Node;

typedef struct SINGLY_LIST
{
	struct NODE *head;
	struct NODE *tail;
	int count;
}SinglyList;

Node *createNode(void* valueptr);
SinglyList *createSinglyList();
void addNodeToHead(SinglyList *list, void *valueptr);
void addNodeToTail(SinglyList *list, void *valueptr);
void destroyNode(Node *node);
void destroyList(SinglyList *list);

SinglyList *createSinglyList()
{
	SinglyList *newList;
	newList = (SinglyList*)malloc(sizeof(SinglyList));
	newList->count = 0;
	newList->head = NULL;
	return newList;
}

Node *createNode(void *valueptr)
{
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->valueptr = valueptr;
	newNode->next = NULL;
	return newNode;
}

void addNodeToHead(SinglyList *list, void *value)
{
	if(list == NULL)
		return;
	Node *newNode;
	newNode = createNode(value);
	
	if(list->count == 0)
	{
		list->head = newNode;
		list->tail = newNode;
		list->count++;
		return;
	}

	newNode->next = list->head;
	list->head = newNode;
}

void addNodeToTail(SinglyList *list, void *value)
{
	if(list == NULL)
		return;

	Node *newNode;
	newNode = createNode(value);

	if(list->count == 0)
	{
		list->head = newNode;
		list->tail = newNode;
		list->count++;
		printf("List is empty, adding new node.\n");
		return;
	}
	
	list->tail->next = newNode;
	list->tail = newNode;
}

void destroyList(SinglyList *list)
{
	if(list != NULL)
	{
		Node *temp = list->head;
		Node *toDelete;
		while(temp != NULL)
		{
			toDelete = temp;
			printf("DELETING NODE %i\n", *((int*)toDelete->valueptr));
			temp = temp->next;
			free(toDelete);
			toDelete = NULL;
		}
	}

	free(list);
	list = NULL;
	printf("LIST DELETED\n");
}

#endif

#ifndef LIST
#define LIST

#include <malloc.h>
#include <stdio.h>

typedef struct list
{
	int smbl;
	unsigned long long frqnc;
	struct list *next;
	struct list *left;
	struct list *right;
} list;

list *AddToList(list *head, unsigned long long frequency, int symbol);
list *BuildList(unsigned long long *frqnc);
list *EnterToList(list *head, list *element);

#endif
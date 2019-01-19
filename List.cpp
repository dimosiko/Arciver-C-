#include "List.h"

list *AddToList(list *head, unsigned long long frequency, int symbol)
{
	list *element;
	list *p = head;

	element = (list*)malloc(sizeof(list));
	element->frqnc = frequency;
	element->smbl = symbol;
	element->right = NULL;
	element->left = NULL;

	if ((!head) || (frequency < head->frqnc))
	{
		element->next = head;
		head = element;
	}
	else
	{
		while ((p->next) && (p->next->frqnc < frequency))
		{
			p = p->next;
		}

		element->next = p->next;
		p->next = element;
	}

	return head;
}

list *EnterToList(list *head, list *element)
{
	list *p = head;

	if ((!head) || (element->frqnc < head->frqnc))
	{
		element->next = head;
		head = element;
	}
	else
	{
		while ((p->next) && (p->next->frqnc < element->frqnc))
		{
			p = p->next;
		}

		element->next = p->next;
		p->next = element;
	}

	return head;
}

list *BuildList(unsigned long long *frqnc)
{
	list *head = NULL;
	int i;

	for (i = 0; i < 256; i++)
	{
		if (frqnc[i] > 0)
		{
			head = AddToList(head, frqnc[i], i);
		}
	}

	return head;
}
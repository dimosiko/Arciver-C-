#include "Tree.h"

list *MakeTree(unsigned long long *frqnc)
{
	list *elem;
	list *root;
	//unsigned long long *frqnc;

	/*//allocate memory for frequency
	frqnc = (unsigned long long*)calloc(256, sizeof(unsigned long long));
	if (frqnc == NULL)
	{
		//err 
	}

	CalcFrqnc(FName, frqnc);*/

	root = BuildList(frqnc);

	while ((root) && (root->next))
	{
		elem = (list*)malloc(sizeof(list));

		elem->left = root;
		elem->right = root->next;

		elem->frqnc = root->frqnc + root->next->frqnc;
		elem->smbl = -1;

		root = root->next->next;

		root = EnterToList(root, elem);
	}
	
	return root;
}

list *AddToTree(list *p, int direct, int symbol)
{
	list *element;

	element = (list*)malloc(sizeof(list));
	element->frqnc = 0;
	element->smbl = symbol;
	element->next = NULL;
	element->right = NULL;
	element->left = NULL;

	if (direct)
	{
		p->right = element;
	}
	else
	{
		p->left = element;
	}

	return element;
}

void DestroyTree(list *root)
{
	if (root)
	{
		if (root->left)
		{
			DestroyTree(root->left);
		}

		if (root->right)
		{
			DestroyTree(root->right);
		}

		free(root);
	}
}

unsigned int TreeBypassCount(list *root, unsigned int n)
{
	if (root)
	{
		n++;

		if (root->left)
		{
			n = TreeBypassCount(root->left, n);

			n = TreeBypassCount(root->right, n);
		}
		else
		{
			n++;
		}
	}

	return n;
}

void TreeToTable(list *root, unsigned char **CodesTable, unsigned int *CodesLength, unsigned char *buf, unsigned int len)
{
	unsigned char mask = 128;
	unsigned char lbuf[8];

	memcpy(lbuf, buf, 8);

	if (root)
	{
		//Хитрая схема		

		if (root->left)
		{
			mask >>= len % 8;
			len++;

			TreeToTable(root->left, CodesTable, CodesLength, lbuf, len);

			if (len % 8)
			{
				lbuf[len / 8] |= mask;
			}
			else
			{
				lbuf[len / 8 - 1] |= mask;
			}
			TreeToTable(root->right, CodesTable, CodesLength, lbuf, len);
		}
		else
		{
			memcpy(CodesTable[root->smbl], lbuf, 8);
			CodesLength[root->smbl] = len;
		}
	}
}

int TreeToArray(list *root, unsigned char *array, int n)
{
	if (root)
	{
		if (root->left)
		{
			array[n] = 1;
			n++;

			n = TreeToArray(root->left, array, n);

			n = TreeToArray(root->right, array, n);
		}
		else
		{
			array[n] = 0;
			array[n + 1] = root->smbl;
			n += 2;
		}
	}

	return n;
}

unsigned int ArrayToTree(list *root, unsigned char *array, int dir, unsigned int i)
{
	if (array[i])
	{
		if (i)
		{
			root = AddToTree(root, dir, -1);
		}
		i++;
		i = ArrayToTree(root, array, 0, i);
		
		i = ArrayToTree(root, array, 1, i);
	}
	else
	{
		root = AddToTree(root, dir, array[i + 1]);
		i += 2;
	}

	return i;
}
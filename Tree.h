#ifndef TREE
#define TREE

#include <string.h>
#include "List.h"
#include "Frqnc.h"

list *MakeTree(unsigned long long *frqnc);
list *AddToTree(list *p, int direct, int symbol);
void DestroyTree(list *root);
unsigned int TreeBypassCount(list *root, unsigned int n);
int TreeToArray(list *root, unsigned char *array, int n);
void TreeToTable(list *root, unsigned char **CodesTable, unsigned int *CodesLength, unsigned char *buf, unsigned int len);
unsigned int ArrayToTree(list *root, unsigned char *array, int dir, unsigned int i);

#endif
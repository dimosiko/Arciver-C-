#ifndef SPECIAL
#define SPECIAL

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "CRC32.h"

#define BUF_SIZE 65536

int SeekIfExist(FILE *arc, char **FName, int ex);
char *ReadStr(void);
void CopyToTempBefore(FILE *arc, FILE *tmp, fpos_t fBegin, fpos_t fEnd);
void FullCopy(FILE *arc, FILE *tmp);
void CopyToTempAfter(FILE *arc, FILE *tmp);
int IfCorrupted(FILE *arc, unsigned int hSize, unsigned long long fSize, unsigned char rbuf[BUF_SIZE], unsigned int *len, unsigned int *pos);

#endif
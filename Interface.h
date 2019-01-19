#ifndef INTERFACE
#define INTERFACE

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#define ERR_WRONG_KEY 1
#define ERR_WRONG_PARAMETERS 2
#define HELP 10

#define SIZE 256

int ReadArg(int argc, char *argv[], unsigned char *roles, char **files, int *size);
int arcCheck(char *mode, char *file, FILE **ARC);


#endif
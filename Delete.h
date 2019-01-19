#ifndef ENCODE
#define ENCODE

#include <sys/types.h>
#include <sys/stat.h>
#include "Tree.h"
#include "CRC32.h"

void Encode(char *FName, FILE *arc, char *str);

#endif
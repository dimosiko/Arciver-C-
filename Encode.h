#ifndef ENCODE
#define ENCODE

#include <sys/types.h>
#include <sys/stat.h>
#include "Tree.h"
#include "CRC32.h"

void Encode(char *FName, FILE *arc, FILE *file, char *str, FILE *time);

#endif
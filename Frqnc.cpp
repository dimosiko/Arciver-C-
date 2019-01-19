#include "Frqnc.h"

int CalcFrqnc(FILE *file, unsigned long long *frqnc)
{
	unsigned char buf[BUF_SIZE];
	unsigned int len;
	
	unsigned int i;



	fseek(file, 0, SEEK_SET);

	do
	{
		//copy the file into the buffer
		len = fread(buf, 1, BUF_SIZE, file);
		
		//calculate frequency
		for (i = 0; i < len; i++)
		{
			frqnc[buf[i]]++;
		}
	} 
	while (len == BUF_SIZE);

	fseek(file, 0, SEEK_SET);

	return 0;
}
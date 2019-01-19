#include "Special.h"

char *ReadStr(void)
{
	char buf[256];
	char *str;
	int size = 256;
	unsigned int i;


	while (true)
	{
		str = (char*)malloc(size * sizeof(char));

		buf[size - 1] = '\n';
		fgets(buf, 256, stdin);
		memcpy(str, buf, 256);

		while (buf[255] != '\n')
		{
			str = (char*)realloc(str, size + 255);

			buf[255] = '\n';
			fgets(buf, 256, stdin);
			memcpy(str + size - 1, buf, 256);

			size += 255;
		}
		str[strlen(str) - 1] = '\0';

		if (str[0] == '\0')
		{
			printf("Введена пустая строка. Пожалуйста, введите другое имя\n");
			size = 256;
			free(str);
		}
		else
		{
			for (i = 0; i < strlen(str); i++)
			{
				if ((str[i] == '\\') || (str[i] == '/') || (str[i] == ':') || (str[i] == '*') || (str[i] == '?')
					|| (str[i] == '"') || (str[i] == '<') || (str[i] == '>') || (str[i] == '|'))
				{
					printf("Введены недопустимые символы. Пожалуйста, введите имя, не содержащее символы \\ / : * ? \" < > | \n");
					size = 257;
					free(str);
				}
			}

			if (size != 257)
			{
				break;
			}
			else
			{
				size = 256;
			}
		}
	}
	
	return str;
}

int IfCorrupted(FILE *arc, unsigned int hSize, unsigned long long fSize, unsigned char rbuf[BUF_SIZE], unsigned int *len, unsigned int *pos)
{
	unsigned long n = 0;
	unsigned int k = 0;
	unsigned int llen = *len;
	unsigned char cs[4];



	if (fSize + hSize > BUF_SIZE)
	{
		fread(rbuf, 1, BUF_SIZE, arc);
		n = Crc32(rbuf, BUF_SIZE, n);
		fSize = fSize - BUF_SIZE + hSize;

		while (fSize > BUF_SIZE)
		{
			fread(rbuf, 1, BUF_SIZE, arc);
			n = Crc32(rbuf, BUF_SIZE, n);
			fSize -= BUF_SIZE;
		}

		hSize = 0;
	}
	llen = fread(rbuf, 1, BUF_SIZE, arc);
	n = Crc32(rbuf, fSize + hSize, n);
	
	k = (unsigned int)(llen - fSize - hSize);
	if (k > 4)
	{
		*len = llen;
		*pos = (unsigned int)(fSize + hSize + 4);

		return (memcmp(&n, rbuf + fSize + hSize, 4));
	}
	else
	{
		memcpy(cs, rbuf + fSize + hSize, k);
		llen = fread(rbuf, 1, BUF_SIZE, arc);
		memcpy(cs + k, rbuf, 4 - k);

		*len = llen;
		*pos = 4 - k;

		return (memcmp(&n, cs, 4));
	}
}

int SeekIfExist(FILE *arc, char **FName, int ex)
{
	fpos_t fBegin;

	unsigned char rbuf[BUF_SIZE];

	unsigned long long size = 0;

	unsigned int n;
	unsigned int len;
	unsigned int k = 0;
	int i;

	char *str = *FName;



	fseek(arc, 0, SEEK_SET);

	len = fread(rbuf, 1, BUF_SIZE, arc);  //////
	if (!len)
	{
		return 3;
	}

	i = 0;

	while ((len == BUF_SIZE) || (k != len))
	{
		i++;

		fseek(arc, (long)(0 - len + k), SEEK_CUR);
		len = fread(rbuf, 1, BUF_SIZE, arc);////
		k = 0;

		fseek(arc, (long)(0 - len), SEEK_CUR);
		fgetpos(arc, &fBegin);
		

		if (memcmp(rbuf, "*Dimosiko*", 10))
		{
			printf("Файл не является поддерживаемым файлом архива или испорчен\n");
			return 1;
		}
		k += 19;

		memcpy(&size, rbuf + k, 8);
		k += 8;

		n = rbuf[k] * 255 + rbuf[k + 1];
		k += 2;
		if (ex)
		{
			str = (char*)realloc(str, (n + 1) * sizeof(char));
			memcpy(str, rbuf + k, n);
			str[n] = '\0';
			
			if ((str) && (i == ex))
			{
				k += n;
				n = rbuf[k] * 255 + rbuf[k + 1];
				k += 18 + n;

				if (IfCorrupted(arc, k, size, rbuf, &len, &k))
				{
					printf("Файл архива испорчен\n");
					return 1;
				}
				else
				{
					*FName = str;
					fsetpos(arc, &fBegin);
					return 0;
				}
			}
		}
		else 
		{
			printf("03 ");
			if (strlen(str) == n)
			{
				if (!memcmp(str, rbuf + k, n))
				{
					k += n;
					n = rbuf[k] * 255 + rbuf[k + 1];
					k += 18 + n;

					if (IfCorrupted(arc, k, size, rbuf, &len, &k))
					{
						printf("Файл архива испорчен\n");
						return 1;
					}
					else
					{
						fsetpos(arc, &fBegin);
						return 0;
					}
				}
			}
		}
		k += n;

		n = rbuf[k] * 255 + rbuf[k + 1];
		k += 18 + n;

		if (IfCorrupted(arc, k, size, rbuf, &len, &k))
		{
			printf("Файл архива испорчен\n");
			return 1;
		}
	}

	return 2;
}

void FullCopy(FILE *tmp, FILE *arc)
{
	unsigned char rbuf[BUF_SIZE];

	unsigned int len;



	fseek(tmp, 0, SEEK_SET);
	fseek(arc, 0, SEEK_SET);

	len = fread(rbuf, 1, BUF_SIZE, tmp);
	while (len == BUF_SIZE)
	{
		fwrite(rbuf, 1, BUF_SIZE, arc);
		len = fread(rbuf, 1, BUF_SIZE, tmp);
	}
	fwrite(rbuf, 1, len, arc);

	fseek(arc, 0, SEEK_SET);
	fseek(tmp, 0, SEEK_SET);
}

void CopyToTempBefore(FILE *arc, FILE *tmp, fpos_t fBegin, fpos_t fEnd)
{
	unsigned char rbuf[BUF_SIZE];

	unsigned long long size = 0;
	unsigned int hSize;

	unsigned int n;
	unsigned int len;
	unsigned int k = 0;

	fsetpos(arc, &fBegin);
	len = fread(rbuf, 1, BUF_SIZE, arc);
	fseek(arc, (long)(0 - len), SEEK_CUR);

	while (fBegin != fEnd)
	{
		len = fread(rbuf, 1, BUF_SIZE, arc);
		fseek(arc, (long)(0 - len), SEEK_CUR);
		k = 0;

		k += 19;
		memcpy(&size, rbuf + k, 8);
		k += 8;
		n = rbuf[k] * 255 + rbuf[k + 1];
		k += 2 + n;
		n = rbuf[k] * 255 + rbuf[k + 1];
		k += 18 + n;

		hSize = k;
		if (size + hSize > BUF_SIZE)
		{
			fread(rbuf, 1, BUF_SIZE, arc);
			fwrite(rbuf, 1, BUF_SIZE, tmp);
			size = size - BUF_SIZE + hSize;

			while (size > BUF_SIZE)
			{
				fread(rbuf, 1, BUF_SIZE, arc);
				fwrite(rbuf, 1, BUF_SIZE, tmp);
				size -= BUF_SIZE;
			}

			hSize = 0;
		}
		len = fread(rbuf, 1, BUF_SIZE, arc);
		fwrite(rbuf, 1, (size_t)size + hSize, tmp);

		k = (unsigned int)(len - size - hSize);
		if (k > 4)
		{
			k = (unsigned int)(size + hSize + 4);
			fwrite(rbuf + size + hSize, 1, 4, tmp);
		}
		else
		{
			fwrite(rbuf + size + hSize, 1, 4, tmp);
			len = fread(rbuf, 1, BUF_SIZE, arc);
			k = 4 - k;
			fwrite(rbuf, 1, k, tmp);
		}
		
		fseek(arc, (long)(0 - len + k), SEEK_CUR);
		fgetpos(arc, &fBegin);
	}
}

void CopyToTempAfter(FILE *arc, FILE *tmp)
{
	unsigned char rbuf[BUF_SIZE];

	unsigned long long size = 0;
	unsigned int hSize;

	unsigned int n;
	unsigned int len;
	unsigned int k = 0;



	len = fread(rbuf, 1, BUF_SIZE, arc);
	fseek(arc, (long)(0 - len), SEEK_CUR);

	k += 19;
	memcpy(&size, rbuf + k, 8);
	k += 8;
	n = rbuf[k] * 255 + rbuf[k + 1];
	k += 2 + n;
	n = rbuf[k] * 255 + rbuf[k + 1];
	k += 18 + n;

	hSize = k;
	if (size + hSize > BUF_SIZE)
	{
		fread(rbuf, 1, BUF_SIZE, arc);
		size = size - BUF_SIZE + hSize;

		while (size > BUF_SIZE)
		{
			fread(rbuf, 1, BUF_SIZE, arc);
			size -= BUF_SIZE;
		}

		hSize = 0;
	}
	len = fread(rbuf, 1, BUF_SIZE, arc);

	k = (unsigned int)(len - size - hSize);
	if (k > 4)
	{
		k = (unsigned int)(size + hSize + 4);
	}
	else
	{
		len = fread(rbuf, 1, BUF_SIZE, arc);
		k = 4 - k;
	}

	fseek(arc, (long)(0 - len + k), SEEK_CUR);
	len = fread(rbuf, 1, BUF_SIZE, arc);
	while (len == BUF_SIZE)
	{
		fwrite(rbuf, 1, BUF_SIZE, tmp);
		len = fread(rbuf, 1, BUF_SIZE, arc);
	}
	fwrite(rbuf, 1, len, tmp);
}
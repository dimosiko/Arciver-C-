#include "Decode.h"

const int mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};

int GetSymbol(list **root, int direct)
{
	list *p = *root;

	if (p->right)
	{
		if (direct)
		{
			p = p->right;
		}
		else
		{
			p = p->left;
		}

		*root = p;
	}

	return p->smbl;
}

void Decode(char *FName, FILE *arc)
{
	list *root;
	list *TreeBuf;
	
	FILE *file;

	struct __utimbuf64 TimeBuf;
	__time64_t mtime;
	__time64_t atime;

	unsigned char rbuf[BUF_SIZE];
	unsigned char wbuf[BUF_SIZE];
	unsigned char *p = rbuf;
	unsigned char *array;

	unsigned long long size = 0;

	unsigned int n;
	unsigned int i;
	int k;
	int j;
	unsigned int len;

	root = (list*)calloc(1, sizeof(list));

	memset(wbuf, 0, sizeof(wbuf));

	len = fread(rbuf, 1, BUF_SIZE, arc);  //////

	if (!memcmp(p, "*Dimosiko*", 10))
	{
		printf("Decode started\n");
	}
	p += 10;

	k = p[0];
	p++;

	memcpy(&size, p, 8);
	p += 8;

	p += 8;

	n = p[0] * 255 + p[1];
	/*FName = (char*)malloc((n + 1) * sizeof(char));
	memcpy(FName, p + 2, n);
	FName[n] = '\0';*/
	p += 2 + n;
	
	n = p[0] * 255 + p[1];
	array = (unsigned char*)malloc(n * sizeof(unsigned char));
	memcpy(array, p + 2, n);
	p += 2 + n;

	memcpy(&atime, p, 8); //запоминаем время для восстановления
	p += 8;

	memcpy(&mtime, p, 8);
	p += 8;

	file = fopen(FName, "wb");

	if (k == 2)
	{
		fclose(file);
	}
	else if (k == 1)
	{
		memset(wbuf, p[0], BUF_SIZE);
		while (size > BUF_SIZE)
		{
			fwrite(wbuf, 1, BUF_SIZE, file);

			size = size - BUF_SIZE;
		}
		fwrite(wbuf, 1, (size_t)size, file);
	}
	else if (k == 3)
	{
		fseek(arc, (long)(0 - len + (p - rbuf)), SEEK_CUR);
		while (size > BUF_SIZE)
		{
			fread(rbuf, 1, BUF_SIZE, arc);
			fwrite(rbuf, 1, BUF_SIZE, file);
			size -= BUF_SIZE;
		}
		fread(rbuf, 1, (size_t)size, arc);
		fwrite(rbuf, 1, (size_t)size, file);
	}
	else
	{
		ArrayToTree(root, array, 0, 0);
		free(array);

		n = 0;
		TreeBuf = root;

		for (i = p - rbuf; i < len; i++)
		{
			for (j = 0; j < 8; j++)
			{
				k = GetSymbol(&TreeBuf, rbuf[i] & mask[j]);

				if (k != -1)
				{
					wbuf[n] = k;
					size--;

					n++;

					if (n == BUF_SIZE)
					{
						fwrite(wbuf, 1, n, file);
						n = 0;
					}

					TreeBuf = root;
				}

				if (size == 0)
				{
					break;
				}
			}

			if (size == 0)
			{
				break;
			}
		}

		while ((len == BUF_SIZE) && (size))
		{
			len = fread(rbuf, 1, BUF_SIZE, arc);

			for (i = 0; i < len; i++)
			{
				for (j = 0; j < 8; j++)
				{
					k = GetSymbol(&TreeBuf, rbuf[i] & mask[j]);

					if (k != -1)
					{
						wbuf[n] = k;
						size--;

						n++;

						if (n == BUF_SIZE)
						{
							fwrite(wbuf, 1, n, file);
							n = 0;
						}

						TreeBuf = root;
					}

					if (size == 0)
					{
						break;
					}
				}

				if (size == 0)
				{
					break;
				}
			}
		}

		fwrite(wbuf, 1, n, file);
	}

	DestroyTree(root); //освобождаем память
	

	fclose(file);

	TimeBuf.actime = atime; //восстановление времени
	TimeBuf.modtime = mtime;
	_utime64(FName, &TimeBuf);

	printf("Decode complete\n");
}

#include "Encode.h"

void MakeHead(FILE *arc, FILE *file, char *FName, list *root, unsigned long long fSize, unsigned long long *Size, unsigned int *hSize)
{
	struct _stat64 fStat;

	unsigned char wbuf[BUF_SIZE];

	unsigned char *array;
	unsigned int n;
	unsigned char *p = wbuf;

	unsigned long long bSize = *Size;
	


	memcpy(p, "*Dimosiko*", 10); //сигнатура
	p += 10;

	if (root)
	{
		if (root->left) //односимвольный или нет
		{
			if (bSize >= fSize) //нужно ли сжатие
			{
				p[0] = 3; //без сжатия
			}
			else
			{
				p[0] = 0; //обычное сжатие
			}
		}
		else
		{
			p[0] = 1; //односимвольный
			bSize = 1;
		}
	}
	else
	{
		p[0] = 2; //пустой
		bSize = 0;
	}
	p++;

	memcpy(p, &fSize, 8); //размер файла (несжатый)
	p += 8;

	memcpy(p, &bSize, 8); //размер файла (сжатый)
	p += 8; 

	n = strlen(FName); //размер имени файла
	p[0] = n / 255;  //255, не 256!!
	p[1] = n % 255;
	p += 2;

	memcpy(p, FName, n); //имя файла
	p += n;

	if (bSize >= fSize)
	{
		p[0] = 0;
		p[1] = 0;
		p += 2;
	}
	else
	{
		n = TreeBypassCount(root, 0); //размер дерева
		p[0] = n / 255;
		p[1] = n % 255;
		p += 2;

		array = (unsigned char*)malloc(n * sizeof(unsigned int));
		TreeToArray(root, array, 0);

		memcpy(p, array, n); //дерево 
		p += n;

		free(array);
	}
	

	_fstat64(_fileno(file), &fStat);
	memcpy(p, &(fStat.st_atime), 8); //время последнего доступа
	p += 8;
	memcpy(p, &(fStat.st_mtime), 8); //время последнего изменения
	p += 8;

	*hSize = p - wbuf;
	*Size = bSize;
	fwrite(wbuf, 1, p - wbuf, arc); //запись в файл	
}

void MakeBody(FILE *arc, FILE *file, unsigned char **CodesTable, unsigned int *CodesLength, unsigned long fCount, unsigned int ost)
{
	unsigned char rbuf[BUF_SIZE];
	unsigned char wbuf[BUF_SIZE];
	unsigned int len;

	unsigned int bite = 0;
	int bit = 0;

	unsigned int n = 0;
	unsigned int i;
	int buf;

	unsigned long count = 0;



	memset(wbuf, 0, sizeof(wbuf));
	printf("%d/%d записано\r", count, fCount);

	do
	{
		//считываем содержимое файла в архив
		len = fread(rbuf, 1, BUF_SIZE, file);

		for (n = 0; n < len; n++)
		{
			buf = CodesLength[rbuf[n]];
			i = 0;

			for (; buf > 0; buf -= 8)
			{
				wbuf[bite] |= CodesTable[rbuf[n]][i] >> bit;

				if (buf >= 8)
				{
					if (bite == BUF_SIZE - 1)
					{
						fwrite(wbuf, 1, BUF_SIZE, arc);
						memset(wbuf, 0, sizeof(wbuf));
						count++;
						printf("%d/%d записано\r", count, fCount);

						bite = 0;
						wbuf[bite] |= CodesTable[rbuf[n]][i] << (8 - bit);
					}
					else
					{
						bite++;
						wbuf[bite] |= CodesTable[rbuf[n]][i] << (8 - bit);
					}
				}
				else
				{
					if (8 - bit < buf)
					{
						if (bite == BUF_SIZE - 1)
						{
							fwrite(wbuf, 1, BUF_SIZE, arc);
							memset(wbuf, 0, sizeof(wbuf));
							count++;
							printf("%d/%d записано\r", count, fCount);

							bite = 0;
							wbuf[bite] |= CodesTable[rbuf[n]][i] << (8 - bit);
							bit = buf - 8 + bit;
						}
						else
						{
							bite++;
							wbuf[bite] |= CodesTable[rbuf[n]][i] << (8 - bit);
							bit = buf - 8 + bit;
						}
					}
					else
					{
						bit = bit + buf;
					}
				}

				i++;
			}
		}
	} 
	while (len == BUF_SIZE);

	fwrite(wbuf, 1, ost, arc);
	count++;
	printf("%d/%d записано\n", count, fCount);
}

unsigned long long CalcSize(unsigned long long *frqnc, unsigned int *CodesLength)
{
	int i;
	unsigned long long sizem = 0;
	unsigned long long sizeo = 0;

	for (i = 0; i < 256; i++)
	{
		sizem += (CodesLength[i] / 8) * frqnc[i];
		sizem += ((CodesLength[i] % 8) * frqnc[i]) / 8;
		sizeo += ((CodesLength[i] % 8) * frqnc[i]) % 8;
	}

	sizem += sizeo / 8;

	if (sizeo % 8 > 0)
	{
		sizem++;
	}

	return sizem;
}

void copy(FILE *arc, FILE *file, unsigned long fCount)
{
	unsigned char rbuf[BUF_SIZE];
	unsigned int len;
	unsigned long count = 0;
	
	printf("%d/%d записано\r", count, fCount);
	len = fread(rbuf, 1, BUF_SIZE, file);
	while (len == BUF_SIZE)
	{
		fwrite(rbuf, 1, BUF_SIZE, arc);
		len = fread(rbuf, 1, BUF_SIZE, file);
		count++;
		printf("%d/%d записано\r", count, fCount);
	}
	fwrite(rbuf, 1, len, arc);
	count++;
	printf("%d/%d записано\n", count, fCount);
}

void Encode(char *FName, FILE *arc, FILE *file, char *str, FILE *time)
{
	list *root = NULL; 

	//FILE *file;

	fpos_t fBegin = 0;
	fpos_t fEnd = 0;

	unsigned long long *frqnc;

	unsigned long long fSize = 0;
	unsigned long long bSize = 0;
	unsigned int hSize = 0;

	unsigned char *buf;  
	unsigned char rbuf[BUF_SIZE];

	unsigned char **CodesTable;
	unsigned int *CodesLength;
	
	unsigned long n = 0;


	
	printf("Начата архивация файла %s\n", FName);

	CodesLength = (unsigned int*)malloc(256 * sizeof(unsigned int)); //ERRORS
	memset(CodesLength, -1, 256 * sizeof(unsigned int));

	CodesTable = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for (n = 0; n < 256; n++)
	{
		CodesTable[n] = (unsigned char*)calloc(8, sizeof(unsigned char));
	}

	buf = (unsigned char*)calloc(8, sizeof(unsigned char));
	if (buf == NULL)
	{
		//err;
	}

	frqnc = (unsigned long long*)calloc(256, sizeof(unsigned long long));
	if (frqnc == NULL)
	{
		//err 
	}
	CalcFrqnc(file, frqnc);

	root = MakeTree(frqnc); //создание дерева
	TreeToTable(root, CodesTable, CodesLength, buf, 0); //создание таблицы из дерева
	bSize = CalcSize(frqnc, CodesLength); //подсчет размера сжатого файла

	free(buf); //больше не нужны
	free(frqnc);
	
	//file = fopen(FName, "rb");  //размер файла
	fseek(file, 0, SEEK_END);
	fSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	fgetpos(arc, &fBegin); 
	MakeHead(arc, time, str, root, fSize, &bSize, &hSize); //заголовок

	n = (unsigned long)(bSize / BUF_SIZE); //количество сегментов
	if (bSize % BUF_SIZE)
	{
		n++;
	}

	if (root)
	{
		if (root->left)
		{
			if (bSize >= fSize)
			{
				copy(arc, file, n);
			}
			else
			{
				MakeBody(arc, file, CodesTable, CodesLength, n, bSize % BUF_SIZE); //тело
			}
		}
		else
		{
			fwrite(&root->smbl, 1, 1, arc);
			bSize = 1;
		}
	}
	else
	{
		bSize = 0;
	}
	fgetpos(arc, &fEnd);

	DestroyTree(root); //освобождаем память 
	free(CodesLength);
	for (n = 0; n < 256; n++)
	{
		free(CodesTable[n]);
	}
	free(CodesTable);

	fsetpos(arc, &fBegin);

	n = 0; //подсчет кс
	if (bSize + hSize > BUF_SIZE)
	{
		fread(rbuf, 1, BUF_SIZE, arc);
		n = Crc32(rbuf, BUF_SIZE, n);
		bSize = bSize - BUF_SIZE + hSize;
	
		while (bSize > BUF_SIZE)
		{
			fread(rbuf, 1, BUF_SIZE, arc);
			n = Crc32(rbuf, BUF_SIZE, n);
			bSize -= BUF_SIZE;
		}

		hSize = 0;
	}
	fread(rbuf, 1, size_t(bSize + hSize), arc); 
	n = Crc32(rbuf, bSize + hSize, n);
	
	fsetpos(arc, &fEnd);
	fwrite(&n, 1, 4, arc); //запись контрольной суммы
	fflush(arc);
	
	//fclose(file);

	printf("Архивация файла %s завершена\n", FName);
}


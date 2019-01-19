#include "Listing.h"

int Listing(FILE *arc)
{
	__time64_t mtime;

	unsigned char rbuf[BUF_SIZE];

	unsigned long long size = 0;
	unsigned long long fSize = 0;

	char *FName;

	unsigned int n;
	unsigned int k = 0;
	int mode;
	unsigned int len;



	printf("Имя                 | Размер файла           | Размер в архиве        | Принцип сжатия | Дата посл. изменения файла\n"); //20 1 24 1 24 1 16 1

	fseek(arc, 0, SEEK_SET);
	len = fread(rbuf, 1, BUF_SIZE, arc);

	while ((len == BUF_SIZE) || (k != len))
	{
		fseek(arc, (long)(0 - len + k), SEEK_CUR);
		len = fread(rbuf, 1, BUF_SIZE, arc);
		k = 0;

		fseek(arc, (long)(0 - len), SEEK_CUR);

		if (memcmp(rbuf, "*Dimosiko*", 10))
		{
			printf("Файл не является поддерживаемым файлом архива или испорчен\n");
			return 1;
		}
		k += 10;
		mode = (rbuf + k)[0];
		k++;
		memcpy(&fSize, (rbuf + k), 8);
		k += 8;
		memcpy(&size, (rbuf + k), 8);
		k += 8;
		n = (rbuf + k)[0] * 255 + (rbuf + k)[1];
		FName = (char*)malloc((n + 1) * sizeof(char));
		memcpy(FName, (rbuf + k) + 2, n);
		FName[n] = '\0';
		k += 2 + n;
		n = (rbuf + k)[0] * 255 + (rbuf + k)[1];
		k += 10 + n;
		memcpy(&mtime, (rbuf + k), 8);
		k += 8;

		if (IfCorrupted(arc, k, size, rbuf, &len, &k))
		{
			printf("Чтение далее невозможно. Архив испорчен");
			return 1;
		}
		else
		{
			if (strlen(FName) > 16)
			{
				printf("%16s... | ", FName);
			}
			else
			{
				printf("%19s | ", FName);
			}
			printf("%22lld | ", fSize);
			printf("%22lld | ", size);
			if (mode == 0)
			{
				printf("Обычный        | ");
			}
			else if (mode == 1)
			{
				printf("Односимвольный | ");
			}
			else if (mode == 2)
			{
				printf("Пустой         | ");
			}
			else if (mode == 3)
			{
				printf("Несжатый       | ");
			}
			printf("%s", _ctime64(&mtime));
		}
	}

	return 0;
}
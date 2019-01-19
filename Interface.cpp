#include "Interface.h"

int ReadArg (int argc, char *argv[], unsigned char *roles, char **files, int *size)
{
	int i;
	int pos = 0;
	int sizea = 0;

	

	for (i = 1; i < argc; i++)
	{
		if ((pos == 0) || (pos > 5))
		{
			if (argv[i][0] == '/')
			{
				if (argv[i][2] == '\0')
				{
					switch (argv[i][1])
					{
					case 'h':
						return HELP;
						break;

					case 'x':
						pos = 1;
						break;

					case 'a':
						pos = 2;
						break;

					case 'd':
						pos = 3;
						break;

					case 'l':
						pos = 4;
						break;

					case 'f':
						pos = 5;
						break;

					default:
						return ERR_WRONG_KEY;
						break;
					}
				}
				else
				{
					return ERR_WRONG_KEY;
				}
			}
			else
			{
				if (pos == 0)
				{
					return ERR_WRONG_KEY;
				}
				else
				{
					roles[sizea] = pos - 5;
					files[sizea] = argv[i]; //имя файла
					sizea++;
				}
			}
		}
		else
		{
			if (pos > 3)
			{
				roles[sizea] = pos; //файл под листинг/проверку
			}
			else
			{
				roles[sizea] = pos + 5; //файл архива
			}

			files[sizea] = argv[i]; //имя файла
			
			sizea++;

			if (pos > 3)
			{
				pos = 0;
			}
			else
			{
				pos += 5;
			}
		}
	}

	*size = sizea;

	return 0;
}

int arcCheck(char *mode, char *file, FILE **ARC)
{
	int choise = 0;
	FILE *arc = *ARC;

	if (arc)
	{
		fclose(arc);
	}

	if (!_access(file, 0))
	{
		if (mode == "rb")
		{
			if (!_access(file, 4))
			{
				arc = fopen(file, mode);
			}
			else
			{
				printf("Архив не может быть открыт для чтения. Действия с архивом %s прекращены\n", file);
				return 1;
			}
		}
		else
		{
			if (!_access(file, 6))
			{
				arc = fopen(file, mode);
			}
			else
			{
				printf("Архив не может быть открыт для чтения и записи. Действия с архивом %s прекращены\n", file);
				return 1;
			}
		}
	}
	else
	{
		arc = fopen(file, mode);
	}

	if (!arc)
	{
		if (!_access(file, 0))
		{
			printf("Файл архива %s недоступен для открытия", file);
		}
		else
		{
			while (choise == 0)
			{
				printf("Файл архива %s не существует. Создать его? y/n\n", file);
				fflush(stdin);
				scanf("%c", &choise);

				if ((choise == 'y') || (choise == 'Y'))
				{
					arc = fopen(file, "wb+");
					if (!arc)
					{
						printf("Не удается создать файл архива %s. Действия с ним прекращены\n", file);
						*ARC = arc;
						return 1;
					}
					else
					{
						*ARC = arc;
						return 0;
					}
				}
				else if ((choise == 'n') || (choise == 'N'))
				{
					printf("Действия с архивом %s прекращены\n", file);
					*ARC = arc;
					return 1;
				}
				else
				{
					printf("Команда не распознана\n");
					choise = 0;
				}
			}
		}
	}
	else
	{
		*ARC = arc;
		return 0;
	}
}
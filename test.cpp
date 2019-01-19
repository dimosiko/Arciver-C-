#include <locale.h>
#include "Interface.h"
#include "Special.h"
#include "Encode.h"
#include "Decode.h"
#include "Listing.h"

int main(int argc, char *argv[])
{
	FILE *arc = NULL;
	FILE *tmp;
	FILE *tmp2;
	FILE *file;

	fpos_t fBegin;
	fpos_t fCur;

	int i;

	unsigned char *roles;
	char **files;
	
	char *str;
	char *arcName = NULL;
	char choise = 0;

	int size = 0;
	int err;
	int ex = 0;



	setlocale(LC_ALL, "Rus");
	
	if (argc == 1)
	{
		//printf("Введите команду: ");
		//
	}
	else
	{
		roles = (unsigned char*)malloc(argc * sizeof(unsigned char));            /*check*/
		files = (char**)malloc(argc * sizeof(char*));

		switch (ReadArg(argc, argv, roles, files, &size))
		{
		case HELP:
			printf("Перед вами архиватор. Он принимает на вход строку комманд вида:\n");
			printf("<архиватор> [<команда> <архив> [файлы]]\n");
			printf("Где архиватор - имя .exe файла архиватора, архив - имя файла архива\n");
			printf("файлы - имена файлов, которые вы хотите добавить / извлечь / удалить\n");
			printf("команда - одна из 5 комманд взаимодействия с архивом (команды вводятся на английском языке):\n");
			printf("/a - Добавить файл(ы) в архив\n");
			printf("/x - Извлечь файл(ы) из архива\n");
			printf("/d - Удалить файл(ы) из архива\n");
			printf("/l - Просмотреть содержимое архива\n");
			printf("/h - Вызвать помощь. Эта команда прерывает исполнение программы\n");
			break;

		case ERR_WRONG_KEY:
			printf("\nВведен неверный ключ. Вызвать помощь: </h>. Выход из программы\n");
			break;

		default:
			i = 0;
			while (i < size)
			{
				switch (roles[i])
				{
				case 6: //открытие файла архива для извлечения

				case 7: //открытие файла архива для добавления

				case 8: //открытие файла архива для удаления
					if (roles[i] == 6)
					{
						err = arcCheck("rb", files[i], &arc);
					}
					else
					{
						err = arcCheck("rb+", files[i], &arc);
					}
					if (err)
					{
						while ((roles[i + 1] < 6) && (i < size - 1))
						{
							i++;
						}
					}
					else
					{
						fgetpos(arc, &fBegin);
						arcName = files[i];
						if (roles[i] == 6)
						{
							if ((i + 1 == size) || (roles[i + 1] != 1))//////////////
							{
								roles[i] = 1;
								ex = 1;
								i--;
							}
						}
					}
					break;

				case 1: //извлечение файла
					if (ex)
					{
						str = NULL;
					}
					else
					{
						str = files[i];
					}

					
					err = SeekIfExist(arc, &str, ex);
					
					if ((str) && (ex))
					{
						i--;
						ex++;
					}
					else
					{
						ex = 0;
					}

					choise = '2';
					if (err == 0)
					{
						while (choise == '2')
						{
							if (!_access(str, 0))
							{
								printf("Файл с таким именем уже есть в папке, выберите действие:\n");
								printf("1: Не разархивировать этот файл\n");
								printf("2: Переименовать этот файл\n");
								printf("3: Перезаписать этот файл\n\n");
								fflush(stdin);
								scanf("%c", &choise);
								printf("\n");

								if (choise == '1')
								{
									printf("Файл %s пропущен\n", str);
								}
								else if (choise == '2')
								{
									printf("Введите имя файла, под которым он будет разархивирован:\n");
									fflush(stdin);
									str = ReadStr();
								}
								else if (choise == '3')
								{
									if (!_access(str, 2) || !_access(str, 6))
									{
										Decode(str, arc);
									}
									else
									{
										printf("Файл %s не может быть перезаписан\n", str);
										choise = '2';
									}
								}
								else
								{
									printf("Такой команды нет в списке\n");
									choise = '2';
								}
							}
							else
							{
								Decode(str, arc);
								break;
							}
						}
					}
					else if (err == 2)
					{
						printf("Такого файла нет в архиве\n");
					}
					else if (err == 3)
					{
						printf("Файл архива пуст\n");
					}
					break;

				case 2: //добавление файла
					str = files[i];
					choise = '2';

					while (choise == '2')
					{
						err = SeekIfExist(arc, &str, 0);
						if (err > 1)
						{
							file = fopen(files[i], "rb");
							if (file && (!_access(files[i], 4) || !_access(files[i], 6)))
							{
								tmp = tmpfile();
								FullCopy(file, tmp);
								Encode(files[i], arc, tmp, str, file);
								fclose(file);
								fclose(tmp);
							}
							else
							{
								printf("Невозможно открыть файл %s для чтения. Он будет пропущен\n", files[i]);
							}
							break;
						}
						else if (err == 0)
						{
							printf("Файл с таким именем уже есть в архиве, выберите действие:\n");
							printf("1: Не архивировать этот файл\n");
							printf("2: Переименовать этот файл\n");
							printf("3: Перезаписать этот файл\n\n");
							fflush(stdin);
							scanf("%c", &choise);
							printf("\n");

							if (choise == '1')
							{
								printf("Файл %s пропущен\n", files[i]);
							}
							else if (choise == '2')
							{
								printf("Введите новое имя файла для архивации:\n");
								fflush(stdin);
								str = ReadStr();
							}
							else if (choise == '3')
							{
								file = fopen(files[i], "rb");
								if (file && (!_access(files[i], 4) || !_access(files[i], 6)))
								{
									fgetpos(arc, &fCur);
									tmp = tmpfile();

									CopyToTempBefore(arc, tmp, fBegin, fCur);
									tmp2 = tmpfile();
									FullCopy(file, tmp2);
									Encode(files[i], tmp, tmp2, str, file);
									fclose(file);
									fclose(tmp2);
									CopyToTempAfter(arc, tmp);

									fclose(arc);
									fopen(arcName, "wb+");
									FullCopy(tmp, arc);
								}
								else
								{
									printf("Невозможно открыть файл %s для чтения. Он будет пропущен", files[i]);
								}					
							}
							else
							{
								printf("Такой команды нет в списке\n");
								choise = '2';
							}
						}
						else
						{
							break;
						}
					}
					break;
				
				case 3: //удаление файла
					err = SeekIfExist(arc, &files[i], 0);
					if (err == 0)
					{
						fgetpos(arc, &fCur);
						tmp = tmpfile();

						CopyToTempBefore(arc, tmp, fBegin, fCur);
						CopyToTempAfter(arc, tmp);

						fclose(arc);
						fopen(arcName, "wb+");
						FullCopy(tmp, arc);
					}
					else if (err == 2)
					{
						printf("Такого файла нет в архиве\n");
					}
					else if (err == 3)
					{
						printf("Файл архива пуст\n");
					}
					break;

				case 4: //листинг

				case 5: //проверка на целостность(false)
					err = arcCheck("rb", files[i], &arc);
					if (err)
					{
						while ((roles[i + 1] < 6) && (i < size - 1))
						{
							i++;
						}
					}
					else
					{
						if (roles[i] == 4)
						{
							Listing(arc);
						}
						else
						{

						}
					}
					break;
				}

				i++;
			}

			if (arc)
			{
				fclose(arc);
			}

			free(roles);
			free(files);
		}
	}

	return 0;
}
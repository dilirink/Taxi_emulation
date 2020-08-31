#include "cfg_reader.h"

struct cfg_read *cfg_reader(struct cfg_read *cf) /*открываем и читаем если не открываем то создаём новый дефолтный и его читаем*/
{
    char port[BUFLEN]; /*буфер порта*/
    char ip[BUFLEN];   /*буфер айпи*/
    char line[BUFLEN]; /*буфер считаной линии при построчном считывании*/
    int linenum = 0;   /*счётчик для вывода строки файла в которой была ошибка*/
    int flag_port = 0; /*если есть ошибка в заполнении порта то флаг останеться в нули*/
    int flag_ip = 0;   /*если есть ошибка в заполнении айпи то флаг останеться в нули*/
    FILE *fp;

    if ((fp = fopen("config.txt", "r+")) == NULL) /*пытаемся открыть если файл не доступен то мы создаём его и наполняем, в случае a+t сразу он создаётся и нам нужно делать проверку на заполнение.*/
    {
        printf("Cannot open file,create new file \n");
        fp = fopen("config.txt", "a+");
        char standart[] = "# coments\nPORT 1884\nIP 127.0.0.1"; /*стандартная форма заполнения конфига*/
        fputs(standart, fp);

        if (fseek(fp, 0, SEEK_SET) == 0)
            printf("fseek выполнено\n");
        else
            printf("sfseek ошибка\n");
    }
    while (fgets(line, BUFLEN, fp) != NULL)
    {
        char key[BUFLEN], value[BUFLEN];

        linenum++;
        if (line[0] == '#')
            continue;

        if (sscanf(line, "%5s %15s", key, value) != 2)
        {
            fprintf(stderr, "Syntax error, line %d\n", linenum);
            continue;
        }

        if (strcmp(key, "PORT") == 0)
        {
            flag_port = 1;
            strcpy(port, value);
        }

        if (strcmp(key, "IP") == 0)
        {
            flag_ip = 1;
            strcpy(ip, value);
        }
    }
    fclose(fp);
    if (flag_port == 0 || flag_ip == 0)
    {
        printf("BAD config file\n");

        cfg_reader(cf);
    }
    strcpy(cf->PORT, port);
    strcpy(cf->IP, ip);

    printf("\ncfg read:\n\tport-%s\n\t\n", port);
    return cf;
}

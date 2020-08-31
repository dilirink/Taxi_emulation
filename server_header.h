#ifndef _HEAD_SERVER_
#define _HEAD_SERVER_

#include "master_header.h"
#include "cfg_reader.h"

#define MICRO_BUFFER 100
#define MAX_CLIENT 100

struct parser_return
{
    int type;
    int cord_x;
    int cord_y;
    int cord_distination_y;
    int cord_distination_x;
} ptr_parser;

struct list *root_list; /*первый лист структуры с таксистами/пассажирами*/

struct statistics
{
    int kilometrs;
    int trips;
} stats;

enum mode_connect
{
    start_connection = 0,
    connect_between_cli_and_tax = 1,
    message_start = 1,
    message_stop = 0
};

enum message_start_cheker
{

    connected = 0,
    dont_start_connnected = -1
};

enum cli_tax
{
    client = 1,      /*свободный клиент*/
    client_busy = 2, /*свободный клиент*/
    taxi = 0,        /*занятое такси*/
    taxi_busy = 3    /*занятое такси*/

};

struct list /*структура совмещает в себе список из клиентов и таксистов некоторые переменные служат для таксиста по свойму для клиента по свойму*/
{
    int type;               /* пользователь или такси*/
    int cord_y;             /*кордината У на которой находиться пользователь/таксист*/
    int cord_x;             /*кордината Х на которой находиться пользователь/таксист*/
    int cord_distination_y; /*дополнительные кординаты для пользоывателя, то куда он едет (У).*/
    int cord_distination_x; /*дополнительные кординаты для пользоывателя, то куда он едет(Х).*/
    int number_of_trips;    /*оценка данного таксиста*/
    int sock_tax_or_client;
    int socket;                  /*сокет через котороый будем общаться с водителем или пассажиром.*/
    int flag_message;            /*флаг для отслеживания  начала общения между кдиентом и такси.*/
    struct list *tax_or_cli_ptr; /*указатель на такси которое везёт клиента или на клиента если блок принадлежит таксисту*/
    struct list *ptr;            /* указатель на следующий элемент*/
};

struct list *deletelem(struct list *del_list);                                          /*удалить элемент;*/
struct list *addelem(struct list *lst, struct parser_return *ptr_parser, int sock);     /*/добавления узла;*/
void listprint(struct list *lst);                                                       /*вывод элементов списка*/
double distans(int cord_x_taxi, int cord_y_taxi, int cord_x_client, int cord_y_client); /*расчёт растояния между двумя точками на плоскости*/
struct parser_return *parser(char *message_buf, struct parser_return *ptr_parser);      /*разибирает сообщение на то кто это написал такси\клиент и вытаскивает координаты*/
int socket_creat(struct sockaddr_in servaddr, struct cfg_read *cfg);                    /*создаём сокет*/
int sort(struct list *lst);                                                             /*сортировка списка и подбор клиенту самого близкого таксиста.*/
int connects(struct list *root, int sock, char message[MICRO_BUFFER]);                  /*функция соединения между кдиентом и таксистом*/
void start_message(int socket);                                                         /*отправляем  клиенту сообщение что такси найдено. и медлу такси и клиентом начинается сообщение*/
void restart_message(int socket);                                                       /*отправляем сообщение об отвале клиента или таксиста чтобы клиент или таксист сбросили счётчики сообщений.*/
void catch (int sig);
void message_start_cheker(struct list *lst); /*проверяем начато ли сообщение между клиентом и такси.*/

#endif
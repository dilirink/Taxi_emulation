#ifndef _HEAD_
#define _HEAD_

#include "master_header.h"
#include "cfg_reader.h"
#include "func_client_taxi.h"

int sock;

typedef struct
{
    int massage_number;
    char *massage;
    char *request_massge;
    char *console_massage;
} taxi, client;

taxi table_massage_taxi[] =
    {
        {0, "cli_1", "tax_1", "Едем зарабатывать"},                               /*тут таксист едет к пассажиру*/
        {1, "cli_6", "tax_7", "я приехал ВЫХОДИ!! каждая минута ожидания 200р."}, /*таксист отвечает что ок он приехал*/
        {2, "cli_8", "tax_8", "Ну зРасте а  ГДЕ МАСКА ТО?!?!?!"},                 /*таксист едет и тут мы типо засыпаем пока едем, таксист кидает последнее сообщение что приехали*/
        {3, "cli_3_", "tax_9"},                                                   /*подтверждаем получение оценки.*/
        {0, NULL, NULL}};

client table_massage_client[] =
    {
        {0, "ser_1", "cli_1", "вызвал такси"},                 /*тут таксист едет к пассажиру*/
        {1, "tax_1", "cli_6", "Ура такси катит ко мне"},       /*таксист отвечает что ок он приехал*/
        {2, "tax_7", "cli_8", "какая же развалюха ну лан GO"}, /*таксист едет и тут мы типо засыпаем пока едем, таксист кидает последнее сообщение что приехали*/
        {3, "tax_8", "cli_3_", "лучше посплю"},                /*подтверждаем получение оценки.*/
        {4, "tax_9", NULL, NULL},
        {0, NULL, NULL}};

#endif

/*
gcc -c cfg_reader.c;
ar rc libcfg_reader.a cfg_reader.o;
ranlib libcfg_reader.a;
gcc -c func_client_taxi.c;
ar rc libfunc_client_taxi.a func_client_taxi.o;
ranlib libfunc_client_taxi.a;
gcc taxi_main.c -o taxi  -L. -lcfg_reader -L. -lfunc_client_taxi;
gcc client_main.c -o client -L. -lcfg_reader -L. -lfunc_client_taxi;
gcc server_main.c server_func.c -o server  -L. -lcfg_reader -lm;



valgrind --tool=massif

ms_print massif.out.pid


*/
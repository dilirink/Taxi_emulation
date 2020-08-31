#include "server_header.h"

int sort(struct list *lst) /* 0-такси 1-клиент*/
{
    struct list *lst2, *tax, *cli; /*lst2-для второго цикла, tax- ячейка таксиста для быстрого доступа если его дистанция меньше чем у остальных*/
    int dist;                      /*дистанция до клиента перемененная в которую будет сохраняться наименьшая дистанция для сравнения с другими*/
    int dist2;                     /*расчётная дистанция которую будем вычислять и сравнивать с distans*/
    lst = root_list;
    tax = NULL;
    cli = NULL;
    dist = 0;
    while (lst != NULL) /*бежим по списку в поисках первого попавшегося пассажира*/
    {
        if ((lst->type) == client) /*если нашли клиента то  подбираем ближайшего к не му таксиста*/
        {
            lst2 = root_list;
            while (lst2 != NULL) /*бежим по списку в поисках первого попавшегося таксиста и начинаем замерять дистанцию перебираю всех оставшихся таксистов*/
            {
                if ((lst2->type) == taxi) /*нашли таксиста.*/
                {
                    dist2 = distans(lst->cord_y, lst->cord_x, lst2->cord_y, lst2->cord_x);

                    if (dist == 0)
                    {
                        dist = dist2;
                        tax = lst2;
                        cli = lst;
                    }

                    if (dist2 < dist)
                    {
                        dist = dist2;
                        tax = lst2;
                        cli = lst;
                    }
                }
                if (lst2->ptr == NULL) /*проверяем на конечный лист в списке*/
                {
                    if (cli == NULL) /*проверка на корректное заполнение указателя на клиента*/
                        break;

                    if (cli->type == client) /*проверяем чтобы в странице клиента была заполнено что это клиент*/
                    {
                        if (tax != NULL) /*проверяем заполнен ли указатель на таксиста.*/
                        {

                            cli->type = client_busy;               /*мееняем тип клиента на занятый*/
                            tax->type = taxi_busy;                 /*меняем тип водителя на занятый*/
                            cli->tax_or_cli_ptr = tax;             /*записываем указатель на таксиста в списке пользователя, чтобы в последствии можно было освобидить его.*/
                            tax->tax_or_cli_ptr = cli;             /*записываем таксисту указатель на клиента*/
                            cli->sock_tax_or_client = tax->socket; /*переписываем сокет таксиста в страницу клиента*/
                            tax->sock_tax_or_client = cli->socket;
                            stats.kilometrs += dist; /*добавляем в статистику километры*/
                            start_message(cli->socket);
                            listprint(root_list);
                            break;
                        }
                        break;
                    }
                    break;
                }
                lst2 = lst2->ptr; /*переход на сл.страницу списка.*/
            }
        }
        lst = lst->ptr; /*переход на сл.страницу списка.*/
    }
    return 0;
}

void message_start_cheker(struct list *lst) /*проверяем чтобы стартовое сообщение отправилось*/
{

    struct list *next;
    next = root_list;

    while (next != NULL) /*бежим по списку*/
    {
        if (next->type == client_busy && next->flag_message == dont_start_connnected)
        {
            if (send(next->socket, "ser_1", strlen("ser_1"), MSG_NOSIGNAL) <= 0)
                perror("message_start_cheker");
            break;
        }
        next = next->ptr;
    }
}
void start_message(int socket) /*отправляем стартовое сообщение*/
{

    struct list *next;
    next = root_list;

    while (next != NULL) /*бежим по списку*/
    {
        if (socket == next->socket)
        {
            if (send(socket, "ser_1", strlen("ser_1"), MSG_NOSIGNAL) <= 0)

                perror("start_message");

            next->flag_message = dont_start_connnected;
        }
        next = next->ptr;
    }
}

int connects(struct list *root, int sock, char message[MICRO_BUFFER]) /*соединение первого попавшегося клиента с отсортированным для него таксистом*/
{
    struct list *next;
    next = root_list;

    while (next != NULL) /*бежим по списку*/
    {
        if (sock == next->socket)
        {
            if (next->type == client_busy)
                next->flag_message = connected; /*при запуске сообщенийй между клиентом и сервером флаг опускаем.*/
            /*printf("ot %d k %d mes %s\n", sock, next->sock_tax_or_client, message);*/
            send(next->sock_tax_or_client, message, MICRO_BUFFER, MSG_NOSIGNAL);
            if (strstr(message, "cli_3") != NULL)
            {
                next->tax_or_cli_ptr->number_of_trips += 1;              /*записываем поезду конкретному таксисту.*/
                stats.trips += 1;                                        /*записываем в обшую статистику колличество поездок*/
                next->tax_or_cli_ptr->cord_x = next->cord_distination_x; /*переписываем координаты  назначения клиента тксисту  чтобы отсчёт вёлся от них.*/
                next->tax_or_cli_ptr->cord_y = next->cord_distination_y; /* переписываем координаты  назначения клиента тксисту  чтобы отсчёт вёлся от них.*/
                break;
            }
        }
        next = next->ptr;
    }
    return 0;
}

void catch (int sig)
{
    struct list *next;
    next = root_list;

    switch (sig)
    {
    case SIGUSR1:

        listprint(root_list); /*пока выводим базу данных*/
        puts("SIGUSR1");
        break;

    case SIGINT:
    case SIGTERM:

        printf(YEL "кажется меня хотят отключить\n" RESET "" RED " I i\'ll be back\n" RESET);
        while (next != NULL)
        {
            send(next->socket, "ser_d", sizeof("ser_d"), 0);
            next = next->ptr;
        }
        exit(1);

    default:

        puts("Unknown signal");
        break;
    }
}

double distans(int cord_x_taxi, int cord_y_taxi, int cord_x_client, int cord_y_client) /*расчёт растояния между двумя точками на плоскости*/
{
    return sqrt((cord_x_client - cord_x_taxi) * (cord_x_client - cord_x_taxi) + (cord_y_client - cord_y_taxi) * (cord_y_client - cord_y_taxi));
}

int socket_creat(struct sockaddr_in servaddr, struct cfg_read *cfg)
{
    int sockfd, opt = 1;
    /*bzero(&servaddr, sizeof(servaddr));*/

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("sockfd");
        exit(1);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        close(sockfd);
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    if (listen(sockfd, 10) < 0)
    {
        perror("listen");
        close(sockfd);
        exit(1);
    }
    return sockfd;
}

struct parser_return *parser(char message_buf[MICRO_BUFFER], struct parser_return *ptr_parser)
{
    char *strtok_buffer;
    char buff[MICRO_BUFFER], buff_cord[MICRO_BUFFER], buff_cord_destination[MICRO_BUFFER];
    char buf_codr_x[MICRO_BUFFER], buf_codr_y[MICRO_BUFFER], buf_codr_disrination_x[MICRO_BUFFER], buf_codr_disrination_y[MICRO_BUFFER];

    for (int str = 3, sr = 0; message_buf[str] != '\0'; str++, sr++) /*+3 у str  для пропуска cli\tax в сообщении*/
        buff[sr] = message_buf[str];

    /*определяем клиента/таксиста*/
    if (strstr(message_buf, "cli") != NULL)
        ptr_parser->type = client;
    if (strstr(message_buf, "tax") != NULL)
        ptr_parser->type = taxi;

    /*дробим строку пополам и записываем всё во временную переменную*/
    strtok_buffer = strtok(buff, " ");
    if (strtok_buffer != NULL)
    {
        strcpy(buff_cord, strtok_buffer);
    }

    strtok_buffer = strtok(NULL, " ");
    if (strtok_buffer != NULL)
    {
        strcpy(buff_cord_destination, strtok_buffer);
    }

    /*вытаскиваем кординаты такси/клиента*/
    strtok_buffer = strtok(buff_cord, "_"); /*дробим строку пополам и вытаскиваем*/
    if (strtok_buffer != NULL)
    {
        strcpy(buf_codr_y, strtok_buffer);
    }

    strtok_buffer = strtok(NULL, "_");
    if (strtok_buffer != NULL)
    {
        strcpy(buf_codr_x, strtok_buffer);
    }

    /*вытаскиваем координаты назнаечения*/
    strtok_buffer = strtok(buff_cord_destination, "_");
    if (strtok_buffer != NULL)
    {
        strcpy(buf_codr_disrination_y, strtok_buffer);
    }

    strtok_buffer = strtok(NULL, "_");
    if (strtok_buffer != NULL)
    {
        strcpy(buf_codr_disrination_x, strtok_buffer);
    }

    sscanf(buf_codr_x, "%*c%*c%d", &ptr_parser->cord_x);
    sscanf(buf_codr_y, "%*c%*c%d", &ptr_parser->cord_y);
    sscanf(buf_codr_disrination_x, "%*c%*c%d", &ptr_parser->cord_distination_x);
    sscanf(buf_codr_disrination_y, "%*c%*c%d", &ptr_parser->cord_distination_y);

    return ptr_parser;
}

struct list *addelem(struct list *lst, struct parser_return *ptr_parser, int sock)
{
    struct list *temp, *p;
    printf("addelem\n");
    if (root_list == NULL) /*добавление первого элемента списка*/
    {
        temp = (struct list *)malloc(sizeof(struct list));
        temp->type = ptr_parser->type; /* сохранение поля данных добавляемого узла*/
        temp->ptr = NULL;
        root_list = temp;
    }
    else
    {
        temp = (struct list *)malloc(sizeof(struct list)); /*добавление  в конец списка*/
        p = lst->ptr;                                      /* сохранение указателя на следующий узел*/
        lst->ptr = temp;                                   /* предыдущий узел указывает на создаваемый*/
        temp->type = ptr_parser->type;                     /* сохранение поля данных добавляемого узла*/
        temp->ptr = p;                                     /* созданный узел указывает на следующий элемент*/
    }
    temp->cord_y = ptr_parser->cord_y;
    temp->cord_x = ptr_parser->cord_x;
    temp->sock_tax_or_client = 0;
    temp->cord_distination_y = ptr_parser->cord_distination_y;
    temp->cord_distination_x = ptr_parser->cord_distination_x;
    temp->number_of_trips = 0; /*просто занулили.*/
    temp->socket = sock;
    temp->flag_message = -1;
    listprint(root_list);
    return (temp);
}

struct list *deletelem(struct list *del_list) /*del_list-указатель на удаляемый узел*/
{

    struct list *temp;
    if (del_list == root_list && del_list->ptr == NULL) /*удаляем первый лист в списке идуший после хэдера если он единственный*/
    {
        free(del_list); /*удаляем страницу*/
        root_list = NULL;
        listprint(root_list);
        return root_list;
    }
    else /*проеверяем что страница не ппоследняя в списке.*/
    {
        temp = root_list;
        while (temp != NULL) /*ищем предшевствуюший лист удаляймому.*/
        {
            if (temp->ptr == del_list) /*если нашли страницу идушую перед удаляймой*/
            {
                if (del_list->ptr != NULL) /*проверяем есть ли другие страницу за этой или она последняя и по разному обрабатываем перезапись указателя.*/
                {
                    temp->ptr = del_list->ptr;
                }
                else /*если страница последняя.*/
                {
                    temp->ptr = NULL;
                }

                break;
            }
            if (root_list == del_list) /*если страниуца стоит после рут листа.*/
            {
                if (del_list->ptr != NULL)
                {
                    root_list = del_list->ptr;
                }
                else /*если страница последняя.*/
                {
                    root_list = NULL;
                }
                break;
            }
            temp = temp->ptr;
        }

        if (del_list->type == client_busy) /*если данному клиенту была подобрана пара то мы выспобождаем ее.*/
        {
            del_list->tax_or_cli_ptr->type = taxi;
        }

        if (del_list->type == taxi_busy) /*есди данному таксисту была подобрана пара то высвобождаем её.*/
        {
            del_list->tax_or_cli_ptr->type = client;
        }

        free(del_list);
        sort(root_list);

        for (temp = root_list;; temp = temp->ptr) /*возвращаем новую последнюю страницу.*/
        {
            if (temp->ptr == NULL)
                break;
        }
        listprint(root_list);
        return temp;
    }
}

void listprint(struct list *lst) /*вывод элементов списка*/
{

    struct list *p;
    system("clear");
    printf("server pid-%d\n", getpid());
    printf(CYN "total trips-" WHT "%d" CYN ",\ntotal kilometers-" WHT "%d" CYN ";\n" RESET, stats.trips, stats.kilometrs);
    if (root_list == NULL) /*если список пуст то выводим  сообщение ниже.*/
    {
        fputs("\x1b[K", stdout);
        printf(RED "список пуст\n" RESET);
        return;
    }
    p = root_list; /*вывод всех клиентов*/
    printf(BLU "\n __|CLIENT|_____________________________________________________________\n" RESET);
    while (p != NULL)
    {
        if (p->type == client)
            printf(BLU "|" RESET "FREE socket=|%2d| vezet=|%2d| y=|%3d| x=|%3d| naznacnenie y=|%3d| x=|%3d|" BLU "|\n" RESET, p->socket, p->sock_tax_or_client, p->cord_y, p->cord_x, p->cord_distination_y, p->cord_distination_x);
        /*printf(BLU"|"RESET"type|%d| socket=|%2d| vezet=|%2d| y=|%3d| x=|%3d| poin=%14p ptr=%14p tax-%p"BLU"|\n"RESET,p->type,p->socket,p->sock_tax_or_client,p->cord_y,p->cord_x,p,p->ptr,p->tax_or_cli_ptr); // вывод значения элемента p*/
        if (p->type == client_busy)
            printf(BLU "|" RESET "BUSY socket=|%2d| vezet=|%2d| y=|%3d| x=|%3d| naznacnenie y=|%3d| x=|%3d|" BLU "|\n" RESET, p->socket, p->sock_tax_or_client, p->cord_y, p->cord_x, p->cord_distination_y, p->cord_distination_x);
        if (p->ptr == NULL)
            printf(BLU "'-----------------------------------------------------------------------'\n" RESET);
        p = p->ptr; /* переход к следующему узлу*/
    }

    p = lst; /*вывод всех таксистов*/
    printf(YEL "\n __|TAXI|_____________________________________\n" RESET);
    while (p != NULL)
    {
        if (p->type == taxi)
            printf(YEL "|" RESET "FREE socket=|%2d| y=|%3d| x=|%3d| poezdok=|%2d|" YEL "|\n" RESET, p->socket, p->cord_y, p->cord_x, p->number_of_trips);
        /*printf(YEL"|"RESET"type|%d| socket=|%2d| y=|%3d| x=|%3d| poezdok=|%2d| poin=%14p ptr=%14p cli-%p"YEL"|\n"RESET,p->type,p->socket,p->cord_y,p->cord_x,p->number_of_trips,p,p->ptr,p->tax_or_cli_ptr); // вывод значения элемента p*/
        if (p->type == taxi_busy)
            printf(YEL "|" RESET "BUSY socket=|%2d| y=|%3d| x=|%3d| poezdok=|%2d|" YEL "|\n" RESET, p->socket, p->cord_y, p->cord_x, p->number_of_trips);
        if (p->ptr == NULL)
            printf(YEL "'---------------------------------------------'\n" RESET);
        p = p->ptr; /* переход к следующему узлу*/
    }
}
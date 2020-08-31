#include "server_header.h"

int main()
{
    char buffer[BUFLEN];
    int sock; /*cord буфер для пересохранения кординат иначе затиралось.*/
    int i, sockfd, newsockfd, maxfd, client[MAX_CLIENT], maxi;
    struct sockaddr_in servaddr;
    struct list *next_list, *list;
    struct sigaction sign;
    struct timeval time;
    struct cfg_read cfg;
    fd_set set, allset; /*объявление  новых буферов.*/

    printf("server pid-%d\n", getpid());

    /*обработка ошибок*/
    sigemptyset(&sign.sa_mask); /*инициализируем набор  сигналов*/
    sign.sa_flags = 0;
    sign.sa_handler = catch;
    sigaction(SIGUSR1, &sign, NULL);
    sigaction(SIGTERM, &sign, NULL);
    sigaction(SIGINT, &sign, NULL);

    cfg = *cfg_reader(&cfg);             /*заполняем структуру с портом и айпи, в случае сервера только порт.*/
    list = next_list = root_list = NULL; /*создаём ХЭДЕР списка и сразу его записываем в глобальную переменную для обрашения их любой функции*/

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(cfg.PORT));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    sockfd = socket_creat(servaddr, &cfg); /*создаём сокет.*/

    /*настройки для select*/
    time.tv_usec = 110;
    time.tv_sec = 0;

    maxfd = sockfd; /*забираем номер сокета для селекта*/
    for (i = 0; i < MAX_CLIENT; i++)
        client[i] = -1; /*высвобождаем от мусора массив для  сокетов чтобы небыло ложных срабатываний*/

    maxi = -1;               /*для цикла на проверку отвала/наличия данных  перезаписываем самым большим сокетом чтобы не бегать по циклу в холостую.*/
    FD_ZERO(&allset);        /*зануляем маску*/
    FD_SET(sockfd, &allset); /*устанавливаем сокет в маску("добавляем")*/

    while (1)
    {
        set = allset; /*присваиваем значения структур*/
        int ready;
        ready = select(maxfd + 1, &set, NULL, NULL, &time);

        if (ready == -1)
            perror("select");

        if (FD_ISSET(sockfd, &set)) /*соединение с новым клиентом*/
        {
            socklen_t szaddr = sizeof(servaddr);
            newsockfd = accept(sockfd, (struct sockaddr *)&servaddr, &szaddr);

            if (newsockfd == -1)
            {
                perror("accept");
            }

            if (read(newsockfd, buffer, BUFLEN) > 0)
            {
                ptr_parser = *parser(buffer, &ptr_parser);              /*чекаем такси/клиент и забираем координаты*/
                next_list = addelem(next_list, &ptr_parser, newsockfd); /*добавление нового клиента*/
                sort(root_list);
            }

            for (i = 0; i < MAX_CLIENT; i++) /*добавляем новый сокет  в маску.*/

                if (client[i] < 0)
                {
                    client[i] = newsockfd;
                    break;
                }

            if (i == MAX_CLIENT)
            {
                perror("слишком много пользователей");
            }
            FD_SET(newsockfd, &allset); /*добавляем сокет нового соединения в маску для дальнейшего отслеживания.*/
            if (newsockfd > maxfd)
                maxfd = newsockfd;
            if (i > maxi) /*проверяем maxi чтобы не бегать по  циклу в пустую проверяя пустые сокеты.*/
                maxi = i;
            if (--ready <= 0) /*больше нет готовых для чтения дискрипторов*/
                continue;
        }

        for (i = 0; i <= maxi; i++) /*проверяем всех клиентов на наличие данных*/
        {
            if ((sock = client[i]) < 0) /*если пустая маска для дескриптора.*/
                continue;
            if (FD_ISSET(sock, &set)) /*провверка сокета на чтение*/
            {
                if ((read(sock, buffer, BUFLEN)) == 0) /*проверка на разорванное соединение*/
                {
                    for (list = root_list;; list = list->ptr) /*ищем  страницу в списке по номеру разорванного сокета*/
                    {
                        if (list->socket == sock) /* нашли по сокету страницу и отправили её на удаление*/
                        {
                            next_list = deletelem(list);
                            break;
                        }
                    }
                    close(sock);           /*закрываем отвалившийся сокет*/
                    FD_CLR(sock, &allset); /*удаляем сокет из маски*/
                    client[i] = -1;        /*записываем -1  чтобы переиспользовать эту ячейку повторно.*/
                }
                else
                {
                    fcntl(sock, F_SETFL, O_NONBLOCK); /*чтобы не блокироваться на этом вызове.*/
                    read(sock, buffer, BUFLEN);
                    connects(root_list, sock, buffer);
                }
                if (--ready <= 0)
                {
                    break; /*конец списка дискрипторов*/
                }
            }
        }
        message_start_cheker(root_list); /*проверяем началось ли общение между клиентом и такси.*/

        time.tv_usec = 0;
        time.tv_sec = 1;
    }
}
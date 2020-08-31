#include "header_client_taxi.h"

int main()
{

    int maxfd, num_state = 0;
    char request[buffer_size];
    struct timeval time;
    struct sockaddr_in serv_addr;
    struct cfg_read cfg;
    fd_set set;

    cord_ptr = *cord_generator(&cord_ptr); /*генерим свои координаты*/
    sprintf(cord_ptr.cord_buffer, "cli_y=%d_x=%d _y=%d_x=%d", cord_ptr.cord_y, cord_ptr.cord_x, cord_ptr.cord_distination_y, cord_ptr.cord_distination_x);
    printf(GRN "мои координаты y=%d x=%d,я еду y=%d x=%d\n" RESET, cord_ptr.cord_y, cord_ptr.cord_x, cord_ptr.cord_distination_y, cord_ptr.cord_distination_x);

    /*вызов функции чтения конфига*/
    cfg = *cfg_reader(&cfg);

    /*заполнение  структуры сети*/
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(cfg.IP);
    serv_addr.sin_port = htons(atoi(cfg.PORT));
    sock = start_connect(serv_addr, &cord_ptr);

    /*обработка переменных для SELECT*/
    maxfd = sock;

    /*заполнение временных итервалов для select*/
    time.tv_sec = 0;
    time.tv_usec = 0; /*я выюрал вариант когда select проходит по дискрипторам 1 раз и освобождает цикл до новго вхождения в select. крч не блокируемся*/

    /*сразу скидываем координаты серверу*/
    send(sock, cord_ptr.cord_buffer, micro_buffer, MSG_NOSIGNAL);
    for (;;)
    {
        FD_ZERO(&set);
        FD_SET(sock, &set);
        select(maxfd + 1, &set, NULL, NULL, &time);

        if (FD_ISSET(sock, &set))
        {
            if ((recv(sock, request, buffer_size, 0)) == 0)
            {
                printf("соединение разорвано\n");
                FD_CLR(sock, &set);
                close(sock);
                sock = start_connect(serv_addr, &cord_ptr);
                printf("NEW SOCKET %d\n", sock);
                num_state = 0;
            }
        }

        int rec;
        rec = recv(sock, request, buffer_size, MSG_DONTWAIT);
        /*printf("mas %s, num_state %d\n", request, num_state);*/
        if (rec == 0) /*отправляем повторно свои координаты.*/
            send(sock, request, sizeof(request), MSG_NOSIGNAL);

        if (strstr(request, "ser_1") != NULL) /*сброс счётчика если такси отвалилось*/
        {

            num_state = 0;
            printf(GRN "мои координаты y=%d x=%d,я еду y=%d x=%d\n" RESET, cord_ptr.cord_y, cord_ptr.cord_x, cord_ptr.cord_distination_y, cord_ptr.cord_distination_x);
        }

        else if (strcmp(request, "tax_9") == 0) /*обработка ошибки когда присылается сообщение с другой сессии.*/
        {
            num_state = 0;
            printf(RED "num_state=0\n" RESET);
            send(sock, table_massage_client[num_state].request_massge, sizeof(table_massage_client[num_state].request_massge), MSG_NOSIGNAL);
            printf(YEL "%s\n" RESET, table_massage_client[num_state].console_massage);
            num_state++;
        }
        if (strstr(request, table_massage_client[num_state].massage) != NULL) /*обработка сообщений и ответ на них*/
        {
            if (num_state == 3)
            {
                sprintf(request, "cli_3_%d", 1 + rand() % (1 - 6));
                send(sock, request, sizeof(request), MSG_NOSIGNAL);
                num_state++;
            }
            if (num_state == 4)
            {
                break;
            }
            send(sock, table_massage_client[num_state].request_massge, sizeof(table_massage_client[num_state].request_massge), MSG_NOSIGNAL);
            printf(YEL "%s\n" RESET, table_massage_client[num_state].console_massage);
            num_state++;
            memset(request, 0, sizeof(request));
        }
        else if (strcmp(request, "ser_d") == 0) /*обработка смерти сервера.*/
        {
            close(sock);
            printf(RED "SERVER умер...\n" RESET);
            exit(1);
        }

        memset(request, 0, sizeof(request));
        time.tv_usec = 0; /*я выюрал вариант когда select проходит по дискрипторам 1 раз и освобождает цикл до новго вхождения в select. крч не блокируемся*/
        time.tv_sec = 0;
        sleep(1);
    }
    printf(RED "Я ВЫЖИЛ И ДОЕХАЛ!\n" RESET);
    close(sock);
    exit(1);
}
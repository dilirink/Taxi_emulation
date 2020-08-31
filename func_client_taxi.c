#include "func_client_taxi.h"

int start_connect(struct sockaddr_in servaddr, struct cord *ptr)
{
    int sock_fd;
    while (1)
    {
        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("ERROR SOCKET\n");
            return -1;
        }

        /*Пытаемся приконнектиться*/
        if (connect(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            /*perror("connect");*/
            close(sock_fd);
        }

        if (send(sock_fd, ptr->cord_buffer, strlen(ptr->cord_buffer), MSG_NOSIGNAL) <= 0) /*начинаем общение с отправки координат.*/
        {
            /*perror("send");*/
        }
        else
        {
            break;
        }
    }

    return sock_fd;
}

struct cord *cord_generator(struct cord *cord_ptr)
{

    int a_rand = 1, b_rand = MAP_SIZA;
    srand(time(NULL));
    cord_ptr->cord_x = a_rand + rand() % (b_rand - a_rand + 1);
    cord_ptr->cord_y = a_rand + rand() % (b_rand - a_rand + 1);
    cord_ptr->cord_distination_y = a_rand + rand() % (b_rand - a_rand + 1);
    cord_ptr->cord_distination_x = a_rand + rand() % (b_rand - a_rand + 1);
    return cord_ptr;
}

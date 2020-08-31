#ifndef _HEAD_FUNC_
#define _HEAD_FUNC_

#include "master_header.h"

struct cord
{
    int cord_x;
    int cord_y;
    int cord_distination_y;
    int cord_distination_x;
    char cord_buffer[micro_buffer];
} cord_ptr;

int sock;

struct cord *cord_generator(struct cord *cord_ptr);               /* генерация кординат принимает в себя размер карты*/
int start_connect(struct sockaddr_in servaddr, struct cord *ptr); /*создаём сокет*/

#endif
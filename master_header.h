#ifndef _HEAD_MASTER_
#define _HEAD_MASTER_

#define _POSIX_SOURCE /*иначе ругался на sigemptyset*/

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <sys/select.h>
#include <signal.h>

#define MAP_SIZA 100
#define micro_buffer 30
#define buffer_size 256
#define BUFLEN 256

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m" /* example  printf("This is " RED "red" RESET " and this is " BLU "blue" RESET "\n");*/

#endif
CC= gcc
CFLAGS = -c
STANDART= -std=c99
EROR_FLAGS = -Wall -Werror -pedantic -ansi
LIB_CLI_PROG =  -L. -lcfg_reader -L. -lfunc_client_taxi
LIB_SER = -L. -lcfg_reader -lm
SRCS = client taxi server
SRC_SERVER = server_main.c server_func.c
SRC_CLI= client.c
SRC_TAX=taxi.c
LIB1= cfg_reader.c
LIB2= func_client_taxi.c
C_FILE_SERVER = server_func.c server_main.c
.PHONY: clean_binary_file clean all

all:  $(SRCS)



client:  lib$(LIB1:.c=.a) lib$(LIB2:.c=.a) $(SRC_CLI) #сборка клиента
	$(CC) $@.c -o $@ $(LIB_CLI_PROG) $(EROR_FLAGS) $(STANDART)

taxi: lib$(LIB1:.c=.a) lib$(LIB2:.c=.a) $(SRC_TAX)  #сборка таксиста
	$(CC)  $@.c -o $@ $(LIB_CLI_PROG) $(EROR_FLAGS) $(STANDART)

server: lib$(LIB1:.c=.a) $(SRC_SERVER) #сборка сервера
	$(CC) $(SRC_SERVER) -o $@ $(LIB_SER) $(EROR_FLAGS) $(STANDART)

lib$(LIB1:.c=.a) : $(LIB1)
	$(CC)  $(CFLAGS) $<
	ar rc $@ $(LIB1:.c=.o)
	ranlib $@

lib$(LIB2:.c=.a) : $(LIB2)
	$(CC)  $(CFLAGS) $<
	ar rc $@ $(LIB2:.c=.o)
	ranlib $@

clean:
	rm -f *.o *.a
clean_binary_file:
	find ./* -prune  -perm /111  -type f  -exec rm -v {} \;

# prune зашита от входа в подкатологи.
# perm поиск по разрешениям файла.
# -type f искать только файлы.
#  rm -v пояснение что именно он удаляет.
# передача аргументов от find {}.

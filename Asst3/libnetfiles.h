#ifndef _libnetfiles_h_
#define _libnetfiles_h_

#define INVALID_FILE_MODE 6969
#define UNRESTRICTED 0
#define EXCLUSIVE 1
#define TRANSACTION 2 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

int netserverinit(char * hostname, int filemode);

int netopen(const char * pathname,int flags);

ssize_t netread(int fildes, void *buf, size_t nbyte);

ssize_t netwrite(int fildes, const void *buf, size_t nbyte);

int netclose(int fildes);

#endif
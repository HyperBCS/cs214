#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>

extern int errno;

int main(){
	char * h = calloc(1,12);
	int fd = open("test.txt",0);
	int a = read(fd,h,6);
	int b = read(fd,h,12);
	printf("A: %d\nB: %d\n",a,b);
	printf("%s\n",strerror(errno));
}
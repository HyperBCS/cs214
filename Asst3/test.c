#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

fd_set rfds;
int fildes[2];  

void * begin(){
	printf("hi\n");
	sleep(2);
	write(fildes[1], "Hello world\n", 12);
	return 0;
}

void * sel(){
	struct timeval tv;
	tv.tv_sec = 5;
    tv.tv_usec = 0;
	int retval = select(fildes[1], &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */

   if (retval == -1)
        perror("select()");
    else if (retval)
        printf("Data is available now.\n");
        /* FD_ISSET(0, &rfds) will be true. */
    else
        printf("No data within five seconds.\n");

  	return 0;
}

int main(void)
{
    struct timeval tv;
    int retval;
	pthread_t tid[2];
   /* Watch stdin (fd 0) to see when it has input. */ 
	int status = pipe(fildes);
    FD_ZERO(&rfds);
    FD_SET(fildes[0], &rfds);
    pthread_create(&tid[0], NULL, begin, 0);
    pthread_create(&tid[1], NULL, sel, 0);
   /* Wait up to five seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

   retval = select(fildes[1], &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */

   if (retval == -1)
        perror("select()");
    else if (retval)
        printf("Data is available now.\n");
        /* FD_ISSET(0, &rfds) will be true. */
    else
        printf("No data within five seconds.\n");

   exit(EXIT_SUCCESS);
}
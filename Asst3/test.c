#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include "libnetfiles3.c"
// make header file with all include and function definition

int main(void)
{
  // if(argc != 3){
  //  printf("Incorrect arguments\n");
  //  exit(1);
  // }
  // char * hostname = argv[1];
  char * hostname = "localhost";
  netserverinit(hostname,1);
  char * filename = "song.mp3";
  char * filename2 = "song2.mp3";
  int fdd = netopen(filename,O_WRONLY);
  int fdd2 = netopen(filename2,O_RDONLY);
  printf("[%d] %s\n",fdd,strerror(errno));
   FILE * file = fopen(filename, "r");
   fseek(file, 0L, SEEK_END);
   int size = ftell(file);
  printf("SIZE: %d\n",size);
  // //printf("FD: %d\n",fdd);
   char * txt = calloc(1,size);
   printf("Read: %zd\n",netread(fdd,txt,size));
   printf("Write: %zd\n",netwrite(fdd2,txt,size));
   printf("[%d] %s\n",errno,strerror(errno));
  // int fcc =  netclose(fdd);
  //  printf("Netclose %d\n[%d] %s\n",fcc,errno,strerror(errno));
  // // int fdd2 = netopen(filename2,O_RDWR);
}
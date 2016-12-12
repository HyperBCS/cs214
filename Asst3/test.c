#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include "libnetfiles2.c"

int main(void)
{
  // if(argc != 3){
  //  printf("Incorrect arguments\n");
  //  exit(1);
  // }
  // char * hostname = argv[1];
  char * hostname = "localhost";
  netserverinit(hostname,1);
  // char * filename = "pic.png";
  // char * filename2 = "pic2.png";
  // int fdd = netopen(filename,O_RDONLY);
  //  int fdd2 = netopen(filename2,O_RDWR);
  //  FILE * file = fopen(filename, "r");
  //  fseek(file, 0L, SEEK_END);
  //  int size = ftell(file);
  // printf("SIZE: %d\n",size);
  // //printf("FD: %d\n",fdd);
  //  char * txt = calloc(1,size);
  // printf("Read: %zd\n",netread(fdd,txt,size));
  // // printf("Read: %zd\n",netread(fdd,txt,size));
  // printf("Write: %zd\n",netwrite(fdd2,txt,size));
  //  //fclose(file);
  // // int fdd2 = netopen(filename2,O_RDWR);
  char * filename = "pic.png";
  int fdd = netopen(filename,O_RDWR);
  printf("FD\n");
  sleep(8);
  netclose(fdd);
}
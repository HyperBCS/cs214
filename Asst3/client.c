#include "libnetfiles.h"
// make header file with all include and function definition

int main(void)
{
  char * hostname = "localhost";
  netserverinit(hostname,1);
  char * filename = "song.mp3";
  char * filename2 = "song2.mp3";
  int fdd = netopen(filename,O_RDWR);
  int fdd2 = netopen(filename2,O_RDWR);
  FILE * file = fopen(filename, "r");
  fseek(file, 0L, SEEK_END);
  int size = ftell(file);
  printf("SIZE: %d\n",size);
  char * txt = calloc(1,size);
  printf("Read: %zd\n",netread(fdd,txt,size));
  printf("Write: %zd\n",netwrite(fdd2,txt,size));
  printf("[%d] %s\n",errno,strerror(errno));
  int fcc =  netclose(fdd);
  printf("Netclose %d\n[%d] %s\n",fcc,errno,strerror(errno));
  free(txt);
  return 0;
}
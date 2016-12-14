#include "libnetfiles.h"
// make header file with all include and function definition

int main(void)
{
  // if(argc != 3){
  //  printf("Incorrect arguments\n");
  //  exit(1);
  // }
  // char * hostname = argv[1];
  char * hostname = "localhost";
  netserverinit(hostname,2);
  printf("%d\n",netopen("pic.jpg",O_RDWR));
  printf("%d\n",netopen("d",O_RDWR));
  printf("%d\n",netopen("pic2.jpg",O_RDWR));
  printf("%d\n",netopen("pic.jpg",O_RDONLY));
   printf("[%d] %s\n",errno,strerror(errno));
  // sleep(8);
  // int cc = netclose(fdd);
  // printf("[%d] %s\n",cc,strerror(errno));

}
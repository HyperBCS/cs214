#include "mymalloc.h"

int main(){
int * i[3000];
int j;
for(j = 0;j < 3000;j++){
	i[j] = malloc(1);
	//*i[j] = 10;
}
// for(j = 0;j < 1500;j++){
// 	*i[j] = 2147483647;
// }
// for(j = 0;j < 1500;j++){
// 	printf("i[%d]: %d\n",j,*i[j]);
// }
listmem();
for(j = 0;j < 3000;j++){
	free(i[j]);
}
listmem();
return 0;
}
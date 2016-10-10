#include "mymalloc.h"

int main(){
int * i[850];
int j;
for(j = 0;j < 834;j++){
	i[j] = (int*)malloc(sizeof(int));
	//*i[j] = 10;
}
for(j = 0;j < 833;j++){
	*i[j] = 2147483647;
}
for(j = 0;j < 833;j++){
	printf("i[%d]: %d\n",j,*i[j]);
}
listmem();
for(j = 0;j < 834;j++){
	free(i[j]);
}
listmem();
return 0;
}
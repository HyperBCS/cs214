#include "mymalloc.h"

int main(){

int i = 0;
int* j[1500];
for(i = 0;i  < 1250;i++){
	j[i] = malloc(1);
}
for(i = 1249;i  >= 0;i--){
	free(j[i]);
}
	listmem();
	return 0;
}
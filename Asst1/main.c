#include "mymalloc.h"

int main(){
int i;
int * j[1500];
for(i = 0;i < 120;i++){
	j[i] = malloc(40);
}

for(i = 118;i >= 0;i--){
	free(j[i]);
}

listmem();

	return 0;
}
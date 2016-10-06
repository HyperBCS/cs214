#include "mymalloc.h"

int main(){

int* a = malloc(4998);
free(a);
free(a);
free(0);

	listmem();
	return 0;
}
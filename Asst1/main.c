#include "mymalloc.h"

int main(){
	int j = foo2(2);
	int * p = (int*)malloc(10);
	printf("%p\n",p);
	return 0;
}
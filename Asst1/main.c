#include "mymalloc.h"

int main(){
	int j = foo2(2);
	int * p = (int*)malloc(10);
	char * q = (char*)malloc(300);
	int * r = (int*)malloc(4658);
	int * s = (int*)malloc(50000);
		listmem();
	q = "THIS WORKS";
	printf("%s\n",q);
	//int * r = (int*)malloc(5000);
	//int * r = (int*)malloc(5000);
	//printf("%p\n",p);
	//printf("%p\n",q);
	return 0;
}
#include <stdio.h>
#include <time.h>
//#include "mymalloc.h"

int main(){
	int i = 0;
	int j[75];
	for (i = 0;i< 50;i++){
		j[i] = (int)malloc(100);
	}
	for (i = 0;i< 25;i++){
		free(j[i]);
	}
	listmem();
}
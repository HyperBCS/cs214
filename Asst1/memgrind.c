
#include <stdio.h>
#include <time.h>
#include "mymalloc.h"

void loadA(){

    // --- WORKLOAD A ---
    char* a[3000];
    int i = 0;
    //Allocating 1 byte pointers - 3000 times
    for(i = 0; i < 3000; i++){
        a[i] = (char*) malloc(1);
    }
    //Freeing 1 byte pointers - 3000 times
    for(i = 0; i < 3000; i++){
        free(a[i]);
    }

}

void loadB(){
    int i = 0;
    char* b;
    //Allocating 1 byte pointer then Freeing 1 byte pointer - 3000 times
    for(i = 0; i < 3000; i++){
        b = (char *) malloc(1);
        free(b);
    }

}

void loadC(){
    //Workload C
    int i = 0;
    int mallocd;
    int net;
    int sum;
    int free = 0;
    int choice;
    int pos;
    char *grid[3000];
    int list[3000];
    for(i = 0;i<3000;i++){
        grid[i] = 0;
    }
    while(mallocd < 3000 || free < 3000){
        choice = rand() % 2;
        if(net == 0){
            choice = 1;
        } else if(mallocd >= 2999){
            choice = 0;
        }
        //choice = 1;
        if(choice == 1){
            while(1){
                pos = rand() % 3000;
                //printf("POS: %d GRID: %p\n",pos,grid[pos]);
                if((int)grid[pos] == 0){
                    //printf("FULL pos: %d\n",pos);
                    grid[pos] = malloc(rand() % 1000);
                    if(grid[pos] == 0){
                        break;
                    }
                    mallocd++;
                    net++;
                    break;
                } else{
                    continue;
                }
            }
        } else{
            while(1){
               // printf("FREE\n");
                pos = rand() % 3000;
                if((int)grid[pos] == 0){
                    free(grid[pos]);
                    continue;
                } else{
                    short int size = *(short int*)(grid[pos]-2) & 1;
                    if(size == 1){
                        free(grid[pos]);
                        free++;
                        net--;
                        break;
                    }
                }
            }
        }
    }

}

void loadD(){
    //-- WORKLOAD D ---
    int i = 0;
    int mallocd;
    int net;
    int sum;
    int free = 0;
    int choice;
    int pos;
    char *grid[3000];
    int list[3000];
    for(i = 0;i<3000;i++){
        grid[i] = 0;
    }
    while(mallocd < 3000 || free < 3000){
        choice = rand() % 2;
        if(net == 0){
            choice = 1;
        } else if(mallocd >= 2999){
            //listmem();
            choice = 0;
        }
        //choice = 1;
        if(choice == 1){
            while(1){
                pos = rand() % 3000;
                //printf("POS: %d GRID: %p\n",pos,grid[pos]);
                if((int)grid[pos] == 0){
                    //printf("FULL pos: %d\n",pos);
                    grid[pos] = malloc(rand() % 5000);
                    if(grid[pos] == 0){
                        break;
                    }
                    mallocd++;
                    net++;
                    break;
                } else{
                    continue;
                }
            }
        } else{
            while(1){
               // printf("FREE\n");
                pos = rand() % 3000;
                if((int)grid[pos] == 0){
                    continue;
                } else{
                    short int size = *(short int*)(grid[pos]-2) & 1;
                    if(size == 1){
                        free(grid[pos]);
                        free++;
                        net--;
                        break;
                    }
                }
            }
        }
    }
}

int main(){
    struct timeval t0;
    struct timeval t1;
    long elapsed;
    int i = 0;
    printf("\n==========WORKLOAD A==========\n");
    gettimeofday(&t0, 0);
    for(i = 0; i < 100; i++) {
        loadA();
    }
    gettimeofday(&t1, 0);
    elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
    printf("Average time: %luμs\n",elapsed/100);
    listmem();

    printf("\n==========WORKLOAD B==========\n");
    gettimeofday(&t0, 0);
    for(i = 0; i < 100; i++) {
        loadB();
    }
    gettimeofday(&t1, 0);
    elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
    printf("Average time: %luμs\n",elapsed/100);
    listmem();

    printf("\n==========WORKLOAD C==========\n");
    gettimeofday(&t0, 0);
    for(i = 0; i < 100; i++) {
        loadC();
    }
    gettimeofday(&t1, 0);
    elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
    printf("Average time: %luμs\n",elapsed/100);
    listmem();

    printf("\n==========WORKLOAD D==========\n");
    gettimeofday(&t0, 0);
    for(i = 0; i < 100; i++) {
        loadD();
    }
    gettimeofday(&t1, 0);
    elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
    printf("Average time: %luμs\n",elapsed/100);
    listmem();

    return 0;
}
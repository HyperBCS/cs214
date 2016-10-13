
#include <stdio.h>
#include <time.h>
#include "mymalloc.h"

void loadA(){

    // --- WORKLOAD A ---
    printf("\n==========WORKLOAD A==========\n");
    char* a[3000];
    int i = 0;
    printf("-Allocating 1 byte pointers - 3000 times\n\t");
    for(i = 0; i < 3000; i++){
        a[i] = (char*) malloc(1);
    }
    listmem();
    printf("\n-Freeing 1 byte pointers - 3000 times\n\t");
    for(i = 0; i < 3000; i++){
        free(a[i]);
    }
    listmem();

}

void loadB(){
    int i = 0;
    printf("\n==========WORKLOAD B==========\n");
    char* b;
    listmem();
    printf("\n-Allocating 1 byte pointer then Freeing 1 byte pointer - 3000 times\n\t");
    for(i = 0; i < 3000; i++){
        b = (char *) malloc(1);
        free(b);
    }
    listmem();

}

void loadC(){

    // -- WORKLOAD C ---
    printf("\n==========WORKLOAD C==========\n");
    int i = 0;
    int mallocd = 0;
    int net = 0;
    int free = 0;
    int choice = 0;
    int pos = 0;
    char *grid[3000];
    int list[3000];
    for(i = 0;i<3000;i++){
        grid[i] = 0;
    }
    while(mallocd < 3000 || free < 3000){
        choice = rand() % 2;
        if(net == 0){
            choice = 1;
        } else if(mallocd == 2999){
            choice = 0;
        }
        //choice = 1;
       // printf("CHOICE: %d\n",choice);
        if(choice == 1){
            while(1){
                pos = rand() % 3000;
                //printf("POS: %d GRID: %p\n",pos,grid[pos]);
                if((int)grid[pos] == 0){
                    //printf("FULL pos: %d\n",pos);
                    grid[pos] = malloc(1);
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
    printf("FREE: %d\n",free);
    listmem();

}

void loadD(){
        //-- WORKLOAD D ---
    printf("\n==========WORKLOAD D==========\n");
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
    listmem();
}

int main(){
    // struct timeval t0;
    // struct timeval t1;
    // long elapsed;
    // int i = 0;
    // gettimeofday(&t0, 0);
    // for(i = 0; i < 100; i++) {
    //     loadC();
    // }
    // gettimeofday(&t1, 0);
    // elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
    // printf("ELAPSED TIME: %lu\n",elapsed);
    loadC();

    return 0;
}
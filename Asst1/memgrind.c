
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
    //Workload B
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
    int mallocd = 0;
    int net = 0;
    int free = 0;
    int choice = 0;
    int pos = 0;
    // initialize the pointer array
    char *grid[3000];
    // Zero out the array because it may not be empty
    for(i = 0;i<3000;i++){
        grid[i] = 0;
    }
    // Work until we malloc and free 3000 times total
    while(mallocd < 3000 || free < 3000){
        // Choose randomly to free or malloc
        choice = rand() % 2;
        if(net == 0){
            choice = 1;
        } else if(mallocd >= 2999){
            choice = 0;
        }
        // If 1 we malloc if 0 we free
        if(choice == 1){
            while(1){
                // Allocating 1 byte
                grid[mallocd] = malloc(1);
                if(grid[mallocd] == 0){
                    break;
                }
                mallocd++;
                net++;
                break;
            }
        } else{
            while(1){
                // We need to keep track of free's so we use metadata to determine if we successfully freed
                pos = rand() % mallocd;
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

void loadD(){
    //-- WORKLOAD D ---
    int i = 0;
    int mallocd = 0;
    int net = 0;
    int free = 0;
    int choice = 0;
    int pos = 0;
    // initialize the pointer array
    char *grid[3000];
    // Zero out the array because it may not be empty
    for(i = 0;i<3000;i++){
        grid[i] = 0;
    }
    // Work until we malloc and free 3000 times total
    while(mallocd < 3000 || free < 3000){
        // Choose randomly to free or malloc
        choice = rand() % 2;
        if(net == 0){
            choice = 1;
        } else if(mallocd >= 2999){
            choice = 0;
        }
        // If 1 we malloc if 0 we free
        if(choice == 1){
            while(1){
                // Allocating a random amount
                grid[mallocd] = malloc(rand() % 5000);
                if(grid[mallocd] == 0){
                    break;
                }
                mallocd++;
                net++;
                break;
            }
        } else{
            while(1){
                // We need to keep track of free's so we use metadata to determine if we successfully freed
                pos = rand() % mallocd;
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

// Workload E places an artificial limit with the upper, middle, or lower third allocated. This choice is random
void loadE(){
    //Workload E
    int i = 0;
    int mallocd = 0;
    int net = 0;
    int free = 0;
    int choice = rand() % 3;
    char * upper = malloc(1664);
    char * mid = malloc(1664);
    char * lower = malloc(1664);
    // Choose what chunk we allocate first
    if(choice == 2){
        free(lower);
        free(mid);
    } else if(choice == 1){
        free(lower);
        free(upper);
    } else{
        free(upper);
        free(mid);
    }
    int pos;
    // Initialize the pointer array
    char *grid[3000];
    for(i = 0;i<3000;i++){
        grid[i] = 0;
    }
    // Work until we malloc and free 3000 times total
    while(mallocd < 3000 || free < 3000){
        // Choose randomly to free or malloc
        choice = rand() % 2;
        if(net == 0){
            choice = 1;
        } else if(mallocd >= 2999){
            choice = 0;
        }
        // If 1 we malloc if 0 we free
        if(choice == 1){
            while(1){
                // Mallocing 1 byte
                grid[mallocd] = malloc(1);
                if(grid[mallocd] == 0){
                    break;
                }
                mallocd++;
                net++;
                break;
            }
        } else{
            while(1){
                // We need to keep track of free's so we use metadata to determine if we successfully freed
                pos = rand() % mallocd;
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
    // Free left over space
    free(upper);
    free(mid);
    free(lower);
}

// Allocates 3000 bytes then free from middle going out until it reaches the ends
void loadF(){

    // --- WORKLOAD F ---
    char* a[1250];
    int i = 0;
    //Allocating 1 byte pointers - 3000 times
    for(i = 0; i < 1250; i++){
        a[i] = (char*) malloc(1);
    }
    //Freeing 1 byte pointers - 3000 times
    for(i = 0; i < 1250/2; i++){
        free(a[1250/2 + i]);
        free(a[1250/2 - i-1]);
    }

}

int main(){
    struct timeval t0;
    struct timeval t1;
    long elapsed;
    int i = 0;
    srand(time(NULL));
    // Start the testing. We run the loop 100 times and use gettimeofday to record the average difference in time.
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

    printf("\n==========WORKLOAD E==========\n");
    gettimeofday(&t0, 0);
    for(i = 0; i < 100; i++) {
        loadE();
    }
    gettimeofday(&t1, 0);
    elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
    printf("Average time: %luμs\n",elapsed/100);
    listmem();

    printf("\n==========WORKLOAD F==========\n");
    gettimeofday(&t0, 0);
    for(i = 0; i < 100; i++) {
        loadF();
    }
    gettimeofday(&t1, 0);
    elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
    printf("Average time: %luμs\n",elapsed/100);
    listmem();

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>

// Use a struct to send arguments to thread function.
struct arg_struct {
    char * filename;
    int len;
    int offset;
    int tid;
    int part;
}arg;

// Output function to generate file names. Very handy
char * output(char * filename,int part){
	char * file_n = (char*)calloc(1,255*sizeof(char));
    strcat(file_n,filename);
    char part_n[sizeof(int)];
    sprintf(part_n, "%d", part);
    strcat(file_n,"_LOLS");
    if(part >= 0){
 	   strcat(file_n,part_n);
	}
    int i;
    for (i=0; file_n[i]!= '\0'; i++){
        if (file_n[i] == '.'){
        	file_n[i] = '_';
        }
    }
    return file_n;
}

// Main RLE encoding section
void * encode(void* arg){
	// Put the struct into variables better to keep track.
	struct arg_struct *args = (struct arg_struct *)arg;
	char * filename = args->filename;
	int len = args->len;
	int offset = args->offset;
	int part = args->part;
	FILE * file = fopen(filename, "r");
	// Attempt to open file throw error if we cannot
	if (file == NULL) {
		printf("ERROR: Failed to open file.\n");
		return 0;
	}
	fseek(file,offset,SEEK_SET);
	// Setup the pointers to be used and some init variables.
    char * prev = (char*)calloc(2,sizeof(char));
    char * ptr = (char*)calloc(2,sizeof(char));
    char * tempStr = (char*)calloc(len+1,sizeof(char));
    char * strPtr = tempStr;
    char temp[10*sizeof(char)];
    int cLen = 0;
    int count = 1;
    int done = 0;
    // THis is the main part of RLE. while(1) is risky but ill take the chance.
    while(1) {
    	// Detect if we are done scanning file. If so we need to prepare to exit
		if(fscanf(file, "%c", ptr) == EOF || len<=0){
			*ptr = 0;
			done = 1;
		}
		// If we encounter an alphabet char enter for statement
        if(isalpha(*ptr) || *ptr == 0){
        	// if multiple chars increment count, else flush count and add prev char to output string.
      		if(*prev == *ptr && *prev != 0){
      			count++;
      		} else if(*ptr != *prev && *prev != 0){
      			if(count == 2){
              memcpy(strPtr, prev, 1);
              strPtr++;
      				memcpy(strPtr, prev, 1);
              strPtr++;
      			} else if(count == 1){
      				memcpy(strPtr, prev, 1);
              strPtr++;
      			} else{
      				sprintf(temp, "%d", count);
              cLen = strlen(temp);
              memcpy(strPtr, temp, cLen);
              strPtr += cLen;
      				memcpy(strPtr, prev, cLen);
      				strPtr++;
      			}

  			count = 1;
  		}
  		*prev = *ptr;
    	}
		if(done == 1){
			break;
		}
		len--;

		}
    FILE * fp;
    // Attempt to open file and if we cannot write exit with error.
    char * file_n = output(filename,part);
    fp = fopen (file_n, "w+");
    if(fp == NULL){
      printf("ERROR: Failed to write file, please check permissions.\n");
    }else{
      fprintf(fp, "%s", tempStr);
      fclose(fp);
    }
    // Free EVERYTHING make valgrind happy with "All heap blocks were freed -- no leaks are possible" :)
    fclose(file);
    free(file_n);
    free(prev);
    free(ptr);
    free(tempStr);
    return 0;
}

// Where the program starts.
int main(int argc, char ** argv){
	// Check for correct argument numbers
	if(argc != 3){
		printf("ERROR: Expected 2 arguments (filepath and number of parts)\n");
		return 1;
	}
	char * filename = argv[1];
	FILE * file = fopen(filename, "r");
	// CHeck if we can open the file.
	if (file == NULL) {
		printf("Failed to open file.\n");
		return 1;
	}
	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	printf("SIZE: %d\n",size);
	rewind(file);
	fclose(file);
	int parts = 0;
	parts = atoi(argv[2]);
	// Error if empty file. Wasted cycles otherwise.
	if(size <= 0){
		printf("ERROR: Empty file\n");
		return 1;
	}
	// Check if threads > chars. If so lower threads = chars
	if(parts > size){
		printf("More threads than chars, lowering thread count to char count\n");
		parts = size;
	// Sanity check for parts
	} else if(parts <= 0){
		printf("ERROR: Please enter a number greater than 0\n");
		return 1;
	}
	// THread tid array. May be useful someday but not today.
	pthread_t * tid = (pthread_t*)calloc(parts,sizeof(pthread_t));
	int part_size = size / parts;
	int extra = size % parts;
	int offset = 0;
	int first = 1;
	struct arg_struct *args[parts];
	int i = 0;
	// This for loop calculates all arguments to be sent to the thread function. MATH.
	for(i = 0;i<parts;i++){
		args[i] = (struct arg_struct *)malloc(sizeof(arg));
		if(first){
			args[i]->len = part_size + extra;
			args[i]->offset = offset;
			offset += part_size + extra;
			first = 0;
		}else{
			args[i]->len = part_size;
			args[i]->offset = offset;
			offset += part_size;
		}
		if(parts == 1){
			args[i]->part = -1;
		} else{
			args[i]->part = i;
		}
		args[i]->filename = filename;
	}
	// Hey, we're creating the threads here. Wonderful
	for(i = 0;i<parts;i++){
		args[i]->tid = i;
		pthread_create(&tid[i], NULL, encode, (void*)args[i]);
	}
	// We need to join them together so our program doesn't end before we are done.
	for(i = 0;i<parts;i++){
		pthread_join(tid[i], NULL);
	}
	// Free everything so it makes us happy.
	for(i = 0;i<parts;i++){
		free(args[i]);
	}
	free(tid);
	return 0;
}
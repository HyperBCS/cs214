#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    *strPtr = 0;
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
  // if someone tries to run binary it needs to be correct
	if(argc != 6){
		printf("ERROR: Expected 5 arguments (filename, length, offset, child #, part #)\n");
		return 1;
	}
  // Wanted some similarity between thread and process version so we call encode the same way with the struct.
  struct arg_struct *args = calloc(1,sizeof(arg));
  args->filename = argv[1];
  args->len = atoi(argv[2]);
  args->offset = atoi(argv[3]);
  args->tid = atoi(argv[4]);
  args->part = atoi(argv[5]);
  // After we finish encoding free then return.
  encode(args);
  free(args);
  return 0;
}
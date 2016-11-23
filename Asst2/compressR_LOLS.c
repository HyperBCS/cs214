#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// This is the process version which does not do any compression, but calls the compression binary.
int main(int argc, char ** argv){
	// Check for valid number of arguments
	if(argc != 3){
		printf("ERROR: Expected 2 arguments (filepath and number of parts)\n");
		return 1;
	}
	char * filename = argv[1];
	FILE * file = fopen(filename, "r");
	// Check if file can be read or exists.
	if (file == NULL) {
		printf("Failed to open file.\n");
		return 1;
	}
	// We need to find the size of the file to do math on it.
	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	printf("SIZE: %d\n",size);
	rewind(file);
	fclose(file);
	int parts = 0;
	parts = atoi(argv[2]);
	// If the file is empty throw error and return.
	if(size <= 0){
		printf("ERROR: Empty file\n");
		return 1;
	}
	// If there are more parts than chars lower part count to char count.
	if(parts > size){
		printf("More threads than chars, lowering thread count to char count\n");
		parts = size;
	// Look for invalid part numbers
	} else if(parts <= 0){
		printf("ERROR: Please enter a number greater than 0\n");
		return 1;
	}
	file = fopen("compressR_worker_LOLS", "r");
	// Check that we can actually exec because binary may be missing.
	if (file == NULL) {
		printf("ERROR: Worker binary not found in folder. Please compile compressR_worker_LOLS.c to compressR_worker_LOLS.\n");
		return 1;
	}
	fclose(file);
	int part_size = size / parts;
	int extra = size % parts;
	int offset = 0;
	int first = 1;
	char ** args = calloc(7,sizeof(char*));
	// Keep track of child pids. May be useful someday but today is not the day.
	pid_t * childPID = calloc(parts, sizeof(pid_t));
	int i = 0;
	for(i = 2; i < 6;i++){
		args[i] = (char*)calloc(1,10*sizeof(char));
	}
	args[0] = "./compressR_worker_LOLS";
	args[1] = filename;
	args[6] = NULL;
	// Calculate arguments to be sent to the worker file.
	for(i = 0;i<parts;i++){
		if(first){
			sprintf(args[2], "%d", part_size + extra);
			sprintf(args[3], "%d", offset);
			offset += part_size + extra;
			first = 0;
		}else{
			sprintf(args[2], "%d", part_size);
			sprintf(args[3], "%d", offset);
			offset += part_size;
		}
		if(parts == 1){
			sprintf(args[5], "%d", -1);
		} else{
			sprintf(args[5], "%d", i);
		}
		sprintf(args[4], "%d", i);
		
		childPID[i] = fork();
		if(childPID[i] == 0) // child process
        {	
        	execvp("./compressR_worker_LOLS",args);
        	printf("ERROR: Forking failed, exiting....\n");
		    free(childPID);
			for(i = 2;i<6;i++){
				free(args[i]);
			}
			free(args);
			return 1;
        }
     // Wait for processes to finish and then free everything
	}
	for(i = 0;i<parts;i++){
		wait(NULL);
	}
	free(childPID);
	for(i = 2;i<6;i++){
		free(args[i]);
	}
	free(args);
	return 0;
}
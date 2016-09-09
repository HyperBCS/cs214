#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*
GLOBALS
*/

char *curr_char;
int i = 0;
int n = 0;
int starting;
int ending;

/*
LINKED LIST NODE
*/

struct Node {
	char *value;
	struct Node *next;
};

/*
BEGINS ITERATION FROM SPECIFIED STARTING POINT AND CREATES TOKEN UPON REACHING NON-ALPHABETIC CHARACTER
*/

char *tokenize(char *tempstr){
	while(curr_char[i] != '\0'){
		if(!isalpha(curr_char[i])){
			break;
		}
		i++;
	}

	ending = i;
	char *token = (char*)malloc((ending-starting)*(sizeof(char)));
	
	int j;
	int x = 0;

	for(j = starting; j < ending; j++){
		token[x] = tempstr[j];
		x++;
	}
	
	token[x]='\0';
	return token;
}

char sorter(char ** strings, int len){
   	char * temp = strings[0];
   	strings[0] = strings[1];
   	strings[1] = temp;
	return ** strings;
}

int partition (char ** arr, int l, int h){
    char * x = arr[h];
    int i = (l - 1);
 
    for (int j = l; j <= h- 1; j++){

        if (strcmp(arr[j],x) < 0){
            i++;
		   	char * temp = arr[i];
		   	arr[i] = arr[j];
		   	arr[j] = temp;
        }
    }
    
    char * temp = arr[i+1];
	arr[i+1] = arr[h];
	arr[h] = temp;
	return (i + 1);
}

void quickSort(char ** A, int l, int h){
    if (l < h){
        int p = partition(A, l, h); 
        quickSort(A, l, p - 1);  
        quickSort(A, p + 1, h);
    }
}

int main(int argc, char **argv){
	if (argc != 2) {
		fprintf(stderr, "ERROR: No Arguments Detected!\n");
		exit(1);
	}

	curr_char = argv[1];
	char *input_str = argv[1];
	int length_of_input = strlen(input_str);

	struct Node *p;
	struct Node *head = 0;
	struct Node *tail = 0;
	struct Node *next;
	
	while(curr_char[i] != '\0' && i < length_of_input){
		n++;
		starting = i;
		char *outputted_token = tokenize(input_str);
		if(strlen(outputted_token) <= 0){
			i++;
			continue;
		}
		p = (struct Node *)malloc(sizeof(struct Node));
		p->value = outputted_token;
		p->next = 0;

		if(head == 0){
			head = p;
			tail = p;
		}else{
			tail->next = p;
			tail = p;
		}

		i++;
	}

	char **strings = (char**)malloc(n*sizeof(char*));
	n = 0;
	int linked_list_length = 0;
	for(p = head; p != 0 ; p = p->next, n++){
		strings[n] = p->value;
		linked_list_length++;
	}

   quickSort(strings, 0,linked_list_length-1);
   for( n = 0 ; n < linked_list_length; n++ ){
   		printf("%s\n", strings[n]);
   }

   free(strings);
   for (p = head; p != 0; p = next){
   		next = p->next;
   		free(p);
   }

	return 0;
}
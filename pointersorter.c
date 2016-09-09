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
	char *token = (char*)malloc((ending-starting+1)*(sizeof(char)));
	
	int j;
	int x = 0;

	for(j = starting; j < ending; j++){
		token[x] = tempstr[j];
		x++;
	}
	
	token[x]='\0';
	return token;
}

void swap(char ** word, int a, int b){
	char * temp = word[a];
	word[a] = word[b];
	word[b] = temp;
}

/*
Performs partitioning for the array which includes placing all the words less than pivot to left of pivot and greater words to the right.
*/
int partition (char ** words, int s, int f){
	char * x = words[f];
	int i = (s - 1);
 	int j;
	for (j = s; j <= f - 1; j++){
		if (strcmp(words[j],x) < 0){
			i++;
			swap(words,i,j);
		}
	}
	swap(words,i+1,f);
	return (i + 1);
}


/*
Recursively partitions the array. s is starting, and f is ending index. Arr is the input array of words
*/
void quickSort(char ** arr, int s, int f){
	if (s < f){
		int p = partition(arr, s, f); 
		quickSort(arr, s, p - 1);  
		quickSort(arr, p + 1, f);
	}
}

int main(int argc, char **argv){
	if (argc != 2) {
		fprintf(stderr, "ERROR: No Arguments Detected!\n");
		return 1;
	}

	curr_char = argv[1];
	char *input_str = argv[1];
	int length_of_input = strlen(input_str);

	struct Node *p;
	struct Node *head = 0;
	struct Node *tail = 0;
	struct Node *next;
	
	while(curr_char[i] != '\0' && i < length_of_input){
		starting = i;
		char *outputted_token = tokenize(input_str);
		if(strlen(outputted_token) <= 0){
			free(outputted_token);
			i++;
			continue;
		}
		p = (struct Node *)malloc(sizeof(struct Node));
		n++;
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

	if(linked_list_length == 0){
		fprintf(stderr, "ERROR: Blank String!\n");
		free(strings);
		return 1;
	}

   quickSort(strings, 0,linked_list_length-1);
   for( n = 0 ; n < linked_list_length; n++ ){
		printf("%s\n", strings[n]);
		free(strings[n]);
   }

   /*
   Freeing dynamic memory allocation
   */
   free(strings);
   for (p = head; p != 0; p = next){
		next = p->next;
		free(p);
   }

	return 0;
}
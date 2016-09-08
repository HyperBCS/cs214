#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*
GLOBALS
*/

char *curr_char;
int i = 0;
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
		printf("%c\n", curr_char[i]);
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

int main(int argc, char **argv){
	curr_char = argv[1];
	char *input_str = argv[1];
	printf("%s\n", input_str);
	int length_of_input = strlen(input_str);

	struct Node *p;
	struct Node *head = 0;
	struct Node *tail = 0;
	struct Node *next;
	
	while(curr_char[i] != '\0' && i < length_of_input){
		starting = i;
		char *outputted_token = tokenize(input_str);

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

	int linked_list_length = 0;
	for(p = head; p != 0 ; p = p->next){
		printf("%s\n", p->value);
		linked_list_length++;
	}
	printf("%d\n", linked_list_length);

	return 0;
}
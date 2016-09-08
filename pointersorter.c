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

char sorter(char ** strings, int len){

   	char * temp = strings[0];
   	strings[0] = strings[1];
   	strings[1] = temp;
	return ** strings;
}


/* A typical recursive C/C++  implementation of QuickSort */
 
/* This function takes last element as pivot, places 
   the pivot element at its correct position in sorted 
   array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right 
   of pivot */
int partition (char ** arr, int l, int h)
{
	//printf("Partitioning");
    char * x = arr[h];
    int i = (l - 1);
 
    for (int j = l; j <= h- 1; j++)
    {
        if (strcmp(arr[j],x) < 0)
        {
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
 
/* A[] --> Array to be sorted, 
  l  --> Starting index, 
  h  --> Ending index */
void quickSort(char ** A, int l, int h)
{
	//printf("in quick sort");
    if (l < h)
    {        
        /* Partitioning index */
        int p = partition(A, l, h); 
        quickSort(A, l, p - 1);  
        quickSort(A, p + 1, h);
    }
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
		n++;
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
	printf("Length: %d\n",n);
	char **strings = (char**)malloc(n*sizeof(char*));
	n = 0;
	int linked_list_length = 0;
	for(p = head; p != 0 ; p = p->next, n++){
		strings[n] = p->value;
		linked_list_length++;
	}

   quickSort(strings, 0,linked_list_length-1);
   //sorter(strings, linked_list_length);

   printf("Before sorting the list is: \n");
   for( n = 0 ; n < linked_list_length; n++ ) 
   {
      printf("%s ", strings[n]);
      printf("\n");
   }


	return 0;
}
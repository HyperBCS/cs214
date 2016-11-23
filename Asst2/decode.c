#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


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

void decode(char * filename, int part){
    char * file_n = output(filename,part);
    FILE *f = fopen(file_n, "rb");
      if (f == NULL) {
    printf("Failed to open file.\n");
    free(file_n);
    return;
  }
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    rewind(f);
    char * rle = (char*)calloc(1,1000000);
    char * res = (char*)calloc(1,1000000);
    fread(rle, len, 1, f);
    fclose(f);

  char let[2];
  char * n;
  let[1] = '\0';
  int count = 0;
  int i = 0;
  for (i = 0;i < strlen(rle);i++){
    int j = 0;
    if(!isalpha(rle[i])){
      n = rle + i;
      char * end;
      int num = strtol(n,&end,10);
      int digits = end - n;
      for(j = 1; j < num;j++){
        memcpy(let,rle + i + digits,sizeof(char));
        strcat(res,let);
      }
      i = i + digits-1;
    } else if(rle[i] == rle[i+1]){
      memcpy(let,rle + i,sizeof(char));
      strcat(res,let);
      strcat(res,let);
      i = i + 1;
    } else{
      memcpy(let,rle + i,sizeof(char));
      strcat(res,let);
    }
  }
  printf("%s",res);
  free(rle);
  free(res);
  free(file_n);
  return;
}
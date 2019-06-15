//ptr.c
//
//refresher on some of the finer points of C
//--------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <limits.h>
#include <string.h>


int main(void){
  
  char *ch1;
  char buf[200];
  char keyword[] = "HI"; 
  char buf2[] = "abcdeHIfghHIijklmno";
  int max_buff_size = 200;
  int i;  

  i = 0;
  while((ch1 = (strstr(buf2,keyword))) != NULL){
    
    *ch1 = '%';
    while((&buf2[i] != &ch1) && (buf2[i] != '%')){
      fprintf(stderr,"%c",buf2[i]);
      i++;
    }//while not at the substring
    i += strlen(keyword);

    fprintf(stderr,"\e[7m");
    fprintf(stderr,"%s",keyword);
    fprintf(stderr,"\e[0m");

  }//while ch1 = (strstr(buf2,keyword)) != NULL)
  
  while(buf2[i] != '\0'){
    fprintf(stderr,"%c",buf2[i]);
    i++;
  }//while not at the substring
  
  fprintf(stderr,"\n--------------------------\n");
  
  return 0;
}//main
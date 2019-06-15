//nick powers
//4 may 2014
/*
 refresher on pointer linked list
 pg 123 of Kruse & Ryba Data Structures
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){

  struct snode {
    char *str;
    struct snode *next;
  };//string node
  
  char line[1024];
  struct snode *head = NULL;
  struct snode *tail = NULL;
  struct snode *newnode, *ptr;

  if(argc != 1){
    fprintf(stderr, "usage: ./nodes\n"); 
    exit(1);
  }//if

  while(fgets(line, 1024, stdin) == line){
    
    if((newnode = (struct snode *)malloc(sizeof(struct snode))) == NULL){
      fprintf(stderr, "nodes: malloc failure for newnode\n");
      exit(1);
    }//if

    if((newnode->str = malloc(strlen(line)+1)) == NULL){
      fprintf(stderr, "nodes: malloc failure for newnode->str\n");
      exit(1);
    }//if

    if(strncpy(newnode->str, line, strlen(line)+1) == NULL){
      fprintf(stderr,"nodes: strcpy problem\n");
      exit(1);
    }//if

    newnode->next = NULL;

    //first insertion is a special case
    if(tail == NULL)
      head = tail = newnode;

    else {
       tail->next = newnode;
       tail = newnode;
    }//else
  }//while

  //print the list
  for(ptr = head; ptr != NULL; ptr = ptr->next)
     printf("%s", ptr->str);  

  exit(0);
}//main
//sayings.c
//nick powers
//5 feb 2016
//
//
//Usage: 
//$ grep -n -H  [search-word] [file1] [file2] | ./sayings [-b] [-h] [search-word] 
// or
//$ grep -n -H pattern [file1] [file2] > [tmpfile] ; ./sayings [searchword] < [tmpfile]
//
//This program takes the output from a valid grep -n -H [search-word] call of the format
//filename:line#:linetext
//
//example:
//% grep -n -H elephant art computers
//art:44:A fool-proof method for sculpting an elephant: first, get a huge block of
//art:45:marble; then you chip away everything that doesn’t look like an elephant.
//art:426:God is really only another artist.  He invented the giraffe, the elephant
//computers:679:An elephant is a mouse with an operating system.
//
//sayings takes grep output for its input. 
//From it, sayings learns the input files to check, and
//it prints each entire saying in which the keyword appears.
//
//The sayings flags are -b for Banner, and -h for Highlight
//Both are optional and can appear in any order before the search-string
//
//Banner mode indicates the number of sayings that contain the search string
//before printing each saying 
//
//Highlight mode highlights the searchstring in each saying
//It is achieved with  \e[7m  and  \e[0m  as the beginning & terminating strings 
//for highlighting text in the xterm.
//
//Uses dynamically allocated data structures and multiple files 
//
// malloc(3) 
// stdio(3) 
// string(3) 
// scanf(3) 
// printf(3) 
// fgets(3) 
// grep(1)
//
//NAME_MAX <-- max number of bytes in a filename
//--------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

//definition for linked lists
//adapted from Kearn's slides set 1, slides 31,32,33 
struct linkedlist{
  char *line;
  struct linkedlist *next;
  
};

int main(int argc, char *argv[]){

  //declare variables
  int i,j,k;
  int hits;
  char curr_line[1024];
  char fname[1024];
  char keyword[1024];
  char *ch;
  struct linkedlist *files_head = NULL;
  struct linkedlist *files_tail = NULL;
  struct linkedlist *saying_head = NULL;
  struct linkedlist *saying_tail = NULL;
  struct linkedlist *newnode, *ptr, *temp;
  FILE *infile;
  int found;
  int b_flag;
  int h_flag;


  //check the command line arguments for sayings
  switch(argc){     
  case 0:
    fprintf(stderr, "usage optional flags: ./sayings [-b] [-h] keyword\n");
    exit(1);
    break;
    
  case 1:
    fprintf(stderr, "usage optional flags: ./sayings [-b] [-h] keyword\n");
    exit(1);
    break;
    
  case 2:
    if(strlen(argv[1]) > 1023){
      fprintf(stderr, "bad argument: argv[1] > 1023\n");
      exit(1);
    }//if bad
    strncpy(keyword,argv[1], 1024);
    break;
    
  case 3:
    if((strcmp(argv[1],"-b") != 0) && (strcmp(argv[1],"-h") != 0)){
      fprintf(stderr, "usage optional flags: ./sayings [-b] [-h] keyword\n");
      exit(1);
    }//if bad flags
    else{//one good flag is set
      if(strcmp(argv[1],"-b") == 0){
	b_flag = 1;
	h_flag = 0;
      }//if banner mode
      else{
	h_flag = 1;
	b_flag = 0;
      }//if highlight mode
    }//otherwise set the flags
    
    if(strlen(argv[2]) > 1023){
      fprintf(stderr, "bad argument: argv[1] > 1023\n");
      exit(1);
    }//if bad keyword
    strncpy(keyword,argv[2], 1024);
    break;
    
  case 4:

    if(((strcmp(argv[1],"-b") != 0) && (strcmp(argv[1],"-h") != 0)) ||
       ((strcmp(argv[2],"-b") != 0) && (strcmp(argv[2],"-h") != 0)) ||
       (strcmp(argv[1], argv[2]) == 0)){
      fprintf(stderr, "usage optional flags: ./sayings [-b] [-h] keyword\n");
      exit(1);	
    }//if bad flags
    else{
      b_flag = 1;
      h_flag = 1;
    }//else set the flags

    if(strlen(argv[3]) > 1023){
      fprintf(stderr, "bad argument: argv[1] > 1023\n");
      exit(1);
    }//if bad keyword
    strncpy(keyword,argv[3], 1024);
    break;
    
  default:
    fprintf(stderr, "usage: ./sayings -b -h keyword\n");
    exit(1);
    break;
  }//switch
  
  //fprintf(stderr, "About to read from stdin\n");
  //read data from standard input
  while(fgets(curr_line,1024,stdin) == curr_line){

    //fprintf(stderr,"valid line read\n");
    if((newnode = (struct linkedlist *)malloc(sizeof(struct linkedlist))) == NULL){
      fprintf(stderr, "nodes: malloc failure for newnode\n");
      exit(1);
    }//if bad malloc for newnode
    if((newnode->line = malloc(strlen(curr_line)+1)) == NULL){
      fprintf(stderr, "nodes: malloc failure for newnode->line\n");
      exit(1);
    }//if bad malloc for newnode->line
    if(strncpy(newnode->line, curr_line, strlen(curr_line)+1) == NULL){
      fprintf(stderr,"nodes: strcpy problem\n");
      exit(1);
    }//if bad strncpy
    newnode->next = NULL;
    //ready to append

    if(files_tail == NULL){  //first file name added
      //fprintf(stderr,"First file name append\n");

      if(strstr(curr_line,":") == NULL){
	fprintf(stderr,"<line without ':'> ");//do nothing
      }//if bad input or no results
      
      else{
	//fprintf(stderr,"First good-ish file name added\n");
	//substring copy the file name from curr_line to fname
	j = 0;
	while(curr_line[j] != ':'){
	  fname[j] = curr_line[j];
	  j++;
	}//while
	fname[j] = '\0';
	
	if(strncpy(newnode->line, fname, strlen(fname)+1) == NULL){
	  fprintf(stderr,"strncpy from fname failed in first append\n");
	}//if strncpy failed
	else{
	files_head = files_tail = newnode;
	}//else successful copy
      }//else good-ish input
    }//if first item

    //else new addition
    else{
      if(strstr(curr_line,":") == NULL){
	;//do nothing, bad line
      }//if newnode->line doesnt have a ':', so ignore
      else{
	//fprintf(stderr,"nth file name addition\n");
	j = 0;
	while(curr_line[j] != ':'){
	  fname[j] = curr_line[j];
	  j++;
	}//while
	fname[j] = '\0';

	if(strncpy(newnode->line,fname,strlen(fname)+1) == NULL){
	  fprintf(stderr,"strncpy from fname failed in later append\n");
	}//if strncpy failed
	else{
	  if(strcmp(files_tail->line, newnode->line) == 0){
	    //fprintf(stderr,"Repeat file name\n");//skip
	  }//if it's a repeat
	  else {
	    files_tail->next = newnode;
	    files_tail = newnode;
	  }//else append new file name
	}//else successful fname string n copy
      }//else it has a ':'
    }//else append to the linked list
  }//while getting from stdin
  //---------------------------------------

  //done with stdin
  //now we have a linked list of file names
  //which contain the keyword to search  
  //fprintf(stderr,"\nDone with Stdin\n");

  newnode = files_head;
  j = 0;
  while(newnode != NULL){
    j++;
    //fprintf(stderr,"%s\n", newnode->line);
    newnode = newnode->next;
  }//print loop

  found = 0;
  hits = 0;
  ptr = files_head;
  //fprintf(stderr,"About to enter first files' pass\n");
  while(ptr != NULL){//first pass to get total hit count
    if((infile = fopen(ptr->line, "r")) == NULL){
      //fprintf(stderr,"read file: %s :didn't open. skipped\n", ptr->line);
      ptr = ptr->next;//skip over bad file
    }//the read file didnt open
    else{
      //fprintf(stderr,"read file: %s :opened successfully\n", ptr->line);
      while(fgets(curr_line,1024,infile) == curr_line){
	if(strstr(curr_line,keyword) != NULL){
	  //fprintf(stderr,"key found: %s\n", curr_line);
	  hits++;//this saying has the keyword at least once
	  while((fgets(curr_line,1024,infile) == curr_line) && 
		(strstr(curr_line,"%") != NULL)){
	    ;//fprintf(stderr,"this saying already has key, skip line: %s\n",curr_line);
	  }//while in same saying
	  //fprintf(stderr,"last line/percent: %s\n", curr_line);//1st line of new saying
	}//if needle is in haystack
	else{
	  ;//fprintf(stderr,"no key in line: %s\n", curr_line);//no key
	}//else line had no keyword
      }//while
      ptr = ptr->next;
      fclose(infile);//close file stream
    }//else good file open
    found = 1;
  }//while more files

  //done with first pass through files to get hit count in case of banner mode
  if(b_flag == 1){
    fprintf(stderr,"%d witty sayings contain the string %s\n\n",hits,keyword);
  }//if banner mode set
  
  //second pass:
  //for each input file, for each saying,
  //store in a linked list
  //while storing, check for keyword
  //after saying, if keyword is present, print the saying
  //empty the linked list
  newnode = NULL;
  ptr = files_head;
  //fprintf(stderr,"About to enter second files' pass\n");
  while(ptr != NULL){
    if((infile = fopen(ptr->line, "r")) == NULL){
      //fprintf(stderr,"read file: %s :didn't open. skipped\n", ptr->line);
      ptr = ptr->next;//skip over bad file
    }//the read file didnt open
    else{
      //fprintf(stderr,"read file: %s :opened successfully (again?)\n", ptr->line);
      //while read curr_line from infile
      saying_head = saying_tail = NULL;

      while(fgets(curr_line, 1024, infile) == curr_line){
	
	if(strstr(curr_line,"%")){ 
	  newnode = saying_head;
	  found = 0;
	  while((newnode != NULL) && (found == 0)){
	    if(strstr(newnode->line,keyword) != NULL){
	      found = 1;
	    }//if the keyword is found
	    newnode = newnode->next;
	  }//while still in the saying and keyword is not found
	 
	  if(found == 1){
	    newnode = saying_head;
	    if(h_flag == 1){
	      //fprintf(stderr,"highlight mode is on\n");
	      //********************************************************************
	      while(newnode != NULL){
		
		//character by character traversal
		k = 0;
		while((ch = (strstr(newnode->line,keyword))) != NULL){
		  *ch = '%';
		  while((&(newnode->line[k]) != &ch) && (newnode->line[k] != '%')){
		    fprintf(stderr,"%c", newnode->line[k]);
		    k++;
		  }//while not at substring
		  //fprintf(stderr,"(k is %d)", k);
		  k += strlen(keyword);
		  //fprintf(stderr,"(now k is %d)", k);
		  
		  fprintf(stderr,"\e[7m");
		  fprintf(stderr,"%s",keyword);
		  fprintf(stderr,"\e[0m");
		  
		}//while theres a substring
		
		while(newnode->line[k] != '\0'){
		  fprintf(stderr,"%c",newnode->line[k]);
		  k++;
		}//while not at end of array
		//fprintf(stderr,"\n");

		newnode = newnode->next;
	      }//while newnode != NULL, still in saying
	      //fprintf(stderr,"\n");
	      //********************************************************************
	    }//do highlight stuff
	    else{
	      while(newnode != NULL){
		fprintf(stderr,"%s",newnode->line);
		newnode = newnode->next;
	      }//while newnode != NULL //still in saying
	    }//else no highlight, just print the whole sayingsaying
	    fprintf(stderr,"--------------------------\n");
	    found = 0; 
	  }//if the keyword was found
	  
	  //done with this saying, discard
	  newnode = saying_head;
	  while(newnode != NULL){
	    temp = newnode;
	    newnode = newnode->next;
	    free(temp);
	  }//while newnode not NULL, still freeing
	  saying_head = saying_tail = NULL;
	}//if curr_line has a "%", end case
	
	else{
	  if((newnode = (struct linkedlist *)malloc(sizeof(struct linkedlist))) == NULL){
	    fprintf(stderr, "nodes: 2nd pass malloc failure for newnode\n");
	    exit(1);
	  }//if bad malloc
	  if((newnode->line = malloc(strlen(curr_line)+1)) == NULL){
	    fprintf(stderr, "nodes: 2nd pass malloc failure for newnode->line\n");
	    exit(1);
	  }//if bad malloc for node field
	  if(strncpy(newnode->line, curr_line, strlen(curr_line)+1) != newnode->line){
	    fprintf(stderr, "nodes: 2nd pass bad strncpy\n");
	    exit(1);
	  }//if 
	  newnode->next = NULL;
	  
	  if(saying_tail == NULL){
	    //fprintf(stderr,"first line of this saying appended\n");
	    saying_head = saying_tail = newnode;
	  }//if first line appended
	  else{//append additional line
	    saying_tail->next = newnode;
	    saying_tail = newnode;
	    //fprintf(stderr,"another line appended to saying\n");
	  }//else other line

	}//else construct the saying

      }//while not EOF
      ptr = ptr->next;
    }//else: successful file open
  }//while ptr not NULL: more files of great sayings to read
  
  exit(0);
}//main
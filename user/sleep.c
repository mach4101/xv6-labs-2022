#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/*
int
atoi (const char *s) {
  int n; 
  n = 0;
  while('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  return n;
}
*/

int
main(int argc, char *argv[]) {
  
  // if the user forgets to pass an argument, sleep would print an error message
  if(argc <= 1) {
    fprintf(2, "usage: sleep pattern [file...]\n");
    exit(1);
  }

  // obtain the command-line arguments passed to a program
  char *timestr = argv[1];

  //convert a string to an integer
  int time = atoi(timestr);

  sleep(time);
  exit(0);
  
}

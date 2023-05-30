#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  char *exec_argv[MAXARG];
  
  int idx = 0;
  for(int i = 1; i < argc; ++i) {
    exec_argv[idx ++] = argv[i];
  }

  char ch;
  char buf[512];
  int cdx = 0;

  while(read(0, &ch, 1)) {
    if(ch != '\n') {
      buf[cdx ++] = ch;
    } else {
      buf[cdx] = '\0';
      
      int id = fork();
      if(id == 0) {
        exec_argv[idx] = buf;
        exec(argv[1], exec_argv);
      } else {
        wait((int *) 0);
        cdx = 0;
      }
    
    }
  }

  exit(0);

}

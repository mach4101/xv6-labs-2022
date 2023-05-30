#include "kernel/types.h"
#include "user/user.h"

# define WR 1
# define RE 0


__attribute__((noreturn))
void 
seive(int fd) {
  int res;
  if(read(fd, &res, 4) == 0){ 
    // if read the end of the pipeline, exit!!
    exit(0);
  }
  
  int p[2];
  pipe(p);

  int pid = fork();

  if(pid == 0) {
    // child
    close(p[WR]);
    seive(p[RE]);  // recurse to prime
    close(p[RE]);

  } else {
    // parent
    close(p[RE]);
    printf("prime %d\n", res);
    
    int tmp;
    while(read(fd, &tmp, 4) != 0) {
      if(tmp % res != 0) {
        write(p[WR], &tmp, 4);
      }
    }

    close(p[WR]);
    wait((int *) 0);
    exit(0);
  }
}


int
main() {
  //use pipe syscall to constuct a pipe
  int p[2];
  pipe(p);

  int pid = fork();

  if(pid == 0) {
    close(p[WR]);
    seive(p[RE]);
    close(p[RE]);

  } else {
    // parent
    close(p[RE]);

    for(int i = 2; i <= 35; ++i) {
      write(p[WR], &i, 4);
    }

    close(p[WR]);
    wait((int *) 0);
  }
  exit(0);
}

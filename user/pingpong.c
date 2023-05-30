#include "kernel/types.h"

#include "user/user.h"
int
main() {
  int pid, status;
  int fds[2];
  
  // use pipe to create a pipe
  pipe(fds);

  // use fork to create a child
  pid = fork();
  
  if(pid == 0) { // child process
    int child_pid = getpid();
    char sig[1];
    read(fds[0], sig, 1);
    fprintf(1, "%d: received ping\n", child_pid);
    exit(0);

  } else {
    int father_pid = getpid();
    // parent should send a byte to the child
    write(fds[1], "a", 1);
    wait(&status);
    fprintf(1, "%d: received pong\n", father_pid);
    exit(0);
  }
}

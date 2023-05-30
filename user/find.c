#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/*
   char *
   fmtname(char *path) {
//static char buf[DIRSIZ + 1];
char *p;

// Find first character after last slash

for(p = path + strlen(path); p >= path && *p != '/'; p--)
;
p ++;

return p;

// Return blank-padded name


if(strlen(p) > DIRSIZ)
return p;

memmove(buf, p, strlen(p));
memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
return buf;

}
*/

void
find(char * path, char * filename) {
  int fd;
  struct dirent de;
  struct stat st;
  char buf[512], *p;

  // get fd
  if((fd = open(path, 0)) < 0) {
    fprintf(2, "error");
    exit(0);
  }

  // use fd get stat
  if(fstat(fd, &st) < 0) {
    fprintf(2, "error");
    close(fd);
    return;
  }

  // traverse fd and check

  while(read(fd, &de, sizeof(de)) == sizeof(de)) {
    if(de.inum == 0) continue;
    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;


    // construct a file path
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p ++ = '/';

    memmove(p, de.name, strlen(de.name));
    p[strlen(de.name)] = 0;

    int tmp_fd;
    if((tmp_fd = open(buf, 0)) < 0) {
      // fprintf(2, "find: cannot open %s\n", buf);
      // return;
      continue;
    }

    // get fd
    if(fstat(tmp_fd, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      close(tmp_fd);
      return;
    }

    close(tmp_fd);
    
    // check the type of the file
    switch(st.type) {
      case T_DEVICE:
      case T_FILE:
        if(strcmp(filename, de.name) == 0) {
          fprintf(1, "%s\n", buf);
        }
        break;
      case T_DIR:
        find(buf, filename);
    }
  }
  close(fd);
  return;
}


int
main(int argc, char *argv[]) {
  if(argc < 2) {
    fprintf(2, "at least two args!\n");
    exit(0);
  } else {
    find(argv[1], argv[2]);
  }

  exit(0);
}

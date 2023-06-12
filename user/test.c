#include<kernel/types.h>
#include<user/user.h>

int
main() {
  unsigned int i = 0x00646c72;
  printf("H%x Wo%s\n", 57616, &i);
  
  printf("x=%d y=%d", 3);
  return 0;
}

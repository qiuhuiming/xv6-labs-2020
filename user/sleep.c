#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
  if (argc != 2) {
    fprintf(2, "usage: sleep ticks\n");
    exit(1);
  }
  int t = atoi(argv[1]);
  if (t == 0) {
    printf("invalid argument: %s\n", argv[1]);
    exit(1);
  }
  sleep(t * 10);
  exit(0);
}

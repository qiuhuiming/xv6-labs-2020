#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
  int p2c[2];
  int c2p[2];
  int ppid = getpid();
  int cpid;
  char buf[1];
  int status;

  pipe(p2c);
  pipe(c2p);
  cpid = fork();
  if (cpid == 0)
  {
    cpid = getpid();
    close(p2c[1]);
    close(c2p[0]);
    while (read(p2c[0], buf, 1) > 0) {
      ;
    }
    printf("%d: received ping\n", cpid);
    write(c2p[1], "b", 1);
    close(c2p[1]);
    close(p2c[0]);
    exit(0);
  }
  close(p2c[0]);
  close(c2p[1]);
  write(p2c[1], "a", 1);
  close(p2c[1]);
  while (read(c2p[0], buf, 1) > 0) {
    ;
  }
  printf("%d: received pong\n", ppid);
  wait(&status);
  exit(0);
  return 0;
}


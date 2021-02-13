#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define R 0
#define W 1

void primes(int fd[2])
{
  close(fd[W]);
  int p;
  int n;

  // 从fd中读数据，第一个数如果非零，一定是质数
  read(fd[R], &p, 4); 
  if (p == 0) {
    close(fd[R]);
    return;
  }
  printf("prime %d\n", p);

  // 创建下一个管道和子进程
  int nfd[2];
  pipe(nfd);
  if (fork() == 0)
  {
    close(fd[R]);
    primes(nfd);
    exit(0);
  }
  else
  {
    while (read(fd[R], &n, 4) > 0)
    {
      if (n % p != 0)
      {
        write(nfd[W], &n, 4);
      }
    }
    close(fd[R]);
    close(nfd[W]);
    wait(0);
  }
}

int main(int argc, char const *argv[])
{
  int fd[2];
  pipe(fd);

  if (fork() == 0)
  {
    primes(fd);
  }
  else
  {
    close(fd[R]);
    for (int i = 2; i <= 35; ++i)
    {
      write(fd[W], &i, 4);
    }
    close(fd[W]);
    wait(0);
  }

  exit(0);
  return 0;
}

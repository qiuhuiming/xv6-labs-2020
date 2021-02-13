#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage: xargs commmand\n");
    exit(0);
  }
  int state = 1;
  char c;
  int myargc = 0;
  char *myargv[MAXARG];
  char line[1000];
  while (myargc < argc - 1) {
    myargv[myargc] = argv[myargc + 1];
    myargc++;
  }

  while (state) {
    char *p = line;
    char *word = line;
    while (1) {
      if (read(0, &c, 1) == 0) {
        exit(0);
      }
      if (c == ' ' || c == '\n') {
        *p = '\0';
        myargv[myargc++] = word;
        p++;
        word = p;
        if (c == '\n') {
          break;
        }      
      } else {
        *p = c;
        p++;
      }
    }
    myargv[myargc++] = 0;
    if (fork() == 0) {
      exec(myargv[0], myargv);
    } else {
      wait(0);
      myargc = argc - 1;
    }
  }
  exit(0);
}

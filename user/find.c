#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/**
 * Write a simple version of the UNIX find program: find all the files in a directory tree with a specific name. Your solution should be in the file user/find.c.

Some hints:

1. Look at user/ls.c to see how to read directories.
2. Use recursion to allow find to descend into sub-directories.
3. Don't recurse into "." and "..".
4. Changes to the file system persist across runs of qemu; to get a clean file system run make clean and then make qemu.
5. You'll need to use C strings. Have a look at K&R (the C book), for example Section 5.5.
6. Note that == does not compare strings like in Python. Use strcmp() instead.
7. Add the program to UPROGS in Makefile.
Your solution is correct if produces the following output (when the file system contains the files b and a/b):

    $ make qemu
    ...
    init: starting sh
    $ echo > b
    $ mkdir a
    $ echo > a/b
    $ find . b
    ./b
    ./a/b
    $ 
*/

/**
 * @brief find all the files in a directory tree with a specific name. 
 * 
 * @param path 
 * @return  1 - find successfully
 *          0 - else
 */
int find(const char *dirname, char const *filename)
{
  struct stat dir_st;
  struct stat file_st;
  struct dirent de;
  int dir_fd;
  int file_fd;
  char buffer[512];
  int dirname_len = strlen(dirname);
  char *p;
  strcpy(buffer, dirname);

  if ((dir_fd = open(dirname, 0)) < 0)
  {
    printf("find: cannot open %s\n", dirname);
    return 0;
  }
  if (fstat(dir_fd, &dir_st) < 0)
  {
    printf("find: cannot stat %s\n", dirname);
    close(dir_fd);
    return 0;
  }
  if (dir_st.type == T_FILE)
  {
    printf("find: %s is a file\n", dirname);
    close(dir_fd);
    return 0;
  }

  while (read(dir_fd, &de, sizeof(de)) == sizeof(de))
  {
    if (!de.name || !de.inum)
    {
      continue;
    }
    
    if ((file_fd = open(de.name, 0)) < 0) {
      printf("find: open %s error\n", de.name);
    }
    fstat(file_fd, &file_st);

    if (file_st.type == T_FILE)
    {
      if (!strcmp(filename, de.name)) {
        printf("%s/%s\n", dirname, filename);
      }
    }
    else if (file_st.type == T_DIR)
    {
      if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) {
        continue;
      }
      p = buffer + dirname_len;
      *p = '/';
      p++;
      strcpy(p, de.name);
      find(buffer, filename);
    }

    close(file_fd);
  }
  close(dir_fd);
  return 0;
}

int main(int argc, char const *argv[])
{
  if (argc < 2)
  {
    printf("usage: find <dirname> <filename>\n");
    exit(0);
  }
  find(argv[1], argv[2]);
  exit(0);
  return 0;
}

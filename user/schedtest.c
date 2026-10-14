#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int gate[2];
  int priorities[2] = {10, 30};
  int i, pid;
  char start;

  if (pipe(gate) < 0) {
    fprintf(2, "schedtest: pipe failed\n");
    exit(1);
  }

  for (i = 0; i < 2; i++) {
    setpriority(priorities[i]);
    pid = fork();

    if (pid < 0) {
      fprintf(2, "schedtest: fork failed\n");
      exit(1);
    }

    if (pid == 0) {
      close(gate[1]);

      // Wait until the parent releases both children.
      if (read(gate[0], &start, 1) != 1)
        exit(1);

      close(gate[0]);
      printf("Running child with priority %d\n", getpriority());
      exit(0);
    }
  }

  close(gate[0]);
  setpriority(49);

  printf("Releasing children with priorities 10 and 30\n");
  write(gate[1], "xx", 2);
  close(gate[1]);

  wait(0);
  wait(0);
  exit(0);
}


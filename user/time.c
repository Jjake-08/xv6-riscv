#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "usage: time command [args...]\n");
    exit(1);
  }

  int start = uptime();
  int pid = fork();

  if (pid < 0) {
    fprintf(2, "time: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    exec(argv[1], &argv[1]);
    fprintf(2, "time: exec %s failed\n", argv[1]);
    exit(1);
  }

  int status;
  struct rusage usage;

  // Wait for the child and retrieve its CPU tick count.
  if (wait2(&status, &usage) < 0) {
    fprintf(2, "time: wait2 failed\n");
    exit(1);
  }

  int elapsed = uptime() - start;

  // Use integer arithmetic and avoid division by zero.
  int percent = 0;
  if (elapsed > 0)
    percent = (int)(((uint64)usage.cputime * 100) / elapsed);

  printf("Elapsed time: %d ticks\n", elapsed);
  printf("CPU time: %d ticks\n", (int)usage.cputime);
  printf("CPU usage: %d%%\n", percent);

  exit(status);
}

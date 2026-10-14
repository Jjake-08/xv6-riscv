#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int pid;
  char *args[] = {"ps", 0};

  printf("Initial priority: %d\n", getpriority());

  printf("Set 49 returned: %d\n", setpriority(49));
  printf("Current priority: %d\n", getpriority());

  printf("Set -1 returned: %d\n", setpriority(-1));
  printf("After invalid value: %d\n", getpriority());

  printf("Set 50 returned: %d\n", setpriority(50));
  printf("After invalid value: %d\n", getpriority());

  printf("Set 10 returned: %d\n", setpriority(10));

  pid = fork();
  if (pid < 0) {
    fprintf(2, "prioritytest: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    printf("Child inherited priority: %d\n", getpriority());
    exec("ps", args);
    fprintf(2, "prioritytest: exec failed\n");
    exit(1);
  }

  wait(0);
  exit(0);
}

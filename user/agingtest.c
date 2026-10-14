#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int gate[2], results[2];
  int priorities[2] = {10, 12};
  int i, pid;
  uint start, response;
  uint data[3];
  uint total_response = 0;
  uint total_turnaround = 0;

  if (pipe(gate) < 0 || pipe(results) < 0) {
    fprintf(2, "agingtest: pipe failed\n");
    exit(1);
  }

  for (i = 0; i < 2; i++) {
    if (setpriority(priorities[i]) < 0)
      exit(1);

    pid = fork();
    if (pid < 0) {
      fprintf(2, "agingtest: fork failed\n");
      exit(1);
    }

    if (pid == 0) {
      volatile uint work = 0;
      uint j;
      uint loops = i == 0 ? 5000000 : 100000000;

      close(gate[1]);
      close(results[0]);

      // Both children receive the same starting timestamp.
      if (read(gate[0], &start, sizeof(start)) != sizeof(start))
        exit(1);
      close(gate[0]);

      response = uptime() - start;

      // The lower-priority child has a shorter job.
      for (j = 0; j < loops; j++)
        work += j;

      data[0] = getpriority();
      data[1] = response;
      data[2] = uptime() - start;

      if (write(results[1], data, sizeof(data)) != sizeof(data))
        exit(1);
      close(results[1]);
      exit(0);
    }
  }

  close(gate[0]);
  close(results[1]);
  if (setpriority(49) < 0)
    exit(1);

  start = uptime();
  for (i = 0; i < 2; i++) {
    if (write(gate[1], &start, sizeof(start)) != sizeof(start))
      exit(1);
  }
  close(gate[1]);

  printf("priority\tresponse\tturnaround (ticks)\n");
  for (i = 0; i < 2; i++) {
    if (read(results[0], data, sizeof(data)) != sizeof(data))
      exit(1);

    printf("%u\t\t%u\t\t%u\n", data[0], data[1], data[2]);
    total_response += data[1];
    total_turnaround += data[2];
  }

  close(results[0]);
  wait(0);
  wait(0);

  printf("Average response: %u.%u ticks\n",
         total_response / 2, (total_response % 2) * 5);
  printf("Average turnaround: %u.%u ticks\n",
         total_turnaround / 2, (total_turnaround % 2) * 5);
  exit(0);
}

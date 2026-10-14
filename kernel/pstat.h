#ifndef XV6_PSTAT_H
#define XV6_PSTAT_H

struct rusage {
  uint cputime; // CPU ticks used by the child process
};
enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct pstat {
  int pid;               // Process ID
  int priority;          // Base priority from 0 to 49
  uint readytime;        // Tick when the process became runnable
  enum procstate state;  // Current process state
  uint64 size;           // Memory size in bytes
  int ppid;              // Parent process ID
  char name[16];         // Process name
};

#endif

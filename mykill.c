#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "sig_util.h"


int main(int argc, char **argv)
{
  /* argc should be 1, and argv should point to one value -- the PID we are trying to kill */
  int target_pid = atoi(argv[1]);

  kill(target_pid, SIGUSR1);
  return 0;
}

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const int MAX = 13;

static void doFib (int n, int doPrint);

/*
 * unix_error - unix-style error routine.
 */
inline static void unix_error (char *msg)
{
  fprintf (stdout, "%s: %s\n", msg, strerror (errno));
  exit (1);
}

int main (int argc, char **argv)
{
  int arg;
  int print = 1;

  if (argc != 2)
    {
      fprintf (stderr, "Usage: fib <num>\n");
      exit (-1);
    }

  arg = atoi (argv[1]);
  if (arg < 0 || arg > MAX)
    {
      fprintf (stderr, "number must be between 0 and %d\n", MAX);
      exit (-1);
    }

  doFib (arg, print);

  return 0;
}

/*
 * Recursively compute the specified number. If print is
 * true, print it. Otherwise, provide it to my parent process.
 *
 * NOTE: The solution must be recursive and it must fork
 * a new child for each call. Each process should call
 * doFib() exactly once.
 */
static void doFib (int n, int doPrint) {
  // Base cases: either n is 0 or 1, so we can't make a recursive call - no need to fork 
  if (n == 0 || n == 1) {
    // return 0 -- print if this is parent (doPrint == 1) else exit
    if (doPrint == 1) {
      fprintf (stdout, "%d\n", n);
    } else {
      exit(n);
    }
  } else { // NOT BC
    int child_pid = fork();
    if (child_pid < 0) { // Account for failure with forking the process - TODO: what causes fork failure?
      fprintf(stderr, "Process forking error.");
    } else if (child_pid == 0) { 
      doFib(n-1, 0);
      exit(0);
    } else { 
      int status; 
      waitpid(child_pid, &status, 0);

      if (status == -1) { // failure of waiting for (n-1) call
        fprintf(stderr, "Process wait error.");
      }

      int final_fib_value = WEXITSTATUS(status);

      int child_pid2 = fork();
      if (child_pid2 < 0) {
        fprintf(stderr, "Process forking error.");
      } else if (child_pid2 == 0) {
        doFib(n-2, 0);
        exit(0);
      } else {
        waitpid(child_pid2, &status, 0);
        if (status == -1) { // failure of waiting for (n-1) call
          fprintf(stderr, "Process wait error.");
        }

        final_fib_value += WEXITSTATUS(status);
        if (doPrint == 1) {
          fprintf(stdout, "%d\n", final_fib_value);
        } else {
          exit(final_fib_value);
        }
      }
    }
  }
  
}

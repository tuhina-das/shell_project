#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "util.h"
#include "sig_util.h"

/* 
 * Functions to handle incoming INT signal, utilized in sig_util.c
 */
void handle_int() {
  /* fprintf is unsafe for signal handling. Check Google Doc for why. */
  ssize_t bytes;
  const int STDOUT = 1;
  bytes = write(STDOUT, "Nice try.\n", 10);
  if(bytes != 10)
    exit(-999);
  return;
}

/* 
 * Functions to handle incoming SIGUSR1 signal, utilized in sig_util.c
 */
void handle_kill() {
  ssize_t bytes;
  const int STDOUT = 1;
  bytes = write(STDOUT, "exiting\n", 8);
  if(bytes != 8)
    exit(-999);
  exit(0);
}

/*
 * First, print out the process ID of this process.
 *
 * Then, set up the signal handler so that ^C causes
 * the program to print "Nice try.\n" and continue looping.
 *
 * Finally, loop forever, printing "Still here\n" once every
 * three seconds.
 */
int main(int argc, char **argv)
{
  /* Print PID -- helpful to users trying to kill this process */
  pid_t pid = getpid();
  fprintf(stdout, "CURRENT PROCESS ID IS: %d\n", pid);

  /* timespec pointer for 3-second sleep interval */
  struct timespec* time = malloc(sizeof(struct timespec));
  time->tv_nsec = 0;
  time->tv_sec = 3;

  /* Never-ending loop to print message while not killed + handle interrupt signals. */
  while(1) {
    signal_action(SIGINT, (handler_t*) (handle_int));
    signal_action(SIGUSR1, (handler_t*) (handle_kill));
    nanosleep(time, NULL);
    fprintf(stdout, "Still here\n");
  } 

  return 0;
}

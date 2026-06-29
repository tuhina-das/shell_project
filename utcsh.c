/*
  utcsh - The UTCS Shell

  Tuhina, tuhina
*/

/* Read the additional functions from util.h. They may be beneficial to you
in the future */
#include "util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/* Global variables */
/* The array for holding shell paths. Can be edited by the functions in util.c*/
char shell_paths[MAX_ENTRIES_IN_SHELLPATH][MAX_CHARS_PER_CMDLINE];
static char prompt[] = "utcsh> "; /* Command line prompt */
static char *default_shell_path[2] = {"/bin", NULL};

/* The maximum number of arguments that a Command can hold */
#define MAX_ARGS 64

/* The current parsed Command's arguments, and number of args */
// Would this get design points off?
char* current_command_args[MAX_ARGS] = { NULL }; // nice sexy global array to help track tokens
int SHELL_ARGC = 0;

/* Variable tracking shell mode */
bool IN_SHELL_MODE = false;

/* End Global Variables */

/* Convenience struct for describing a command. Modify this struct as you see
 * fit--add extra members to help you write your code. */
struct Command
{
  char **args;      /* Argument array for the command */
  char *outputFile; /* Redirect target for file (NULL means no redirect) */
};

/* Here are the functions we recommend you implement */

void tokenize_command_line (char *cmdline);
struct Command parse_command ();
void eval (struct Command cmd);
int try_exec_builtin (struct Command *cmd);
void exec_external_cmd (struct Command cmd);

/* Here are the functions used for testing */
void print_command(struct Command cmd);

/* Main REPL: read, evaluate, and print. This function should remain relatively
   short: if it grows beyond 60 lines, you're doing too much in main() and
   should try to move some of that work into other functions. */
int main (int argc, char **argv)
{
  /* CASE NOT SHELL MODE */
  if (argc == 1) { 
    /* Loop while exit/error is not occurring */
    while (1) {
      /* RESET + READ COMMAND */
      SHELL_ARGC = 0;
      memset(current_command_args, NULL, MAX_ARGS); 
      printf ("%s", prompt);
      
      char* string_buffer = NULL;
      size_t buffer_size = 0;
      size_t characters_read = 0;

      /* Question: Should we use setrlimit()? */
      characters_read = getline(&string_buffer, &buffer_size, stdin);

      // Overwrite '\n' with null terminator 
      string_buffer[strcspn(string_buffer, "\n")] = '\0';
      tokenize_command_line(string_buffer);
      struct Command current_command = parse_command();

      /* EVAL COMMAND */
      eval(current_command);

      /* Print (optional) */
      // Depends on the command. If the command requires it, then do so. We're likely matching functions/function ptrs here.
    }

  } else if (argc == 2) { // CASE SHELL MODE
    /* INITIALIZE LOOP START */
    memset(current_command_args, NULL, MAX_ARGS); 
    char* string_buffer = NULL;
    size_t buffer_size = 0;
    size_t characters_read = 0;

    FILE* file_stream = fopen(argv[1], "r");
    if (file_stream == NULL) {
      // Stream failed, throw an error 
      printf("A fuckin error\n");
    } 

    // characters_read = getline(&string_buffer, &buffer_size, file_stream);
    while (characters_read = getline(&string_buffer, &buffer_size, file_stream) != -1) {

      string_buffer[strcspn(string_buffer, "\n")] = '\0';
      tokenize_command_line(string_buffer);
      struct Command current_command = parse_command();

      /* Evaluate */
      eval(current_command);

      /* RESET */
      SHELL_ARGC = 0;
      memset(current_command_args, NULL, MAX_ARGS); 
      char* string_buffer = NULL;
      size_t buffer_size = 0;
      size_t characters_read = 0;
    }

  } else { // CASE ERROR
    print_error(-1);
    exit(-1);
  }

  // while (1)
  //   {
  //     // Before anything else, reinitialize global array to all nulls?
  //     SHELL_ARGC = 0;
  //     memset(current_command_args, NULL, MAX_ARGS); 

  //     printf ("%s", prompt);
      
  //     /* Read */
  //     // There are probably two main things the user will have: a command (string, first part) and an array of args(the rest -- can include flags etc)
  //     // The thing is, we can't directly look at this string and define it as a pointer. So we'll take the string and split it.
  //     char* string_buffer = NULL;
  //     size_t buffer_size = 0;
  //     size_t characters_read = 0;

  //     /* Question: Should we use setrlimit()? */
  //     if (!IN_SHELL_MODE) {
  //       characters_read = getline(&string_buffer, &buffer_size, stdin);
  //       // Looks at the number of letters occurring before '\n', and overwrites the target with the null terminator 
  //       string_buffer[strcspn(string_buffer, "\n")] = '\0';
  //       tokenize_command_line(string_buffer);
  //       struct Command current_command = parse_command();
  //       /* Evaluate */
  //       eval(current_command);
  //     } else {
  //       FILE* file_stream = fopen(argv[1], "r");
  //       if (file_stream == NULL) {
  //         /* Stream failed, throw an error */
  //         printf("A fuckin error\n");
  //       } 

  //       characters_read = getline(&string_buffer, &buffer_size, file_stream);
  //       while (characters_read != -1) {
  //         string_buffer[strcspn(string_buffer, "\n")] = '\0';
  //         tokenize_command_line(string_buffer);
  //         struct Command current_command = parse_command();
  //         /* Evaluate */
  //         eval(current_command);
  //       }
  //     }
  //     /* Print (optional) */
  //     // Depends on the command. If the command requires it, then do so. We're likely matching functions/function ptrs here.
  //   }
  return 0;
}

/* NOTE: In the skeleton code, all function bodies below this line are dummy
implementations made to avoid warnings. You should delete them and replace them
with your own implementation. */

/** Turn a command line into tokens with strtok
 *
 * This function turns a command line into an array of arguments, making it
 * much easier to process. First, you should figure out how many arguments you
 * have, then allocate a char** of sufficient size and fill it using strtok()
 */
void tokenize_command_line (char *cmdline)
{
  /* Question: Does sufficient mean extra space would result in a deduction of points? */

  char* token = strtok(cmdline, " ");

  while (token != NULL) {
    current_command_args[SHELL_ARGC++] = token;
    token = strtok(NULL, " ");
  }
  current_command_args[SHELL_ARGC] = NULL;

  // printf("Number of args is %d\n", argc); //debug
}

/** Turn tokens into a command.
 *
 * The `struct Command` represents a command to execute. This is the preferred
 * format for storing information about a command, though you are free to change
 * it. This function takes a sequence of tokens and turns them into a struct
 * Command.
 */
struct Command parse_command ()
{
  struct Command command = {.args = current_command_args, .outputFile = NULL};
  bool output_symbol_found_once = false;

  // How do we find an output file? --> Go through tokens and look for ">" -- token following that is output file
  for (int arg_i = 0; arg_i < SHELL_ARGC; arg_i++) {
    // Case carrot (>) already found
    if (output_symbol_found_once) {
      if (current_command_args[arg_i] == NULL || current_command_args[arg_i] == '>') {
        // Error case.
      } else {
        command.outputFile = current_command_args[arg_i];
      }
    } else {
      if (current_command_args[arg_i][0] == '>') {
        output_symbol_found_once = true;
      }
    }
  }

  return command;
}


/** Evaluate a single command
 *
 * Both built-ins and external commands can be passed to this function--it
 * should work out what the correct type is and take the appropriate action.
 */
void eval (struct Command cmd)
{
  // Commands: exit, cd and path
  /* First command of interest: exit */
  char* keyword = cmd.args[0];
  if (strcmp(keyword, "exit") == 0) {
    if (SHELL_ARGC > 1) {
      print_error(0);
      exit(-1);
    }
    exit(0);
  } else if (strcmp(keyword, "cd") == 0) {
    if (SHELL_ARGC > 2) {
      print_error(1);
      exit(-1);
    }

    printf("Changing directory...\n");
    int success = chdir(cmd.args[1]);
    if (success != 0) {
      print_error(2);
      exit(-1);
    }
  } else if (strcmp(keyword, "path") == 0) {
    // Note: path will never error out
    // char cwd[MAX_CHARS_PER_CMDLINE];
    // printf("Current directory: %s\n", getcwd(cwd, sizeof(cwd)));
  } else {
    /* Assume it is an external command */
    exec_external_cmd(cmd);
  }

  return;
}

/** Execute built-in commands
 *
 * If the command is a built-in command, execute it and return 1 if appropriate
 * If the command is not a built-in command, do nothing and return 0
 */
int try_exec_builtin (struct Command *cmd)
{
  return 0;
}

/** Execute an external command
 *
 * Execute an external command by fork-and-exec. Should also take care of
 * output redirection, if any is requested
 */
void exec_external_cmd (struct Command cmd)
{
  char* keyword = cmd.args[0];
  pid_t pid = fork();
    if (pid == 0) {
      /* If child, use execv() to 'turn into' a different process */
      int result = execv(keyword, cmd.args);

      // If the exec fails, we will reach the below code. 
      // At this point, we've reached an error and need to handle it.
      //TODO: why doesn't this exit properly?
      print_error(-1);
      exit(-1);
  } else {
      /* Otherwise, wait for the child to finish */
      waitpid(pid, NULL, 0);
  }
  return;
}

void print_error(int error_type) {
  // TODO -- check if this is a valid setup for errors (style-wise) bc its lowk cursed
  char* emsg; 

  if (error_type == 0) {
    emsg = "An error has occurred: exit call may not have arguments\n";
  } else if (error_type == 1) {
    emsg = "An error has occurred: wrong number of arguments for cd\n";
  } else if (error_type == 2){
    emsg = "An error has occurred: chdir() failed\n";
  } else {
    /* Unrecognized and therefore external command - fork and exec */
    emsg = "An error has occurred: unspecified command\n"; //TODO delete
  }

  int nbytes_written = write(STDERR_FILENO, emsg, strlen(emsg));
  if(nbytes_written != (int)strlen(emsg)){
    exit(2);  // Shouldn't really happen -- if it does, error is unrecoverable
  }
}

/* NOTE: The below functions are for debugging during development. 
They can be removed once this project is completed, though it might be helpful
to keep them. */

/** 
 * Print a command to see its contents.
 */
void print_command(struct Command cmd) {
  printf("--------- Current command info for debugging ---------\n");
  printf("Number of args: %i\n", SHELL_ARGC);
  printf("Command args: ");
  for (int i = 0; i < SHELL_ARGC; i++) {
    printf("%s ", cmd.args[i]);
  }
  printf("\nOutput file, if any: %s\n", cmd.outputFile);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/util.h"
#include "pipe/pipe.h"
#include "exec/exec.h"

int main(int argc, char *argv[])
{
  char *command;
  char **args;

  printf("\nsash - The Suspicious Shell.\n");

  while (1)
  {

    print_prompt();
    command = readline();

    if (strcmp(command, "exit") == 0)
    {
      free(command);
      command = NULL;
      break;
    }
    args = tokenize(command);

    int pipeindx = find_pipe(args);
    if (pipeindx >= 0)
    {
      exec_pipe(args, pipeindx);
      clean(command, args);
      continue;
    }
    exec(args[0], args);

    clean(command, args);
  }

  return 0;
}

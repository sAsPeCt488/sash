#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "sash.h"

#define MAX_ARGS 20



int main(int argc, char* argv[])
{
  char* command;
  char** args;

  printf("\nsash - The Suspicious Shell.\n");

  while(1)
  {

    command = sash_readline();

    if(strcmp(command, "exit") == 0)
    {
      free(command);
      command = NULL;
      break;
    }
    args = sash_parseline(command);

    int pipeindx = sash_pipecheck(args);
    if(pipeindx >= 0)
    {
      sash_pipexecutecommand(args, pipeindx);
      free(command);
      command = NULL;
      free(args);
      args = NULL;
      continue;
    }
    sash_executecommand(args[0], args);

    free(command);
    command = NULL;
    free(args);
    args = NULL;
  }

  return 0;
}


int sash_printprompt()
{
  uid_t euid = geteuid();
  char* pwd;
  char* env = "LOGNAME";
  char* username = getenv(env);
  if (username == NULL)
  {
      username = "sash";
  }
  pwd = (char*)malloc((PATH_MAX + 1)*sizeof(char));
  if (pwd == NULL)
  {
    printf("[*] sash: Memory Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  getcwd(pwd, PATH_MAX + 1);
  if (pwd == NULL)
  {
    printf("[*] sash: Failed to retrive current working directory.\n");

  }
  if (euid != 0)
  {
    printf("\n\x1b[34m(\x1b[0m%s\x1b[34m)\x1b[0m-\x1b[34m[\x1b[0m%s\x1b[34m]\x1b[0m", username, pwd);
    printf("\n\x1b[34m└─\x1b[0m$ ");
  }
  else
  {
    printf("\n\x1b[34m┌──(\x1b[0m%s\x1b[34m)\x1b[0m-\x1b[34m[\x1b[0m%s\x1b[34m]\x1b[0m ", username, pwd);
    printf("\n\x1b[34m└─\x1b[0m\x1b[31m#\x1b[0m ");
  }
  free(pwd);
  pwd = NULL;
  return 0;
}

char* sash_readline()
{
  char* buff;

  buff = (char*)malloc(MAX_ARGS * 2);

  if (buff == NULL)
  {
    printf("[*] sash: Memory Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  sash_printprompt();

  fgets(buff, (MAX_ARGS*2)-1, stdin);
  buff[strcspn(buff, "\n")] = 0;
  return buff;

}


char** sash_parseline(char* line)
{
  char* token;
  char** arguments;
  int argc;
  arguments = (char**)malloc(sizeof(char*)*MAX_ARGS + 1);
  if(arguments == NULL)
  {
    printf("[*] sash: Memory Allocation Error.\n");
    exit(EXIT_FAILURE);
  }


  token = strtok(line, " ");
  arguments[0] = token;
  for(int i = 1; i < MAX_ARGS; ++i)
  {
    token = strtok(NULL, " ");
    if(token == NULL)
    {
      argc = i;
      break;

    }
    arguments[i] = token;
  }
  arguments[argc] = NULL;
  return arguments;
}

redirect_t* sash_redirectcheck(char** args)
{
    redirect_t* redirect;
    redirect = malloc(sizeof(redirect_t));
    if(redirect == NULL)
    {
      printf("[*] sash: Memory Allocation Error.\n");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_ARGS; ++i)
    {
      if(args[i] == NULL)
      {
        break;
      }
      if(strcmp(args[i], ">") == 0)
      {
        redirect->type = 0;
        redirect->index = i;
        return redirect;
      }
      if(strcmp(args[i], "<") == 0)
      {
        redirect->type = 1;
        redirect->index = i;
        return redirect;
      }

    }
    redirect->type = -1;
    redirect->index = -1;
    return redirect;
}

int sash_pipecheck(char** args)
{
  for(int i = 0; i < MAX_ARGS; ++i)
  {
    if(args[i] == NULL)
    {
      break;
    }
    if(strcmp(args[i], "|") == 0)
    {
      return i;
    }
  }
  return -1;
}

int sash_redirect(redirect_t* rdata, char** argv)
{
  if(rdata->type == 0)
  {
    FILE* fd = fopen(argv[rdata->index + 1], "w");
    if (fd == NULL){
      printf("[!] sash: Unable to create file!");
      exit(EXIT_FAILURE);
    }
    argv[rdata->index] = NULL;
    argv[rdata->index + 1] = NULL;
    fflush(stdout);
    dup2(fileno(fd), fileno(stdout));
  }
  if(rdata->type == 1)
  {
    FILE* fd = fopen(argv[rdata->index + 1], "r");
    if (fd == NULL){
      printf("[!] sash: Unable to read file!");
      exit(EXIT_FAILURE);
    }
    argv[rdata->index] = NULL;
    argv[rdata->index + 1] = NULL;
    fflush(stdin);
    dup2(fileno(fd), fileno(stdin));
    fclose(fd);
  }
  return 0;
}

int sash_executecommand(char* command, char** argv){

  if (argv[0] == NULL || command == NULL)
  {
    // printf("[*] sash: No command issued.\n");
    return -1;
  }
  if (strcmp(command,"cd") == 0)
  {
    if (chdir(argv[1]) != 0)
    {
      perror("[*] sash");
    }
    return 0;
  }


  redirect_t* rdata;

  rdata = sash_redirectcheck(argv);

  pid_t pid = fork();

  switch(pid){
    case -1:
      printf("[*] sash: Unable to spawn child process.\n");
    case 0:
      if (rdata->index != -1)
      {
        sash_redirect(rdata, argv);
      }

      if (execvp(command, argv) < 0)
      {
        printf("[*] sash: command not found.\n");
        _Exit(EXIT_FAILURE); // if execvp returns, kill the (child) process
      }
    default:
      waitpid(pid, NULL, 0);
      free(rdata);
      rdata = NULL;
      return 0;
  }
}

int sash_pipexecutecommand(char** argv, int pipeindx)
{
  char** argv1;
  char** argv2;
  argv1 = sash_pipeparse1(argv, pipeindx);
  argv2 = sash_pipeparse2(argv, pipeindx);

  int pfd[2];
  pid_t pid;
  pid_t pid2;

  pid = fork();
  if(pipe(pfd) < 0)
  {
    printf("[*] sash: Unable to create the pipe.\n");
    exit(EXIT_FAILURE);
  }
  switch(pid){
    case -1:
      printf("[*] sash: Unable to spawn child process.\n");
    case 0:
      pid2 = fork();
      switch (pid2) {
        case -1:
          printf("[*] sash: Unable to spawn child process.\n");
        case 0:
          close(pfd[0]);
          dup2(pfd[1], STDOUT_FILENO);
          close(pfd[1]);

          if (execvp(argv1[0], argv1) < 0)
          {
            printf("[*] sash: command not found.\n");
            _Exit(EXIT_FAILURE); // if execvp returns, kill the (child) process
          }
        default:
          close(pfd[1]);
          dup2(pfd[0], STDIN_FILENO);
          close(pfd[0]);
          waitpid(pid2, NULL, 0);

          if (execvp(argv2[0], argv2) < 0)
          {
            printf("[*] sash: command not found.\n");
            _Exit(EXIT_FAILURE); // if execvp returns, kill the (child) process
          }
      }

    default:
      wait(NULL);
      wait(NULL);
      free(argv1);
      free(argv2);
      argv1 = NULL;
      argv2 = NULL;
      return 0;
  }

}

char** sash_pipeparse1(char** argv, int pipeindx)
{
  char** argv1;
  argv1 = (char**)malloc(sizeof(char*)*pipeindx + 1);
  if(argv1 == NULL)
  {
    printf("[*] sash: Memory Allocation Error.\n");
    exit(EXIT_FAILURE);
  }
  for(int i = 0; i < pipeindx; ++i)
  {
    argv1[i] = argv[i];
  }
  argv1[pipeindx] = NULL;
  return argv1;
}

char** sash_pipeparse2(char** argv, int pipeindx)
{
  char** argv2;
  int argc = 0;
  for(int i = pipeindx + 1; i < MAX_ARGS; ++i)
  {
    if(argv[i] == NULL)
    {
      break;
    }
    argc++;
  }

  argv2 = (char**)malloc(sizeof(char*)*argc + 1);
  if(argv2 == NULL)
  {
    printf("[*] sash: Memory Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  int j = pipeindx + 1;

  for(int i = 0; i < argc; ++i)
  {
    argv2[i] = argv[j];
    ++j;
  }
  argv2[argc] = NULL;
  return argv2;
}

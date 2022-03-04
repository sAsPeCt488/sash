
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>

#include "util.h"

int print_prompt()
{
	uid_t euid = geteuid();
	char *pwd;
	char *username = getenv("LOGNAME");
	if (username == NULL)
	{
		username = "sash";
	}
	pwd = (char *)malloc((PATH_MAX + 1) * sizeof(char));
	if (pwd == NULL)
	{
		error("Memory Allocation Error.\n");
	}

	getcwd(pwd, PATH_MAX + 1);
	if (pwd == NULL)
	{
		pinfo("Failed to retrive current working directory.\n");
	}
	if (euid != 0)
	{
		printf("\n(%s) - [%s] $ ", username, pwd);
	}
	else
	{
		printf("\n(%s) - [%s] # ", username, pwd);
	}
	free(pwd);
	pwd = NULL;
	return 0;
}

char *readline()
{
	char *buff;

	buff = (char *)malloc(MAX_ARGS * 2);

	if (buff == NULL)
	{
		error("Memory Allocation Error.\n");
	}

	fgets(buff, (MAX_ARGS * 2) - 1, stdin);
	buff[strcspn(buff, "\n")] = 0;
	return buff;
}

void error(const char *msg)
{
	if (msg != NULL)
	{
		fprintf(stderr, "[!] sash: %s", msg);
	}
	exit(EXIT_FAILURE);
}

void exit_child()
{

	fprintf(stderr, "[*] sash: command not found.\n");
	_Exit(EXIT_FAILURE); // if execvp returns, kill the (child) process
}

void pinfo(const char *msg)
{
	if (msg != NULL)
	{
		fprintf(stdin, "[*] sash: %s", msg);
	}
}

char **tokenize(char *line)
{
	char *token;
	char **arguments;
	int argc;
	arguments = (char **)malloc(sizeof(char *) * MAX_ARGS + 1);
	if (arguments == NULL)
	{
		printf("[*] sash: Memory Allocation Error.\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, " ");
	arguments[0] = token;
	for (int i = 1; i < MAX_ARGS; ++i)
	{
		token = strtok(NULL, " ");
		if (token == NULL)
		{
			argc = i;
			break;
		}
		arguments[i] = token;
	}
	arguments[argc] = NULL;
	return arguments;
}

void clean(char *command, char **args)
{
	free(command);
	command = NULL;
	free(args);
	args = NULL;
}
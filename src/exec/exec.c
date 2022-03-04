
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "util/util.h"
#include "redirect/redirect.h"
#include "pipe/pipe.h"

#include "exec.h"

int exec(char *command, char **argv)
{

	if (argv[0] == NULL || command == NULL)
	{
		return -1;
	}
	if (strcmp(command, "cd") == 0)
	{
		if (chdir(argv[1]) != 0)
		{
			perror("[*] sash");
		}
		return 0;
	}

	redirect_t *rdata;

	rdata = parse_redirect(argv);

	pid_t pid = fork();

	switch (pid)
	{
	case -1:
		pinfo("Unable to spawn child process.\n");
	case 0:
		if (rdata->index != -1)
		{
			redirect(rdata, argv);
		}

		if (execvp(command, argv) < 0)
		{
			exit_child();
		}
	default:
		waitpid(pid, NULL, 0);
		free(rdata);
		rdata = NULL;
		return 0;
	}
}

int exec_pipe(char **argv, int pipeindx)
{
	char **argv1;
	char **argv2;
	argv1 = parse_pipe(argv, pipeindx);
	argv2 = parse_pipe2(argv, pipeindx);

	int pfd[2];
	pid_t pid;
	pid_t pid2;

	pid = fork();
	if (pipe(pfd) < 0)
	{
		error("Unable to create the pipe.\n");
	}
	switch (pid)
	{
	case -1:
		pinfo("Unable to spawn child process.\n");
	case 0:
		pid2 = fork();
		switch (pid2)
		{
		case -1:
			pinfo("Unable to spawn child process.\n");
		case 0:
			close(pfd[0]);
			dup2(pfd[1], STDOUT_FILENO);
			close(pfd[1]);

			if (execvp(argv1[0], argv1) < 0)
			{
				exit_child();
			}
		default:
			close(pfd[1]);
			dup2(pfd[0], STDIN_FILENO);
			close(pfd[0]);
			waitpid(pid2, NULL, 0);

			if (execvp(argv2[0], argv2) < 0)
			{
				exit_child();
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
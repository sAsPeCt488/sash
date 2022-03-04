
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "util/util.h"
#include "redirect.h"

redirect_t *parse_redirect(char **args)
{
	redirect_t *redirect;
	redirect = malloc(sizeof(redirect_t));
	if (redirect == NULL)
	{
		error("Memory Allocation Error.\n");
	}
	for (int i = 0; i < MAX_ARGS; ++i)
	{
		if (args[i] == NULL)
		{
			break;
		}
		if (strcmp(args[i], ">") == 0)
		{
			redirect->type = 0;
			redirect->index = i;
			return redirect;
		}
		if (strcmp(args[i], "<") == 0)
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

int redirect(redirect_t *rdata, char **argv)
{
	if (rdata->type == 0)
	{
		FILE *fd = fopen(argv[rdata->index + 1], "w");
		if (fd == NULL)
		{
			error("Unable to create file!");
		}
		argv[rdata->index] = NULL;
		argv[rdata->index + 1] = NULL;
		fflush(stdout);
		dup2(fileno(fd), fileno(stdout));
	}
	if (rdata->type == 1)
	{
		FILE *fd = fopen(argv[rdata->index + 1], "r");
		if (fd == NULL)
		{
			error("Unable to read file!");
		}
		argv[rdata->index] = NULL;
		argv[rdata->index + 1] = NULL;
		fflush(stdin);
		dup2(fileno(fd), fileno(stdin));
		fclose(fd);
	}
	return 0;
}
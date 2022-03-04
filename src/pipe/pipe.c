
#include <stdlib.h>
#include <string.h>

#include "util/util.h"
#include "pipe.h"

int find_pipe(char **args)
{
	for (int i = 0; i < MAX_ARGS; ++i)
	{
		if (args[i] == NULL)
		{
			break;
		}
		if (strcmp(args[i], "|") == 0)
		{
			return i;
		}
	}
	return -1;
}

char **parse_pipe(char **argv, int pipeindx)
{
	char **argv1;
	argv1 = (char **)malloc(sizeof(char *) * pipeindx + 1);
	if (argv1 == NULL)
	{
		error("Memory Allocation Error.\n");
	}
	for (int i = 0; i < pipeindx; ++i)
	{
		argv1[i] = argv[i];
	}
	argv1[pipeindx] = NULL;
	return argv1;
}

char **parse_pipe2(char **argv, int pipeindx)
{
	char **argv2;
	int argc = 0;
	for (int i = pipeindx + 1; i < MAX_ARGS; ++i)
	{
		if (argv[i] == NULL)
		{
			break;
		}
		argc++;
	}

	argv2 = (char **)malloc(sizeof(char *) * argc + 1);
	if (argv2 == NULL)
	{
		error("Memory Allocation Error.\n");
	}

	int j = pipeindx + 1;

	for (int i = 0; i < argc; ++i)
	{
		argv2[i] = argv[j];
		++j;
	}
	argv2[argc] = NULL;
	return argv2;
}

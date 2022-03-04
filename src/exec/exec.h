#ifndef EXEC_H_
#define EXEC_H_

int exec(char *command, char **argv);
int exec_pipe(char **argv, int pipeindx);

#endif
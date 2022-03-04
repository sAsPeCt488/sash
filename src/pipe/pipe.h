
#ifndef PIPE_H_
#define PIPE_H_

int find_pipe(char **args);
char **parse_pipe(char **argv, int pipeindx);
char **parse_pipe2(char **argv, int pipeindx);

#endif
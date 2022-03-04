
#ifndef UTIL_H_
#define UTIL_H_

#define MAX_ARGS 20

int print_prompt();
char *readline();
void error(const char *msg);
void exit_child();
char **tokenize(char *line);
void clean(char *command, char **args);
void pinfo(const char *msg);

#endif
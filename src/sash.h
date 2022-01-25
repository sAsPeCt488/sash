#ifndef SASH_H_
#define SASH_H_

typedef struct redirect_t
{
  int type;
  int index;

} redirect_t;

char* sash_readline();
char** sash_parseline(char* line);
int sash_executecommand(char* command, char** argv);
int sash_printprompt();
redirect_t* sash_redirectcheck(char** args);
int sash_redirect(redirect_t* rdata, char** argv);
int sash_pipecheck(char** args);
int sash_pipexecutecommand(char** argv, int pipeindx);
char** sash_pipeparse1(char** argv, int pipeindx);
char** sash_pipeparse2(char** argv, int pipeindx);
#endif

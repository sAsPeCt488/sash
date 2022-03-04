
#ifndef REDIRECT_H_
#define REDIRECT_H_

typedef struct redirect_t
{
	int type;
	int index;

} redirect_t;

redirect_t *parse_redirect(char **args);
int redirect(redirect_t *rdata, char **argv);

#endif
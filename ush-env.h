/* ush-env.h */
/* header file for ush-env.c, environment definitions */

#ifndef _USH_ENV_H
#define _USH_ENV_H

#include "ush.h"

/* environment processing functions */
BOOLEAN assign(char **p, char *s);		/* initialize name or value */
BOOLEAN EVexport(char *name);					/* set variable to be exported */
BOOLEAN EVunset(char *name);					/* unset variable */
struct varslot *find(char *name);			/* find symbol table entry */
BOOLEAN EVset(char *name, char *val); /* add name & valude to
						 enviromnemt */
BOOLEAN EVinit();											/* initialize symbol table from
														 environment */
char *EVget(char *name);							/* get value of variable */
void EVprint();												/* printf environment */
BOOLEAN EVcommand(int argc, char *argv[]); /* process environment command */

#endif

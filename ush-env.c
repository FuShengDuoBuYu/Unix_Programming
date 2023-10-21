/* ush-env.c */
/* environment functions for ush */

#include "ush.h"
#include "ush-env.h"

// this data structure is used to store the
// environment assignment information.
struct varslot /* symbol table slot */
{
    char *name;
    char *val;
    BOOLEAN exported;
} sym[MAXVAR];

BOOLEAN assign(char **p, char *s)
{ /* initialize name or value */
    int size;

    size = strlen(s) + 1;
    if (*p == NULL)
    {
        if ((*p = malloc(size)) == NULL){
            printf("assign: malloc failed\n");
            return (FALSE);
        }
    }
    else if ((*p = realloc(*p, size)) == NULL)
        return (FALSE);
    strcpy(*p, s);
    return (TRUE);
}

//////////////////////////////////////////////////////////
// You must implement the invoke function 					//
// do the environment export								       //
//////////////////////////////////////////////////////////
BOOLEAN EVexport(char *name)
{ /* set variable to be exported */
    struct varslot *v = find(name);
    if (v == NULL)
        return (FALSE);
    v->exported = TRUE;
    return (TRUE);
}

struct varslot *find(char *name)
{ /* find symbol table entry */

    int i;
    struct varslot *v;

    v = NULL;
    for (i = 0; i < MAXVAR; i++)
        if (sym[i].name == NULL)
        {
            if (v == NULL){
                v = &sym[i];
                break;
            }
        }
        else if (strcmp(sym[i].name, name) == 0)
        {
            v = &sym[i];
            break;
        }
    return (v);
}

//////////////////////////////////////////////////////////
// You must implement the invoke function 					//
// do the environment set								       //
//////////////////////////////////////////////////////////

BOOLEAN EVset(char *name, char *val)
{ /* add name & valude to enviromnemt */
    struct varslot *v = find(name);
    // set name and value
    if(!assign(&v->name,name) || !assign(&v->val,val)){
        return (FALSE);
    }
    v->exported = FALSE;
    return (TRUE);
}

BOOLEAN EVinit()
{ /* initialize symbol table from
     environment */
    int i, namelen;
    char name[100];

    for (i = 0; i<(MAXVAR-10); i++)
    {
        namelen = strcspn(environ[i], "=");
        strncpy(name, environ[i], namelen);
        name[namelen] = '\0';
        if (!EVset(name, &environ[i][namelen + 1]) || !EVexport(name)){
            return (FALSE);
        }
    }

    // add PATH and HOME
    // get current path
    char *path = getenv("PATH");
    char *home = getenv("HOME");
    if (!EVset("PATH", path) || !EVexport("PATH")){
        return (FALSE);
    }
    if (!EVset("HOME", home) || !EVexport("HOME")){
        return (FALSE);
    }
    return (TRUE);
}

char *EVget(char *name)
{ /* get value of variable */
    struct varslot *v;
    if ((v = find(name)) == NULL || v->name == NULL)
        return (NULL);
    return (v->val);
}

void EVprint()
{ /* printf environment */
    int i;

    for (i = 0; i < MAXVAR; i++)
        if (sym[i].name != NULL)
            printf("%3s %s=%s\n", sym[i].exported ? "[E]" : "",sym[i].name, sym[i].val);
}

//////////////////////////////////////////////////////////
// You must implement the invoke function 					//
// do the environment assignment						       //
//////////////////////////////////////////////////////////
void asg(int argc, char *argv[])
{ /* assignment command */
    
}

//////////////////////////////////////////////////////////
// You must implement the invoke function 					//
// do the environment assignment						       //
//////////////////////////////////////////////////////////
void set(int argc, char *argv[])
{ /* set command */
    int i;
    // if no argument, print the environment
    if (argc == 1)
    {
        EVprint();
        return;
    }
    // if there is argument, set the environment
    if(argc == 3){
        EVset(argv[1],argv[2]);
    }
    else{
        printf("set: wrong argument number\n");
    }
}

void export(int argc, char *argv[])
{ /* export command */
    int i;
    // if no argument, print the environment
    if (argc == 1)
    {
        EVprint();
        return;
    }
    // if there is argument, set the environment
    if(argc == 2){
        EVexport(argv[1]);
    }
    else{
        printf("export: wrong argument number\n");
    }
}

void unset(int argc, char *argv[])
{ /* unset command */
    int i;
    // if no argument, print the environment
    if (argc == 1)
    {
        EVprint();
        return;
    }
    // if there is argument, set the environment
    if(argc == 2){
        EVset(argv[1],"");
    }
    else{
        printf("unset: wrong argument number\n");
    }
}
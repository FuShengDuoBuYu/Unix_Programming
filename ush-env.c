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
        if ((*p = malloc(size)) == NULL)
        {
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
            if (v == NULL)
            {
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

BOOLEAN EVunset(char *name)
{ /* unset variable */
    struct varslot *v = find(name);
    if (v == NULL || v->name == NULL)
        return (FALSE);
    v->name = NULL;
    v->val = NULL;
    v->exported = FALSE;
    return (TRUE);
}

//////////////////////////////////////////////////////////
// You must implement the invoke function 					//
// do the environment set								       //
//////////////////////////////////////////////////////////

BOOLEAN EVset(char *name, char *val)
{ /* add name & valude to enviromnemt */
    struct varslot *v = find(name);
    // set name and value
    if (!assign(&v->name, name) || !assign(&v->val, val))
    {
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

    // add PATH and HOME
    // get current path
    char *path = getenv("PATH");
    char *home = getenv("HOME");
    if (!EVset("PATH", path) || !EVexport("PATH"))
    {
        return (FALSE);
    }
    if (!EVset("HOME", home) || !EVexport("HOME"))
    {
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
    for (int i = 0; i < MAXVAR; i++)
        if (sym[i].name != NULL)
            printf("%3s %s=%s\n", sym[i].exported ? "[E]" : "", sym[i].name, sym[i].val);
}

BOOLEAN EVcommand(int argc, char *argv[])
{
    // only set
    if (argc == 1 && strcmp(argv[0], "set") == 0)
    {
        EVprint();
        return TRUE;
    }
    // only export
    if (argc == 1 && strcmp(argv[0], "export") == 0)
    {
        EVprint();
        return TRUE;
    }
    // set var value
    if (argc == 2 && strcmp(argv[0], "set") == 0)
    {
        // split the argv[1] to var and value
        char *name = strtok(argv[1], "=");
        char *value = strtok(NULL, "=");
        if (!EVset(name, value))
        {
            printf("set error\n");
        }
        return TRUE;
    }
    // export var
    if (argc == 2 && strcmp(argv[0], "export") == 0)
    {
        // split the argv[1] to var and value
        char *name = strtok(argv[1], "=");
        char *value = strtok(NULL, "=");
        if(find(name) == NULL || find(name)->name == NULL){
            if(!EVset(name, value)){
                printf("set error\n");
            }
        }
        if(!EVexport(name)){
            printf("export error\n");
        }
        return TRUE;
    }
    // unset var
    if (argc == 2 && strcmp(argv[0], "unset") == 0)
    {
        // split the argv[1] to var and value
        char *name = strtok(argv[1], "=");
        char *value = strtok(NULL, "=");
        if (!EVunset(name))
        {
            printf("unset error\n");
        }
        return TRUE;
    }
    return FALSE;
}
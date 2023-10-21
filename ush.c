#include "ush.h"
#include "ush-env.h"
#include "ush-parse.h"
#include "ush-prt.h"
#include "ush-sig.h"
#include "errno.h"

#define MAXARG 20
#define MAXFNAME 500
#define MAXWORD 100
#define BADFD -2

//////////////////////////////////////////////////////////
// You must implement the invoke function 					//
// return the process id									     //
/////////////////////////////////////////////////////////
static int invoke(int argc, char *argv[], int srcfd, char *srcfile,
									int dstfd, char *dstfile, BOOLEAN append,
									BOOLEAN bckgrnd)
/* invoke simple command */
{
	return 0;
}

//////////////////////////////////////////////////////////
// You must implement the redirect function 					//
/////////////////////////////////////////////////////////
static void redirect(int srcfd, char *srcfile, int dstfd, char *dstfile,
										 BOOLEAN append, BOOLEAN bckgrnd)
{ /* I/O redirection */
}

static void waitfor(int pid)
{ /* wait for child */

	int wpid, status;

	while ((wpid = wait(&status)) != pid && wpid != -1)
		statusprt(wpid, status);
	if (wpid == pid)
		statusprt(0, status);
}
//////////////////////////////////////////////////////////
// You must implement the builtin function 	to do				//
// the builtin command. 									//
// Note :: the builtin command should not invoke the linux standard //
// function.eg, cd should not invoke chdir						//
// return true if a builtin command ,false other wise				//
//////////////////////////////////////////////////////////

static BOOLEAN builtin(int argc, char *argv[], int srcfd, int dstfd)
/* do built-in */
{
	// the built-in function is cd,echo,exit,pwd
	// exit
	if (argc > 0 && strcmp(argv[0], "exit") == 0){
		if(argc == 1)
			exit(0);
		else
			exit(atoi(argv[1]));
	}
	// echo
	if (argc > 0 && strcmp(argv[0], "echo") == 0){
		int i;
		for(i = 1; i < argc; i++){
			printf("%s ", argv[i]);
		}
		printf("\n");
		return TRUE;
	}
	// pwd
	if (argc > 0 && strcmp(argv[0], "pwd") == 0){
		char buf[100];
		getcwd(buf, 100);
		printf("%s\n", buf);
		return TRUE;
	}
	// cd
	if (argc > 0 && strcmp(argv[0], "cd") == 0){
		if(argc == 1){
			chdir(getenv("HOME"));
		}
		else{
			chdir(argv[1]);
		}
		return TRUE;
	}
	return FALSE;
}

static TOKEN command(int *waitpid, BOOLEAN makepipe, int *pipefdp)
{ /* do simple cmd */

	TOKEN token, term;
	int argc, srcfd, dstfd, pid, pfd[2];
	char *argv[MAXARG + 1], srcfile[MAXFNAME], dstfile[MAXFNAME];
	char word[MAXWORD];
	BOOLEAN append;

	argc = 0;
	srcfd = 0;
	dstfd = 1;
	while (1)
	{
		switch (token = gettoken(word))
		{
		case T_WORD:
			if (argc == MAXARG)
			{
				fprintf(stderr, "Too many args\n");
				break;
			}
			if ((argv[argc] = malloc(strlen(word) + 1)) == NULL)
			{
				fprintf(stderr, "Out of argmemory\n");
				break;
			}
			strcpy(argv[argc], word);
			argc++;
			continue;
		case T_LT:
			if (makepipe)
			{
				fprintf(stderr, "Extra<\n");
				break;
			}
			if (gettoken(srcfile) != T_WORD)
			{
				fprintf(stderr, "Illegal <\n");
				break;
			}
			srcfd = BADFD;
			continue;
		case T_GT:
			if (dstfd != 1)
			{
				fprintf(stderr, "Extra > or >> \n");
				break;
			}
			if (gettoken(dstfile) != T_WORD)
			{
				fprintf(stderr, "Illegal > or >> \n");
				break;
			}
			dstfd = BADFD;
			append = FALSE;
			continue;
		case T_GTGT:
			if (dstfd != 1)
			{
				fprintf(stderr, "Extra > or >>\n");
				break;
			}
			if (gettoken(dstfile) != T_WORD)
			{
				fprintf(stderr, "Illegal >or >>\n");
				break;
			}
			dstfd = BADFD;
			append = TRUE;
			continue;
		case T_BAR:
		case T_AMP:
		case T_SEMI:
		case T_NL:
			argv[argc] = NULL;
			if (token == T_BAR)
			{
				if (dstfd != 1)
				{
					fprintf(stderr, "> or >> conflicts with |\n");
					break;
				}
				term = command(waitpid, TRUE, &dstfd);
			}
			else
				term = token;
			// here invoke the builtin function
			if (!builtin(argc, argv, srcfd, dstfd)){
				perror("builtin");
			}
				
			if (makepipe)
			{
				if (pipe(pfd) == -1)
					syserr("pipe");
				*pipefdp = pfd[1];
				srcfd = pfd[0];
			}
			if (term == T_AMP)
				pid = invoke(argc, argv, srcfd,
										 srcfile, dstfd, dstfile, append, TRUE);
			else
				pid = invoke(argc, argv, srcfd, srcfile, dstfd, dstfile,
										 append, FALSE);

			if (token != T_BAR)
				*waitpid = pid;
			if (argc == 0 && (token != T_NL || srcfd > 1))
				fprintf(stderr, "Missing command\n");
			while (--argc >= 0)
				free(argv[argc]);
			return (term);
		case T_EOF:
			exit(0);
		}
	}
}

/////////////////////////////////////////////////////////////////
// you should better not modify this function 						     //
/////////////////////////////////////////////////////////////////
int main()
{ /* real shell */
	char *prompt;
	int pid, fd;
	TOKEN term;
	void waitfor();

	ignoresig();
	if (!EVinit())
		fatal("can't initialize environment");
	if ((prompt = EVget("PS2")) == NULL)
		prompt = ">";
	printf("%s", prompt);

	while (1)
	{
		term = command(&pid, FALSE, NULL);
		if (term != T_AMP && pid != 0)
			waitfor(pid);
		if (term == T_NL)
			printf("%s", prompt);
		for (fd = 3; fd < 20; fd++)
			(void)close(fd); /* ignore error */
	}
	return (0);
}

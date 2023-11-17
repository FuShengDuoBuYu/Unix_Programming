#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
static volatile sig_atomic_t sigflag; /* set nonzero by sig handler */
static sigset_t newmask, oldmask, zeromask;

// handle the signal with set the flag 1
static void sig_usr(int signo){
	sigflag = 1;
}


void TELL_WAIT(void){
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
		printf("signal(SIGUSR1) error");
	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
		printf("signal(SIGUSR2) error");
	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);

    // block the new mask signal and backup the oldmask
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		printf("SIG_BLOCK error");
}

void TELL_PARENT(pid_t pid){
    // use kill to send signal
	kill(pid, SIGUSR2);
}

void WAIT_PARENT(void){
	while (sigflag == 0)
        // wait for all signals
		sigsuspend(&zeromask);
	sigflag = 0;

	/* Reset signal mask to original value */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		printf("SIG_SETMASK error");
}

void TELL_CHILD(pid_t pid){
    // send the signal to tell that finish
	kill(pid, SIGUSR1);
}

void WAIT_CHILD(void){
	while (sigflag == 0)
		sigsuspend(&zeromask);
	sigflag = 0;

	/* Reset signal mask to original value */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		printf("SIG_SETMASK error");
}

# 系统程序设计lab3
> 郭仲天 19307110250

## 实验目的
使用信号实现父,子进程之间的同步,了解`sigprocmask`,`sigsupend`等函数的使用

## 实验介绍
参考教材《UNIX环境高级编程》图10.24,实现`TELL_CHILD`,`WAIT_PARENT`,`TELL_PARENT`,`WAIT_CHILD`,`TELL_WAIT`函数,并用上述接口在父子进程之间实现同步，当父进程打印 "here is parent"后，子进程才打印"this is child"，注意:不能在子进程中使用`sleep()`接口

> 图10.24

```c
#include "apue.h"

static volatile sig_atomic_t sigflag; /* set nonzero by sig handler */
static sigset_t newmask, oldmask, zeromask;

static void
sig_usr(int signo)	/* one signal handler for SIGUSR1 and SIGUSR2 */
{
	sigflag = 1;
}

void
TELL_WAIT(void)
{
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_sys("signal(SIGUSR1) error");
	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
		err_sys("signal(SIGUSR2) error");
	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);

	/* Block SIGUSR1 and SIGUSR2, and save current signal mask */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");
}

void
TELL_PARENT(pid_t pid)
{
	kill(pid, SIGUSR2);		/* tell parent we're done */
}

void
WAIT_PARENT(void)
{
	while (sigflag == 0)
		sigsuspend(&zeromask);	/* and wait for parent */
	sigflag = 0;

	/* Reset signal mask to original value */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
}

void
TELL_CHILD(pid_t pid)
{
	kill(pid, SIGUSR1);			/* tell child we're done */
}

void
WAIT_CHILD(void)
{
	while (sigflag == 0)
		sigsuspend(&zeromask);	/* and wait for child */
	sigflag = 0;

	/* Reset signal mask to original value */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
}

```

## 实验要求
1. 实验报告请提交源代码和实验报告Word文档(包括运行截图，实验思路和代码分析)，打包后以“学号_姓名.zip”命名，上传到本课程elearning平台
2. 正确实现TELL_CHILD(10分)，WAIT_PARENT(10分)，TELL_PARENT(10分)，WAIT_CHILD(10分)，TELL_WAIT(10分)，程序能正常运行且实现父子进程同步按要求输出(40分)，实验分析和总结(10分)

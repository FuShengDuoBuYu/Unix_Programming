#include "signal_synchronize.h"

int main(){
    pid_t pid;
    // init the synchronize function
    TELL_WAIT();
    // fork the process
    pid = fork();
    if(pid < 0){
        printf("fork error!");
        exit(0);
    }
    // child process 
    else if(pid == 0){
        WAIT_PARENT();
        printf("this is child\n");
        TELL_PARENT(getppid());
    }
    // parent process
    else{
        printf("here is parent\n");
        TELL_CHILD(pid);
        WAIT_CHILD();

    }
    exit(0);
}


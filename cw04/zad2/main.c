#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>


void handler_SIGINT(int sig, siginfo_t *info, void *ucontext){
    printf("\nReceived signal (%d) | PID: %d | PPID: %d\n", info->si_signo, info->si_pid, getppid());
    printf("------------- INFO --------------\n");
    printf("User ID of sending process: %d\n", info->si_uid);
    printf("Code: %d\n", info->si_code);
    printf("User time: %ld\n", info->si_utime);
}

void handler_NOCLDSTOP(int signum){
    printf("Received signal (%d) | PID: %d | PPID: %d\n", signum, getpid(), getppid());
}

void handler_RESETHAND(int signum){
    printf("Handler function here\n");
    printf("Received signal (%d) | PID: %d | PPID: %d\n", signum, getpid(), getppid());
}


void test_SIGINFO(){
    printf("\n\n---------------- TEST FOR SIGINFO ----------------\n");

    struct sigaction act_SIGINT;
    sigemptyset(&act_SIGINT.sa_mask);
    act_SIGINT.sa_sigaction = handler_SIGINT;
    act_SIGINT.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &act_SIGINT, NULL);

    printf("Flag ON\n");
    raise(SIGINT);
}



int recursive_sig1 = 0, recursive_sig2 = 0;

void sig1_handler(int sig) {
    if (recursive_sig1 == 0) {
        recursive_sig1 = 1;
        raise(SIGUSR1);
    }
    else if (recursive_sig1 == 1) {
        printf("SIGUSR1 recursive call\n");
    }
    recursive_sig1 = 2;
}

void sig2_handler(int sig) {
    if (recursive_sig2 == 0) {
        recursive_sig2 = 1;
        raise(SIGUSR1);
    }
    else if (recursive_sig2 == 1) {
        printf("SIGUSR1 recursive call\n");
    }
    recursive_sig2 = 2;
}

void test_NODEFER(){
    printf("\n\n---------------- TEST FOR NODEFER ----------------\n");

    struct sigaction act;
    act.sa_handler = sig1_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);
    
    printf("Flag OFF\n");
    raise(SIGUSR1);

    act.sa_handler = sig2_handler;
    act.sa_flags = SA_NODEFER;
    sigaction(SIGUSR1, &act, NULL);
    printf("Flag ON\n");
    raise(SIGUSR1);
}

void sig_handler(int sig) {
    printf("SIGUSR1 received\n");
}

void test_RESETHAND() {
    printf("\n\n---------------- TEST FOR RESETHAND ----------------\n");

    struct sigaction act;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);
   
    printf("Flag OFF\n");
    raise(SIGUSR1);
    raise(SIGUSR1);

    act.sa_flags = SA_RESETHAND;
    sigaction(SIGUSR1, &act, NULL);
    printf("Flag ON\n");
    raise(SIGUSR1);
    raise(SIGUSR1); 
}


void test_NOCLDSTOP(){
    printf("\n\n---------------- TEST FOR NOCLDSTOP ----------------\n");

    struct sigaction act_SIGCHLD;
    sigemptyset(&act_SIGCHLD.sa_mask);
    act_SIGCHLD.sa_handler = &handler_NOCLDSTOP;
    sigaction(SIGCHLD, &act_SIGCHLD, NULL);

    printf("Flag OFF\n");

    pid_t pid = fork();
    if(pid == 0){
    }
    else{
        printf("Parent process here: %d\n", getpid());
        printf("Child process stopped, SIGCHLD should be sent\n");
        kill(pid, SIGSTOP);
        sleep(1);
    }


    printf("Flag ON\n");
    act_SIGCHLD.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &act_SIGCHLD, NULL);

    pid = fork();
    if(pid == 0){
    }
    else{
        printf("Parent process here: %d\n", getpid());
        printf("Child process stopped, but SIGCHLD should NOT be sent\n");
        kill(pid, SIGSTOP);
        sleep(1);
    }
}



void main(){

    test_SIGINFO();
    test_NODEFER();
    test_NOCLDSTOP();
    test_RESETHAND();
}
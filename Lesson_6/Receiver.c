#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

void true_handler(int signo, siginfo_t *info, void *ucontext);
void false_handler(int signo, siginfo_t *info, void *ucontext);
void empty(int signo);
void parentexit(int signo);
void finish(int signo);

char translation_has_begun = 0;
int out_char = 0, counter = 128, inf_buf_size = 1;
pid_t pid = 0;

int main(int argc, char** argv)
    {
    printf("receiver pid = %d\n", getpid());
    sigset_t set;
    int file_out = open(argv[1], O_WRONLY | O_CREAT, 0666);

    struct sigaction act_finish;
    memset(&act_finish, 0, sizeof(act_finish));
    act_finish.sa_handler = finish;
    sigfillset(&act_finish.sa_mask);
    sigaction(SIGCHLD, &act_finish, NULL);

    struct sigaction act_alarm;
    memset(&act_alarm, 0, sizeof(act_alarm));
    act_alarm.sa_handler = parentexit;
    sigfillset(&act_alarm.sa_mask);
    sigaction(SIGALRM, &act_alarm, NULL);

    struct sigaction sig_true;
    memset(&sig_true, 0, sizeof(sig_true));
    sig_true.sa_flags    = SA_SIGINFO;
    sig_true.sa_sigaction = true_handler;
    sigfillset(&sig_true.sa_mask);
    sigaction(SIGUSR1, &sig_true, NULL);

    struct sigaction sig_false;
    memset(&sig_false, 0, sizeof(sig_false));
    sig_false.sa_flags    = SA_SIGINFO;
    sig_false.sa_sigaction = false_handler;
    sigfillset(&sig_false.sa_mask);    
    sigaction(SIGUSR2, &sig_false, NULL);
    
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigprocmask(SIG_BLOCK, &set, NULL);
    sigemptyset(&set);

    int i = 0;
    do 
        {	
        if (counter == 0)
            {
            int size = write(file_out, &out_char, 1);
            if (size == -1) 
                printf("write_problem\n");
            i++;    
            counter = 128;
            out_char = 0;
            }
        if ((translation_has_begun > 0) && kill(pid, 0) == -1)
            {
            printf("sender died\n");
            exit(1);
            }
        alarm(5);
        sigsuspend(&set);
        } while (1);
    }

void true_handler(int signo, siginfo_t *info, void *ucontext) 
    {
    if (!translation_has_begun)
        {
        translation_has_begun = 1;
        pid = info -> si_pid;
        }
    if (pid != info -> si_pid)
        kill(info -> si_pid, SIGKILL);
    out_char += counter;
    counter /= 2;	
    kill(pid, SIGUSR1);
    }

void false_handler(int signo, siginfo_t *info, void *ucontext) 
    {
    if (!translation_has_begun)
        {
        translation_has_begun = 1;
        pid = info -> si_pid;
        }
    if (pid != info -> si_pid)
        kill(info -> si_pid, SIGKILL);
    counter/=2;	
    kill(pid, SIGUSR1);
    }

void empty(int signo) 
    {
    }

void parentexit(int signo) 
    { 
    if (kill(pid, 0) == -1)
        {
        printf("Sender doesn't exist\n");
        exit(EXIT_SUCCESS);
        }
    }

void finish(int signo)
    {
    printf("success\n");
    exit (0);
    }
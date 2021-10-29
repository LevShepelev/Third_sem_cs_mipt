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
void Get_info();

char translation_has_begun = 0;
int out_char = 0, counter = 128, inf_buf_size = 1;
pid_t pid;
char* inf_buf = NULL;

int main(int argc, char** argv)
    {
    printf("receiver pid = %d\n", getpid());
    sigset_t set;
    int file_out = open(argv[1], O_WRONLY | O_CREAT, 0666);

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
            inf_buf[i] = out_char;
            i++;    
            counter = 128;
            out_char = 0;
            }
        sigsuspend(&set);
        } while (i < inf_buf_size);

    
    int size = write(file_out, inf_buf, inf_buf_size);
    if (size == -1) 
        printf("write_problem\n");
    free(inf_buf);
    }

void true_handler(int signo, siginfo_t *info, void *ucontext) 
    {
    if (!translation_has_begun)
        Get_info();
    if (pid != info -> si_pid)
        kill(info -> si_pid, SIGKILL);
    out_char += counter;
    counter /= 2;	
    kill(pid, SIGUSR1);
    }

void false_handler(int signo, siginfo_t *info, void *ucontext) 
    {
    if (!translation_has_begun)
        Get_info();
    if (pid != info -> si_pid)
        kill(info -> si_pid, SIGKILL);
    counter/=2;	
    kill(pid, SIGUSR1);
    }

void empty(int signo) {
}

void Get_info()
    {
    int shmid_1 = shmget(10, 2 * sizeof(int), 0666);
    int* buf_1 = (int*) shmat(shmid_1, NULL, 0);
    pid = buf_1[0];
    inf_buf_size = buf_1[1];
    inf_buf = (char*) calloc (inf_buf_size, sizeof(char));
    shmdt(buf_1);
    shmctl(10, IPC_RMID, NULL);
    translation_has_begun = 1;
    }
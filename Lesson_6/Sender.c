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

void one(int signo, siginfo_t *info, void *ucontext);
void zero(int signo, siginfo_t *info, void *ucontext); 
void empty(int signo);
void parentexit(int signo) ;
int pid = 0, counter = 128, out_char = 0;

int main(int argc, char** argv)
    {
    clock_t start, end;
    start = clock();
    int pid = atoi(argv[2]);
    struct stat statistica;
    int stat_error = stat (argv[1], &statistica);
    assert(stat_error == 0);
    int file_in = open(argv[1], O_RDONLY);
    if (file_in == -1)
        exit(-1);
    char* buf = (char*) calloc (statistica.st_size, sizeof(char));
    if (buf == NULL)
        exit(1);
    int size = read(file_in, buf, statistica.st_size);
    if (size == -1)
      printf("read_problem\n");
    
    int shmid_1 = shmget(10, 2 * sizeof(int), IPC_CREAT | 0666);
    int* shm_buf = (int*) shmat(shmid_1, NULL, 0);
    shm_buf[0] = getpid();
    shm_buf[1] = statistica.st_size;
    shmdt(shm_buf);
    
    struct sigaction act_alarm;
    memset(&act_alarm, 0, sizeof(act_alarm));
    act_alarm.sa_handler = parentexit;
    sigfillset(&act_alarm.sa_mask);
    sigaction(SIGALRM, &act_alarm, NULL);

    sigset_t set;
    struct sigaction sig_true;
    memset(&sig_true, 0, sizeof(sig_true));
    sig_true.sa_sigaction = one;
    sig_true.sa_flags    = SA_SIGINFO;
    sigfillset(&sig_true.sa_mask);
    sigaction(SIGUSR1, &sig_true, NULL);

    struct sigaction sig_false;
    memset(&sig_false, 0, sizeof(sig_false));
    sig_false.sa_sigaction = zero;
    sig_false.sa_flags   = SA_SIGINFO;
    sigfillset(&sig_false.sa_mask);    
    sigaction(SIGUSR2, &sig_false, NULL);

    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigprocmask(SIG_BLOCK, &set, NULL );
    sigemptyset(&set);

    sigemptyset(&set);

    struct sigaction sig_empty;                    
    memset(&sig_empty, 0, sizeof(sig_empty));
    sig_empty.sa_handler = empty;
    sigfillset(&sig_empty.sa_mask);    
    sigaction(SIGUSR1, &sig_empty, NULL);

    int i, j = 0;
    while (j < statistica.st_size){	
      alarm(1);
      for (i = 128; i >= 1; i /= 2){
        if (i & buf[j])              
          kill(pid, SIGUSR1);
        else                      
          kill(pid, SIGUSR2);
        sigsuspend(&set); 
      }
    j++; 
    }
    kill(pid, SIGUSR1);
    end = clock();
    printf("time = %f, size = %ld speed = %f B/s\n", (double) (end - start) / (CLOCKS_PER_SEC), statistica.st_size, (double) statistica.st_size / ((double)(end - start) / (CLOCKS_PER_SEC)));
    free(buf);
    }

void one(int signo, siginfo_t *info, void *ucontext) 
    {
    out_char += counter;
    counter /= 2;	
    kill(pid, SIGUSR1);
    }

// SIGUSR2
void zero(int signo, siginfo_t *info, void *ucontext) 
    {
    counter/=2;	
    kill(pid, SIGUSR1);
    }

// Nothing
void empty(int signo) {
}

void parentexit(int signo) { 
  exit(EXIT_SUCCESS);
}
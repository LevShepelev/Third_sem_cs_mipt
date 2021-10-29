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

void True_handler(int signum);
void False_handler(int signum);
void Get_info();
void next_symb();

char out_symb = 0;
char degrees_of_2[8] = {1};
char counter = 7;
int sender_pid = 0;
char translation_has_begun = 0;
int inf_buf_size = 0;
char* inf_buf = NULL;
int current_symb = 0;
char exit_flag = 0;

int main(int argc, char** argv)
    {
    int file_out = open(argv[1], O_CREAT | O_WRONLY, 0666);
    sigset_t set;

    struct sigaction sig_true;
    memset(&sig_true, 0, sizeof(sig_true));
    sig_true.sa_handler = True_handler;
    sigfillset(&sig_true.sa_mask);
    sigaction(SIGUSR1, &sig_true, NULL);

    // SIGUSR2 - zero()
    struct sigaction sig_false;
    memset(&sig_false, 0, sizeof(sig_false));
    sig_false.sa_handler = False_handler;
    sigfillset(&sig_false.sa_mask);    
    sigaction(SIGUSR2, &sig_false, NULL);

    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigprocmask(SIG_BLOCK, &set, NULL );
    sigemptyset(&set);

    printf("receiver pid = %d\n", getpid());
    for (int j = 1; j < 8; j++)
        degrees_of_2[j] = degrees_of_2[j - 1] * 2;
    while(1)
        {
        if (counter == 0)
            {    
            inf_buf[current_symb] = out_symb;          
            counter = 7;
            out_symb = 0;
            current_symb++;
            if (current_symb == inf_buf_size)
                break;
            }   
        sigsuspend(&set);
        }
    for (int j = 0; j < inf_buf_size; j++)
    write(file_out, inf_buf, inf_buf_size);
    free(inf_buf);
    }

void True_handler(int signum)
    {
    if (!translation_has_begun)
        Get_info();
    out_symb = degrees_of_2[counter] + out_symb; 
    printf("counter = %d\n", counter);   
    counter--;
    }

void False_handler(int signum)
    {
    if (!translation_has_begun)
        Get_info();
    printf("counter = %d\n", counter);
    counter--;
    }

void Get_info()
    {
    int shmid_1 = shmget(10, 2 * sizeof(int), 0666);
    int* buf_1 = (int*) shmat(shmid_1, NULL, 0);
    sender_pid = buf_1[0];
    inf_buf_size = buf_1[1];
    inf_buf = (char*) calloc (inf_buf_size, sizeof(char));
    printf("inf_buf_size = %d\n", inf_buf_size);
    shmdt(buf_1);
    shmctl(10, IPC_RMID, NULL);
    translation_has_begun = 1;
    }

void next_symb()
    {
    inf_buf[current_symb] = out_symb;
    out_symb = 0;
    counter = 7;
    printf("counter = %d, cur_symb = %d\n", counter, current_symb);
    if (current_symb == inf_buf_size - 1)
        exit_flag = 1;
    current_symb++;
    }
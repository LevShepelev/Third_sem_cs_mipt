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
void Ready_to_send_handler(int signum);
void empty();

char Ready_to_send = 0;
char degrees_of_2[8] = {1};

int main(int argc, char** argv)
    {
    int pid = atoi(argv[2]);
    struct stat statistica;
    int stat_error = stat (argv[1], &statistica);
    assert(stat_error == 0);
    int file_in = open(argv[1], O_RDONLY);
    if (file_in == -1)
        exit(-1);
    char* buf = (char*) calloc (statistica.st_size, sizeof(char));
    read(file_in, buf, statistica.st_size);
    for (int j = 1; j < 8; j++)
        degrees_of_2[j] = degrees_of_2[j - 1] * 2;

    int shmid_1 = shmget(10, 2 * sizeof(int), IPC_CREAT | 0666);
    int* shm_buf = (int*) shmat(shmid_1, NULL, 0);
    shm_buf[0] = getpid();
    shm_buf[1] = statistica.st_size;
    shmdt(shm_buf);

    sigset_t set;
    sigemptyset(&set); // очищает набор сигналов

    // SIGUSR1 - empty()
    struct sigaction act_empty;                    
    memset(&act_empty, 0, sizeof(act_empty));
    act_empty.sa_handler = empty;
    sigfillset(&act_empty.sa_mask);    
    sigaction(SIGUSR1, &act_empty, NULL);


    for (int i = 0; i < statistica.st_size; i++)
        {
        printf("%d\n", i);
        for (int j = 0; j < 8; j++)
            {
            printf("j = %d\n", j);
            if (degrees_of_2[j] & buf[i])
                kill(pid, SIGUSR1);
            else kill(pid, SIGUSR2);

            sigsuspend(&set);
            }
        }
    free(buf);
    }

void empty()
    {
    }

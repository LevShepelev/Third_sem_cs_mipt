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
#include <time.h>


int main(int argc, char** argv)
    {
    srand(time(NULL));  
    const int key = rand() % 1000;
    struct stat statistica;
    int stat_error = stat (argv[1], &statistica);
    assert(stat_error == 0);
    int fin = open(argv[1], O_RDONLY | O_CREAT);

    int shmid = shmget(key, statistica.st_size, IPC_CREAT | 0666);
    char* shm_buf = (char*) shmat(shmid, NULL, 0);
    perror("shmdt\n");
    perror("shmget\n");

    read(fin, shm_buf, statistica.st_size);

    int shmid_1 = shmget(10, 2 * sizeof(int), IPC_CREAT | 0666);
    int* buf = (int*) shmat(shmid_1, NULL, 0);

    buf[0] = key;
    buf[1] = statistica.st_size;
    shmdt(shm_buf);
    shmdt(buf);
    }
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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

int main()
    {
    int shmid = shmget(10, 10, IPC_CREAT | 0666);
    char* shmmem = shmat(shmid,ULL, 0)
    }
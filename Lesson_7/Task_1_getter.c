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


int main(int argc, char** argv)
    {
    int fout = open(argv[1], O_CREAT | O_WRONLY, 0600);
    int shmid_1 = shmget(10, 2 * sizeof(int), 0666);
    int* buf_1 = (int*) shmat(shmid_1, NULL, 0);

    int shmid = shmget(buf_1[0], buf_1[1], 0666);
    char* buf = (char*) shmat(shmid, NULL, 0);
    write(fout, buf, buf_1[1]);
    shmdt(buf);
    shmdt(buf_1);
    shmctl(shmid, IPC_RMID, NULL);
    shmctl(10, IPC_RMID, NULL);
    }
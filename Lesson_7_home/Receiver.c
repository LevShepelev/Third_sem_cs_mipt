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
#include <sys/msg.h>

int main(int argc, char** argv)
    {
    void* msgp = (void*) calloc (1000 + sizeof(long), sizeof(char));
    if (msgp == NULL)
        exit(1);

    int msgid = msgget(14, 0666);
    msgrcv(msgid, msgp, 1000, 1, IPC_NOWAIT);
    printf("%s\n", (char*)(msgp + sizeof(long)));
    free(msgp);
    }
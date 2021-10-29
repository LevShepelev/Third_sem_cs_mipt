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
    const int buf_size = 1000;
    int msgid = msgget(14, IPC_CREAT | 0666);
    void* msgp = (void*) calloc (buf_size + sizeof(long), sizeof(char));
    if (msgp == NULL)
        exit(1);
    char symb;
    int size = 0;
    do 
        {
        scanf("%c", &symb);
        ((char*) msgp)[sizeof(long) + size] = symb;
        size++;
        } while (symb != '\n');
    long mtype = 1;
    ((long*) msgp)[0] = mtype;
    msgsnd(msgid, msgp, size, 0); 
    free(msgp);
    sleep(30);
    }
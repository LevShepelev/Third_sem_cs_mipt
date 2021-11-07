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
#include "Shared_stack.h"


int main()
    {
    Stack_t* stack = attach_stack(10, 5);
    int a = 0;
    push(stack, (void*) &a);
    stack_dump(stack);
    sleep(30);
    detach_stack(stack);
    }
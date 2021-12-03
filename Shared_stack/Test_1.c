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
    for (int i = 0; i < 10; i++)
        {
        push(stack, i * 10);
        printf("size = %d", get_count(stack));
        }
    stack_dump(stack);
    sleep(15);
    detach_stack(stack);
    mark_destruct(stack);
    }
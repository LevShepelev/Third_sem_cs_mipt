#include "Shared_stack.h"
#define number_of_proccess 10

int main()
    {
    int i;
    int pid = getpid();
    printf("main pid = %d\n", pid);
    int data = 0;
    for(i = 0; i < number_of_proccess; i++)
        fork();
    setsid();
    Stack_t* stack = attach_stack(10, 1000000);
    for (i = 0; i < 10; i++)
        {
        push(stack, i);
        }
    int sum = 0;
    for (i = 0; i < 10; i++)
        {
        pop(stack, &data);
        sum += data;
        //printf("popped %d time %d\n", data, i);
        }
    //printf("%d\n", sum);
                    //printf("curr_size = %d\n", get_count(stack));

    if (pid == getpid()) {
        if(!fork()) {
            setsid();
            kill(-pid, SIGKILL);
            push(stack, i);
            push(stack, i);
            pop(stack, &data);
            pop(stack, &data);
        }

        detach_stack(stack);
        mark_destruct(stack);
        return 0;
    }
    detach_stack(stack);
    return 0;
    }
#include "Shared_stack.h"

int main()
    {
    int i, pid = getpid();
    int data;

    for(i = 0; i < 5; i++)
        fork();
    Stack_t* stack = attach_stack(10, 1000);
    for (int i = 0; i < 10; i++)
        {
        push(stack, getpid());
        }

    for (int i = 0; i < 10; i++)
        {
        pop(stack, &data);
        printf("popped %d time %d\n", data, i);
        }
    if (pid == getpid()) {
        sleep(2);
                printf("curr_size = %d\n", get_count(stack));

        detach_stack(stack);
        mark_destruct(stack);
        sleep(2);
        return 0;
    }
    detach_stack(stack);
    return 0;
    }
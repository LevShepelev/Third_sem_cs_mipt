#include "Shared_stack.h"
#define number_of_proccess 10

int main()
    {
    int pid = getpid();
    printf("main pid = %d\n", pid);
    int data = 0;
    int pids[number_of_proccess];
    for(int i = 0; i < number_of_proccess; i++)
        pids[i] = fork();
    
    Stack_t* stack = attach_stack(10, 1000000);
    for (int i = 0; i < 10; i++)
        {
        push(stack, i);
        }
    int sum = 0;
    for (int i = 0; i < 8; i++)
        {
        pop(stack, &data);
        sum += data;
        //printf("popped %d time %d\n", data, i);
        }
    //printf("%d\n", sum);
                    //printf("curr_size = %d\n", get_count(stack));

    if (pid == getpid()) {
        sleep(2);
        for (int i = 0; i < number_of_proccess; ++i)
            {
            printf("pids[%d] = %d\n", i, pids[i]);
            if (kill(pids[i], SIGINT) != -1)
                printf("killed %d\n", pids[i]);
            }
                printf("End:curr_size = %d\n", get_count(stack));
        printf("killed\n");
        sleep(1);
        for (int i = 0; i < 10; i++)
            {
            pop(stack, &data);
            printf("%d\n", data);
            }
        detach_stack(stack);
        mark_destruct(stack);
        return 0;
    }
    detach_stack(stack);
    return 0;
    }
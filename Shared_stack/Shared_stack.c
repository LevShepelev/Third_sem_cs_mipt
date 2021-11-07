#include "Shared_stack.h"

int array_start_size = 5;
const int SEM_KEY = 20;

Stack_t* attach_stack(key_t key, int size)
    {
    int shmid_unique_checker = shmget(key, sizeof(Stack_t), IPC_CREAT | 0666 | IPC_EXCL);
    int shmid = shmget(key, sizeof(Stack_t), IPC_CREAT | 0666);
    Stack_t* stack = (Stack_t*) shmat(shmid, NULL, 0);
    stack -> stack_shmid = shmid;
    array_start_size = size;
    if (shmid_unique_checker != -1)
        {
        printf("Creating\n");
        stack -> parts_max_size = Max_number_of_parts;
        stack -> parts_shmid = shmget(key + 10, stack -> parts_max_size * sizeof(St_elem_t), IPC_CREAT | 0666);
        perror("shmget ");
        stack -> top_part = -1;
        stack -> n_processes = 0;
        }
    stack -> parts = (St_elem_t*) shmat(stack -> parts_shmid, NULL, 0);//to do: here is logical adreess, we dont need to save logical adress in shmmemory, save it on process memory
    stack -> n_processes++;
    stack -> sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    semctl(stack -> sem_id, 0, SETVAL, 1);
    perror("semctl ");
    stack_dump(stack);
    getcher();
    return stack;
    }


int detach_stack(Stack_t* stack)
    {
    for (int i = 0; i <= stack -> top_part; i++)
        {
        shmdt(stack -> parts[i].array);
        shmctl(stack -> parts[i].shmid, IPC_RMID, NULL);
        }
    stack -> n_processes--;
    if (stack -> n_processes == 0)
        semctl(stack -> sem_id, 0, IPC_RMID);
    shmdt(stack -> parts);
    shmctl(stack -> parts_shmid, IPC_RMID, NULL);
    int stack_shmid = stack -> stack_shmid;
    shmdt(stack);
    shmctl(stack_shmid, IPC_RMID, NULL);
    }


int push(Stack_t* stack, void* val) //to do semaphore
    {
    printf("before push sem = %d\n", semctl(stack -> sem_id, 0, GETVAL));
    getchar();
    struct sembuf sem_lock = {0, -1, 0};
    semop(stack -> sem_id, &sem_lock, 1);
    if (stack -> top_part >= 0 && (stack -> parts[stack -> top_part].top) < (stack -> parts[stack -> top_part].size))
        {
        //stack -> parts[stack -> top_part].array = shmat(stack -> parts[stack -> top_part].shmid, NULL, 0);
        stack -> parts[stack -> top_part].top++;
        stack -> parts[stack -> top_part].array[stack -> parts[stack -> top_part].top] = val;
        //shmdt(stack -> parts[stack -> top_part].array);
        stack_dump(stack);
        }
    else 
        {
        stack -> top_part++;
        stack -> parts[stack -> top_part].shmid = shmget(stack -> top_part + 'a', array_start_size * sizeof(void*), IPC_CREAT | 0666);
        stack -> parts[stack -> top_part].array = shmat(stack -> parts[stack -> top_part].shmid, NULL, 0);
        perror("push ");
        stack -> parts[stack -> top_part].size  = array_start_size;
        stack -> parts[stack -> top_part].top   = 0;
        stack -> parts[stack -> top_part].array[0] = val; 
        stack_dump(stack);
        //printf("top+part = %d, stack -> parts[%d] = %p\n", stack -> top_part, stack -> top_part, stack -> parts[stack -> top_part].array);  
        }
    struct sembuf sem_unlock = {0, 1, 0};
    semop(stack -> sem_id, &sem_unlock, 1);
    printf("after push sem = %d\n", semctl(stack -> sem_id, 0, GETVAL));
    return 0;
    }

int pop(Stack_t* stack, void** val)
    {
    printf("before pop sem = %d\n", semctl(stack -> sem_id, 0, GETVAL));
    getchar();
    struct sembuf sem_lock = {0, -1, 0};
    semop(stack -> sem_id, &sem_lock, 1);
    if (stack -> top_part >= 0)
        {
        *val = (stack -> parts[stack -> top_part]).array[stack -> parts[stack -> top_part].top];
        (stack -> parts[stack -> top_part]).top--;
        if ((stack -> parts[stack -> top_part]).top == -1)
            {
            shmdt(stack -> parts[stack -> top_part].array);
            shmctl(stack -> parts[stack -> top_part].shmid, IPC_RMID, NULL);
            stack -> top_part--;
            stack_dump(stack);
            }
        }
    else 
        {
        printf("Stack is empty\n");
        *val = NULL;
        }
    struct sembuf sem_unlock = {0, 1, 0};
    semop(stack -> sem_id, &sem_unlock, 1);
    printf("after pop sem = %d\n", semctl(stack -> sem_id, 0, GETVAL));
    }


int stack_dump(Stack_t* stack)
    {
    printf("stack = %p\nparts = %p\ntop_part = %d\nstack_shmid = %d\nparts_shmid = %d\nn_proccess = %d\n", stack, stack -> parts, stack -> top_part, stack -> stack_shmid, stack -> parts_shmid, stack -> n_processes);
    if (stack -> top_part != -1) 
        {
        printf("parts[%d].array = %p, size = %d\n", stack -> top_part, stack -> parts[stack -> top_part].array, stack -> parts[stack -> top_part].size);
        for (int i = 0; i <= stack -> parts[stack -> top_part].top; i++)
            {
            printf("stack -> parts[%d].array[%d] = %d\n", stack -> top_part, i, *(int*)stack -> parts[stack -> top_part].array[i]);
            }
        }
    }

int get_size(Stack_t* stack)
    {

    }
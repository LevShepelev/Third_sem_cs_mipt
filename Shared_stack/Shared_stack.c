#include "Shared_stack.h"

int array_start_size = 5;
const int SEM_KEY = 20;

Stack_t* attach_stack(key_t key, int size)
    {
    //printf("In attach\n\n");
    Stack_t* stack = (Stack_t*) calloc(1, sizeof(Stack_t));
    int shmid_unique_checker = shmget(key, sizeof(Stack_info_t), IPC_CREAT | 0666 | IPC_EXCL);
    int shmid = shmget(key, sizeof(Stack_info_t), IPC_CREAT | 0666);
    stack -> info = (Stack_info_t*) shmat(shmid, NULL, 0);
    stack -> info -> sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    semctl(stack -> info -> sem_id, 0, SETVAL, 1);
    struct sembuf sem_lock = {0, -1, 0};
    semop(stack -> info -> sem_id, &sem_lock, 1);
    stack -> info_shmid = shmid;
    stack -> top_shmatted_part = -1;
    array_start_size = size;
    if (shmid_unique_checker != -1)
        {
        printf("Creating\n");
        stack -> info -> parts_max_size = 10;
        stack -> info -> top_part = -1;
        stack -> info -> n_processes = 0;
        }
    else 
        {
        for (int i = 0; i <= stack -> info -> top_part; i++)
            {
            stack -> ptr_table[i] = shmat(stack -> info -> parts[i].shmid, NULL, 0);
            if (stack -> ptr_table[i] == NULL)
                {
                printf("attaching error\n");
                return NULL;
                }
            }
        }
    printf ("n_process = before attach %d",  stack -> info -> n_processes);
    printf(" pid = %d\n", getpid());
    stack -> info -> n_processes++;
    
    

    if (shmid == -1 || stack -> info -> sem_id == -1)
        return NULL;
    struct sembuf sem_unlock = {0, 1, 0};
    semop(stack -> info -> sem_id, &sem_unlock, 1);
    return stack;
    }


int detach_stack(Stack_t* stack)
    {
    //printf("in detach\n\n");
    struct sembuf sem_lock = {0, -1, 0};
    semop(stack -> info -> sem_id, &sem_lock, 1);
    Syncronising_new_parts(stack);
    printf ("n_process = before detach %d",  stack -> info -> n_processes);
            printf(" pid = %d\n", getpid());

    stack -> info -> n_processes--;
    if (stack -> info -> n_processes < 0)
        {
        printf("Stack already was destucted in detach\n");
        return -1;
        }
    for (int i = 0; i <= stack -> top_shmatted_part; i++)
        shmdt(stack -> ptr_table[i]);
    stack -> top_shmatted_part = -1;
    struct sembuf sem_unlock = {0, 1, 0};
    semop(stack -> info -> sem_id, &sem_unlock, 1);
    if (stack -> info -> n_processes == 0)
        semctl(stack -> info -> sem_id, 0, IPC_RMID);
    shmdt(stack -> info);
    
    if (errno != 0)
        return -1;
    else return 0;
    }


int mark_destruct(Stack_t* stack)
    {
    stack -> info = (Stack_info_t*) shmat(stack -> info_shmid, NULL, 0);
    printf("in mark, n_proccess = %d", stack -> info -> n_processes);
    while (stack -> info -> n_processes != 0)
        sleep(1);
        
        
        if (stack -> info == (void*) -1)
            {
            printf("stack already was destucted in mark_destruct\n");
            free(stack);
            return 0;
            }
        printf ("top_part in destruct = %d", stack -> info -> top_part);
        for (int i = 0; i <= stack -> info -> top_part; i++)
            shmctl(stack -> info -> parts[i].shmid, IPC_RMID, NULL);
        shmdt(stack -> info);
        shmctl(stack -> info_shmid, IPC_RMID, NULL);
        free(stack);
        if (errno != 0)
            return -1;
        else return 0;
    }


int push(Stack_t* stack, data_t val) 
    {
    struct sembuf sem_lock = {0, -1, 0};
    semop(stack -> info -> sem_id, &sem_lock, 1);
    Syncronising_new_parts(stack);
    if (stack -> info -> top_part >= 0 && (stack -> info -> parts[stack -> info -> top_part].top) < (stack -> info -> parts[stack -> info -> top_part].size))
        {
        stack -> info -> parts[stack -> info -> top_part].top++;
        stack -> ptr_table[stack -> info -> top_part][stack -> info -> parts[stack -> info -> top_part].top] = val;
        }
    else 
        {
        stack -> info -> top_part++;
        stack -> info -> parts[stack -> info -> top_part].shmid = shmget(stack -> info -> top_part + 'a', array_start_size * sizeof(data_t), IPC_CREAT | 0666);
        stack -> ptr_table[stack -> info -> top_part] = shmat(stack -> info -> parts[stack -> info -> top_part].shmid, NULL, 0);
        stack -> top_shmatted_part = stack -> info -> top_part;
        stack -> info -> parts[stack -> info -> top_part].size  = array_start_size;
        stack -> info -> parts[stack -> info -> top_part].top   = 0;
        stack -> ptr_table[stack -> info -> top_part][0] = val; 
        //printf("top+part = %d, stack -> info -> parts[%d] = %p\n", stack -> info -> top_part, stack -> info -> top_part, stack -> info -> parts[stack -> info -> top_part].array);  
        }
    Syncronising_new_parts(stack);
    struct sembuf sem_unlock = {0, 1, 0};
    semop(stack -> info -> sem_id, &sem_unlock, 1);
    if (errno != 0)
        return -1;
    else return 0;
    }

int pop(Stack_t* stack, data_t* val)
    {
    struct sembuf sem_lock = {0, -1, 0};
    semop(stack -> info -> sem_id, &sem_lock, 1);
    Syncronising_new_parts(stack);
    if (stack -> info -> top_part >= 0)
        {
        printf("top_part = %d, top = %d\n", stack -> info -> top_part, stack -> info -> parts[stack -> info -> top_part].top);
        *val = stack -> ptr_table[stack -> info -> top_part][stack -> info -> parts[stack -> info -> top_part].top];
        (stack -> info -> parts[stack -> info -> top_part]).top--;
        if ((stack -> info -> parts[stack -> info -> top_part]).top == -1)
            {
            shmdt(stack -> ptr_table[stack -> info -> top_part]);
            shmctl(stack -> info -> parts[stack -> info -> top_part].shmid, IPC_RMID, NULL);
            stack -> top_shmatted_part--;
            stack -> info -> top_part--;
            }
        }
    else
        {
        printf("Stack is empty\n");
        }
    Syncronising_new_parts(stack);
    struct sembuf sem_unlock = {0, 1, 0};
    //printf("top_part = %d, top_shmatted_part = %d, top = %d\n", stack -> info -> top_part, stack -> top_shmatted_part, stack -> info -> parts[stack -> info -> top_part].top);
    semop(stack -> info -> sem_id, &sem_unlock, 1);
    if (errno != 0)
        return -1;
    else return 0;
    }


void stack_dump(Stack_t* stack)
    {
    printf("stack = %p\nparts = %p\ntop_part = %d\nn_proccess = %d\n", stack, stack -> info -> parts, stack -> info -> top_part, stack -> info -> n_processes);
    if (stack -> info -> top_part != -1) 
        {
        printf("parts[%d].shmid = %d, size = %d\n", stack -> info -> top_part, stack -> info -> parts[stack -> info -> top_part].shmid, stack -> info -> parts[stack -> info -> top_part].size);
        for (int i = 0; i <= stack -> info -> parts[stack -> info -> top_part].top; i++)
            {
            printf("stack -> info -> parts[%d].array[%d] = %d\n", stack -> info -> top_part, i, stack -> ptr_table[stack -> info -> top_part][i]);
            }
        }
    }


int get_size(Stack_t* stack)
    {
    int size = 0;
    for (int i = 0; i <= stack -> info -> parts_max_size; i++)
        {
        size += stack -> info -> parts[i].size;
        }
    return size;
    }


int get_count(Stack_t* stack)
    {
    int curr_size = 0;
    for (int i = 0; i <= stack -> info -> top_part; i++)
        curr_size += stack -> info -> parts[i].top;
    return curr_size;
    }


// The function shmats new parts, that was added by another proccesses
void Syncronising_new_parts(Stack_t* stack)
    {
    for (int i = stack -> top_shmatted_part + 1; i <= stack -> info -> top_part; i++)
        {
        stack -> ptr_table[i] = shmat(stack -> info -> parts[i].shmid, NULL, 0);
        stack -> top_shmatted_part++;
        }
    }
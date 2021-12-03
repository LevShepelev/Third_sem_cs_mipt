#include "Shared_stack.h"

void sem_check(Stack_t* stack);
const int SEM_KEY = 20;
const int SEM_OPEN = 30;
struct sembuf sem_lock = {0, -1, 0};
struct sembuf sem_unlock = {0, 1, 0};

Stack_t* attach_stack(key_t key, int size)
    {
    //printf("In attach\n");
    Stack_t* stack = (Stack_t*) calloc(1, sizeof(Stack_t));
    int shmid_unique_checker = shmget(key, sizeof(Stack_info_t), IPC_CREAT | 0666 | IPC_EXCL);
    int shmid = shmget(key, sizeof(Stack_info_t), IPC_CREAT | 0666);
    int sem_open = semget(SEM_OPEN, 1, IPC_CREAT | 0666 | IPC_EXCL);
    if (sem_open != -1)
        semctl(sem_open, 0, SETVAL, 1);
    else  
        sem_open = semget(SEM_OPEN, 1, IPC_CREAT | 0666);
    stack -> info_shmid = shmid;
    stack -> info = (Stack_info_t*) shmat(shmid, NULL, 0);
    if (shmid_unique_checker != -1)
        {
        semop(sem_open, &sem_lock, 1);
        stack -> info -> sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
        semctl(stack -> info -> sem_id, 0, SETVAL, 1);
        //printf("sem_value_after_opening = %d\n", semctl(stack -> info -> sem_id, 0, GETVAL));
        printf("Creating\n");
        for (int i = 0; i < Max_proccess; i++)
            stack -> info -> pids[i] = 0;
        stack -> info -> top = -1;
        stack -> info -> n_processes = 0;
        stack -> info -> size = size;
        stack -> info -> array_shmid = shmget('a', size, IPC_CREAT | 0666);
        semop(sem_open, &sem_unlock, 1);
        }
    for (int i = 0; i < Max_proccess; i++)
            {
            if (i == Max_proccess)
                {
                printf("Too many proccesses\n");
                exit (1);
                }
            if (stack -> info -> pids[i] == 0)
                {
                stack -> info -> pids[i] = getpid();
                break;
                }
            }
    semop(sem_open, &sem_lock, 1);
    semop(sem_open, &sem_unlock, 1);
    stack -> array = shmat(stack -> info -> array_shmid, NULL, 0);
    stack -> info -> n_processes++;
    if (shmid == -1 || stack -> info -> sem_id == -1)
        return NULL;
    return stack;
    }

int push(Stack_t* stack, data_t val) 
    {
    sem_check(stack);

    semop(stack -> info -> sem_id, &sem_lock, 1);
    //printf("lock_push, pid = %d\n", getpid());
    //printf ("size = %d, top = %d, push %d\n", stack -> info -> size, stack -> info -> top, val);
    if (stack -> info -> top < stack -> info -> size - 1)
        {
        stack -> info -> top++;
        stack -> array[stack -> info -> top] = val;
        }
    else 
        printf("There's no free space in stack, please free space\n");
    //printf("unlock_push, pid = %d\n", getpid());
    //usleep(1);
    semop(stack -> info -> sem_id, &sem_unlock, 1);
    if (errno != 0)
        return -1;
    else return 0;
    }

int pop(Stack_t* stack, data_t* val)
    {
    sem_check(stack);

    semop(stack -> info -> sem_id, &sem_lock, 1);
    //printf("lock_pop, pid = %d\n", getpid());
    if (stack -> info -> top >= 0)
        {
        *val = stack -> array[stack -> info -> top];
        stack -> info -> top--;
        }
    else
        printf("Stack is empty\n");
    //printf("top_part = %d, top_shmatted_part = %d, top = %d\n", stack -> info -> top_part, stack -> top_shmatted_part, stack -> info -> parts[stack -> info -> top_part].top);
    //printf("unlock_pop, pid = %d\n", getpid());
    //usleep(1);
    semop(stack -> info -> sem_id, &sem_unlock, 1);
    if (errno != 0)
        return -1;
    else return 0;
    }

int get_size(Stack_t* stack)
    {
    return stack -> info -> size;
    }

int get_count(Stack_t* stack)
    {
    return stack -> info -> top + 1;
    }

int detach_stack(Stack_t* stack)
    {
    sem_check(stack);
    //printf("lock_det\n");
    stack -> info -> n_processes--;
    //printf("proccess = %d\n", stack -> info -> n_processes);
    //printf("unlock_det\n");
    if (stack -> info -> n_processes < 0)
        {
        //printf("Stack already was destucted\n");
        return -1;
        }
    for (int i = 0; i < Max_proccess; i++)
        {
        if (stack -> info -> pids[i] == getpid())
            stack -> info -> pids[i] = 0;
        }
    shmdt(stack -> info);
    if (errno != 0)
        return -1;
    else return 0;
    }

int mark_destruct(Stack_t* stack)
    {
    //printf("destructor\n");
    assert(stack != NULL);
    stack -> info = (Stack_info_t*) shmat(stack -> info_shmid, NULL, 0);
    sem_check(stack);
    while (stack -> info -> n_processes > 0)
        {
       // printf("\nIn mark, n_proccess = %d, sem = %d\n", stack -> info -> n_processes, semctl(stack -> info -> sem_id, 0, GETVAL));
        Check_proccesses(stack);
        usleep(1);
        }
    
    if (stack -> info == (void*) -1) //man return value shmat
        {
        //printf("stack alreadd was destucted in mark_destruct\n");
        free(stack);
        return 0;
        }
    struct shmid_ds buf;
    int sem_open = semget(SEM_OPEN, 1, IPC_CREAT | 0666);
    semctl(sem_open, 0, IPC_RMID);
    semctl(stack -> info -> sem_id, 0, IPC_RMID);
    shmctl(stack -> info -> array_shmid, IPC_RMID, &buf);
    shmdt(stack -> info);
    shmctl(stack -> info_shmid, IPC_RMID, NULL);
    free(stack);
    printf("free success\n");
    if (errno != 0)
        return -1;
    else return 0;
    }

void sem_check(Stack_t* stack)
    {
    //printf("in sem_check, sem = %d\n", semctl(stack -> info -> sem_id, 0, GETVAL));
    if (semctl(stack -> info -> sem_id, 0, GETVAL) > 1) 
        {
        printf("sem_problem\n");
        exit(1);
        }
    }

void Check_proccesses(Stack_t* stack)
    {
    semop(stack -> info -> sem_id, &sem_lock, 1);
    for (int i = 0; i < Max_proccess; i++)
        {
        if (stack -> info -> pids[i] != 0)
            if (kill(stack -> info -> pids[i], 0) == 0)
                {
                stack -> info -> n_processes--;
                //printf("Proccess with pid = %d, was killed without detaching from stack\n", stack -> info -> pids[i]);
                stack -> info -> pids[i] = 0;
                semop(stack -> info -> sem_id, &sem_unlock, 1);
                }
        }
    semop(stack -> info -> sem_id, &sem_unlock, 1);
    }
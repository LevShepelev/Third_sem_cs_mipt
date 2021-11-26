#pragma once

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
#include <errno.h>
#include <stdatomic.h>

#define Max_proccess 65536

typedef int data_t;

typedef struct stach_info_t
    {
    int size;
    int top;
    int sem_id;
    int n_processes;
    int array_shmid;
    int pids[Max_proccess];
    } Stack_info_t;

typedef struct stack_t 
    {
    Stack_info_t* info;
    data_t* array;
    int info_shmid;
    } Stack_t;



/* Attach (create if needed) shared memory stack to the process.
Returns Stack_t* in case of success. Returns NULL on failure. */
Stack_t* attach_stack(key_t key, int size);

/* Detaches existing stack from process. 
Operations on detached stack are not permitted since stack pointer becomes invalid. */
int detach_stack(Stack_t* stack);

/* Marks stack to be destroed. Destruction are done after all detaches */ 
int mark_destruct(Stack_t* stack);

/* Returns stack maximum size. */
int get_size(Stack_t* stack);

/* Returns current stack size. */
int get_count(Stack_t* stack);

/* Push val into stack. */
int push(Stack_t* stack, data_t);

/* Pop val from stack into memory */
int pop(Stack_t* stack, data_t*);

void Check_proccesses(Stack_t* stack);
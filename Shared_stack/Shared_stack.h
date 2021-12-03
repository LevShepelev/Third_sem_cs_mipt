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

#define Max_number_of_parts 100

typedef int data_t;

typedef struct stack_elem_t 
    {
    int size;
    int top;
    int shmid;
    } St_elem_t;

typedef struct stach_info_t
    {
    St_elem_t parts[Max_number_of_parts];
    int parts_max_size;
    int top_part;
    int sem_id;
    int n_processes;
    } Stack_info_t;

typedef struct stack_t 
    {
    Stack_info_t* info;
    data_t* ptr_table[Max_number_of_parts];
    int info_shmid;
    int top_shmatted_part;
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

/* Print information about stack */
void stack_dump(Stack_t* stack);

/* The function shmats new parts, that was added by another proccesses */
void Syncronising_new_parts(Stack_t* stack);

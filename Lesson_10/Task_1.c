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
#include <sys/msg.h>
#include <sys/mman.h>
#include <pthread.h>
#include <math.h>


void* count (void* data);

double sum = 0, delta;
int n_threads = 0, n_points = 0;
double edges[2];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char** argv)
    {
    pthread_mutex_init(&mutex, NULL);
    n_threads = atoi(argv[1]);
    n_points  = atoi(argv[2]);
    pthread_t* table = calloc(n_threads, sizeof(pthread_t));
    edges[0] = M_PI / 4;
    edges[1] = M_PI / 2;
    delta = (edges[1] - edges[0]) / (n_points - 1);
    int data[2];
    for (int i = 0; i < n_threads; i++)
        {
        data[0] = i * n_points / n_threads;
        data[1] = (i + 1) * n_points / n_threads;
        //printf("data[0] = %d, data[1] = %d\n", data[0], data[1]);
        pthread_create(table + i, NULL, count, (void*) data);
        }

    for (int i = 0; i < n_threads; i++)
        pthread_join(table[i], NULL);
        
    printf("Integral = %.10lf\n", sum);
    free(table);
    }

void* count (void* data)
    {
    int i  = *(int*) data;
    int index_finish = *(int*) ( data + sizeof(int));
    for (; i < index_finish; i++) 
        {
        pthread_mutex_lock(&mutex);
        sum += (edges[0] + delta * i) / sin(edges[0] + delta * i) * delta;
        // printf("point = %.10lf, d_sum = %.10lf,  sum = %.10lf\n", (edges[0] + i * delta), (edges[0] + i * delta) / sin((edges[0] + i * delta)) * delta, sum);
        pthread_mutex_unlock(&mutex);
        }
    return NULL;
    }
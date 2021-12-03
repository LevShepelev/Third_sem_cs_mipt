#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/wait.h>
#include <assert.h>
#include <fcntl.h>
#include <signal.h>

int main()
    {
    int action = 0, pid = 0;
    int prev_delay;
    printf("enter pid of daemon\n");
    scanf("%d", &pid);
    FILE* config = fopen("copydaemon.cfg", "r");
    fscanf(config, "%d", &prev_delay);
    fclose(config);
    printf("To finish daemon press 1\nTo change delay press 2\n");
    scanf("%d", &action);
    if (action == 1)
        {
        if (kill(pid, SIGINT) == -1)
            printf("no such proccess\n");
        }
    else if (action == 2)
        {
        printf("Please write new delay in seconds\n");
        scanf("%d", &prev_delay);
        config = fopen("copydaemon.cfg", "w");
        fprintf(config, "%d", prev_delay);
        if (kill(pid, SIGUSR1) == -1)
            printf("no such proccess\n");
        }        
    else 
        {
        printf("input_error\n");
        exit(1);
        }
    
    }
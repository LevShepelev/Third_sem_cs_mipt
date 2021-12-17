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

int main(int argc, char** argv)
    {
    int action = 0, pid = 0;
    int prev_delay;
    char name[4096], old_source[4096] = {'\0'}, old_dest[4096] = {'\0'};
    pid = atoi(argv[1]);
    FILE* config = fopen("copydaemon.cfg", "r");
    fscanf(config, "%d", &prev_delay);
    fclose(config);
    if (kill(pid, 0) == -1)
        {
        printf("no such proccess\n");
        exit(1);
        }
    printf("To finish daemon press 1\nTo change delay press 2\nTo change source press 3\nTo change destination press 4\nTo change mode classic or intofy press 5\nTo stop demon press 6\nTo wake up Demon press 7\n");
    scanf("%d", &action);
    if (action == 1)
        kill(pid, SIGTERM);
        
    else if (action == 2)
        {
        printf("Please write new delay in seconds\n");
        scanf("%d", &prev_delay);
        config = fopen("copydaemon.cfg", "r");
        fscanf(config, "%d%s%s", &prev_delay, old_source, old_dest);
        fclose(config);
        config = fopen("copydaemon.cfg", "w");
        fprintf(config, "%d\n%s\n%s", prev_delay, old_source, old_dest);
        kill(pid, SIGUSR1);
        }
    else if (action == 3)
        {
        printf("Please write new source dir\n");
        scanf("%s", name);
        config = fopen("copydaemon.cfg", "r");
        fscanf(config, "%d%s%s", &prev_delay, old_source, old_dest);
        fclose(config);
        config = fopen("copydaemon.cfg", "w");
        fprintf(config, "%d\n%s\n%s", prev_delay, name, old_dest);
        kill(pid, SIGUSR1);
        }
    else if (action == 4)
        {
        printf("Please write new dest dir\n");
        scanf("%s", name);
        config = fopen("copydaemon.cfg", "r");
        fscanf(config, "%d%s%s", &prev_delay, old_source, old_dest);
        fclose(config);
        config = fopen("copydaemon.cfg", "w");
        fprintf(config, "%d\n%s\n%s", prev_delay, old_source, name);
        kill(pid, SIGUSR1);
        }      
    else if (action == 5)
        kill(pid, SIGUSR2);
    else if (action == 6)
        kill(pid, SIGSTOP);
    else if (action == 7)
        kill(pid, SIGCONT); 
    else 
        {
        printf("input_error\n");
        exit(1);
        }
    
    }
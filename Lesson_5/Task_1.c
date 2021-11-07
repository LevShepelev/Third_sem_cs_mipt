#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

int main(int argc, char** argv)
    {
    char* arr[4] = {"grep", "A", NULL};
    int wstatus = 0;
    
    int fork_code = fork();
    if (fork_code == 0)
        {
        printf("%s, %s, %s\n", arr[0], arr[1], arr[2]);
        for (int i = 0; arr[i] != NULL; i++)
            {
            printf("%s\n", arr[i]);
            }
        execvp("grep", arr);
        }
    sleep(2);
    waitpid(fork_code,  &wstatus, 0);
    printf("done\n");
    }
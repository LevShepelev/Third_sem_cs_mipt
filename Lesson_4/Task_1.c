#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>


int main()
    {
    char buf[100] = "";
    int pid = 0, file_size = 0;
    int wstatus = 0;
    pid_t fork_code = 0;
    int return_status = 0;
    int file_destination = open("pid-out.txt", O_WRONLY | O_CREAT | O_TRUNC);
    if (file_destination == -1)
        perror("open_destination");
    for (int i = 0 ; i < 1024; i++)
        {
        fork_code = fork();
        if (fork_code == 0)
            {
            int size = sprintf(buf, "%d %d\n", getpid(), getppid());
            write(file_destination, buf, size);
            return 0;
            }

        else if (fork_code > 0)
            {
            return_status = waitpid(fork_code, &wstatus, 0);
            if (WIFEXITED(wstatus))
                {
                int size = sprintf(buf, "parent %d, %d, %d\n", getpid(), fork_code, WEXITSTATUS(wstatus));
                write(file_destination, buf, size);
                }
            }
        else exit (1);
        }
    close(file_destination);
    return 0;
    }
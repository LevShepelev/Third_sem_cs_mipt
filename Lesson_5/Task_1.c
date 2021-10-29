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
    int file_destination = open(argv[1], O_WRONLY | O_CREAT );
    if (file_destination == -1)
        exit(-1);
    printf("fuck2\n");
    dup2(file_destination, STDOUT_FILENO);
    printf("fuck\n");
    execvp(argv[2], (argv + 2));
    perror("execvp");
    }
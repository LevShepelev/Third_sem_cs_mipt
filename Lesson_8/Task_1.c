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

int main(int argc, char **argv) 
    {
    int file_in = open(argv[1], O_RDONLY);
    if (argc != 3)
        {
        printf("Too few arguments");
        return EXIT_FAILURE;
        }

    if (file_in == -1)
        {
        printf("No such file: %s", argv[1]);
        return EXIT_FAILURE;
        }

    int file_out = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (file_out == -1)
        {
        printf("Cant create file_out\n");
        exit(1);
        }
    struct stat statistica;
    int stat_error = stat (argv[1], &statistica);
    assert(stat_error == 0);

    char* sourse = mmap(0, statistica.st_size, PROT_READ, MAP_SHARED, file_in, 0);
    lseek(file_out, statistica.st_size - 1, SEEK_SET);
    write(file_out, "", 1);
    char* destination = mmap(0, statistica.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, file_out, 0);
    memcpy(destination, sourse, statistica.st_size);
    
    close(file_in);
    close(file_out);
    return 0;
    }
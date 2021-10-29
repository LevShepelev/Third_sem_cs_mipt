#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
int main (int argc, char **argv)
    {
    const int buf_size = 100;
    char buf[buf_size];
    int file_source = open(argv[1], O_RDONLY);
    int file_destination = open(argv[2], O_WRONLY);
    if (file_source == -1)
        perror("open_source\n");
    if (file_destination == -1)
        perror("open_destination\n");

    struct stat statistica;
    int stat_error = stat (argv[1], &statistica);
    assert(stat_error == 0);

    for (int i = 0; i < statistica.st_size / buf_size + 1; i++)
        {
        int letters = read(file_source, buf, buf_size);
        if (write(file_destination, buf, letters) != letters)
            perror("write error\n");
        }
    
    close(file_source);
    close(file_destination);
    return 0;
    }
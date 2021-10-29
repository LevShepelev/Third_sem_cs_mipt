#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/poll.h>
void reading_and_printing(int file_source, const char* file_name);


int main(int argc, char** argv)
    {
    struct pollfd files[argc - 1];
    int files_size = argc - 1;
    printf("file_size = %d\n", files_size);
    for (int i = 0; i < files_size; i++)
        {
        int file_descr = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        if (file_descr != -1)
            {
            files[i].fd = file_descr;
            files[i].events = POLLIN;
            }
        else printf("file wasnt open %s\n", argv[i + 1]);
        }
    while (1)
        {
        poll(files, files_size, 0);
        for (int i = 0; i < files_size; i++)
            {
            if (files[i].revents == files[i].events)
                 reading_and_printing(files[i].fd, argv[i + 1]);
            }
        }
    }

void reading_and_printing(int file_source, const char* file_name)
    {
    const int buf_size = 100;
    char buf[buf_size];

    struct stat statistica;
    int stat_error = stat (file_name, &statistica);
    assert(stat_error == 0);

    for (int i = 0; i < statistica.st_size / buf_size + 1; i++)
        {
        int letters = read(file_source, buf, buf_size);
        for (int i = 0; i < letters; i++)
            putchar(buf[i]);
        }
    }
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
int main()
    {
    char c[100000];
    int file = open("big_data_file.txt", O_WRONLY);
    perror("open");
    for (int i = 0; i < 100000; i++)
        c[i] = 44 + i % 10;
    for (int i = 0; i < 20000; i++)
        {
        write(file, c, 100000);

        }
    close(file);
    }
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

int main()
    {
    int temp = 10;
    for (int i = 0; i < 100; i++)
        {
        scanf("%d", &temp);
        printf("%d\n", temp);
        }
    return 0;
    }
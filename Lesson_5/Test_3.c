#include <stdio.h>

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
    int i = 10;
    for (i = 0; i < 1000; i++)
        {
        printf("%d\n", i);
        }
    return 0;
    }
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
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
#include <pthread.h>
#include <math.h>

void my_printf(char* str, ...);

int main()
    {
    int a = 5;
    my_printf("%d %c %d, %s", a, 'A', 6, "priv\n");
    }

void my_printf(char* str, ...)
    {
    int int_value;
    char ch_value;
    char* str_value;
    char out[100];
    va_list op;
    va_start(op, str);
    int i = 0;
    while (str[i] != '\0')
        {
        if (str[i] == '%')
            {
            i++;
            if (str[i] == 'd')
                {
                int_value = va_arg(op, int);
                snprintf(out, 10, "%d", int_value);
                write(STDOUT_FILENO, out, strlen(out));
                }
            else if (str[i] == 'c')
                {
                ch_value = va_arg(op, int);
                write(STDOUT_FILENO, &ch_value, 1);
                }
            else if (str[i] == 's')
                {
                str_value = va_arg(op, char*);
                write(STDOUT_FILENO, str_value, strlen(str_value));
                }
            else printf("\nerror\n");
            }
        else write(STDOUT_FILENO, &(str[i]), 1);
        i++;
        }
    va_end(op);
    }
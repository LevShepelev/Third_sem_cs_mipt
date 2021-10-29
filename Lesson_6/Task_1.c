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
void handler_2(int signum);
void handler_1(int signum);

int main()
    {
    int mask = 0;
    int sa_flags;
    struct sigaction act_1 = {handler_1, NULL, mask, sa_flags, NULL};
    struct sigaction act_2 = {handler_2, NULL, mask, sa_flags, NULL};
    struct sigaction oldact;
    printf("%d\n", getpid());
    sigaction(SIGUSR1, &act_1, &oldact);
    sigaction(SIGUSR2, &act_2, &oldact);
    while (1);
    }

void handler_1(int signum)
    {
    char s[10] = "London ";
    write(STDOUT_FILENO, s, 10);
    }

void handler_2(int signum)
    {
    char s[100] = "is the capital of Great Britain\n";
    write(STDOUT_FILENO, s, 100);
    }

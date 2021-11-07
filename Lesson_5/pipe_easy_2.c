#define _GNU_SOURCE

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

struct str{char* s; int l;};
int linecount(const char* buf, const int value);
void filltext(char* buf, struct str* text, const int numb);
void length_of_strings(const char* buf, struct str* text, const int numb, int value);
char* starting(int* numb, struct str** text, const char* file_name, int* big_buf_size);
void finishing(int pipefd[][2], char* buf, struct str* text, int pipe_counter);
int fill_command(char* command[], int numb, struct str* text, int pipefd[][2], int word_counter, int out, int* pipe_counter, int log_file);

int main(int argc, char** argv)
    {
    struct sigaction sig;
    sig.sa_handler = SIG_IGN;
    sigaction(SIGTTIN, &sig, NULL); 
    int log_file = open("log_file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    int out = dup(STDOUT_FILENO);
    int numb, big_buf_size = 0;
    struct str* text = NULL;
    char* buf = starting(&numb, &text, argv[1], &big_buf_size);
    for (int i = 0; i <  numb; i++)
        printf("\"%s\"\n", text[i].s);
    printf("numb = %d\n", numb);
    int pipe_counter = 0;
    int pipefd[numb][2];
    pipe2(pipefd[pipe_counter], 0);
    char* command[numb + 1];
    int word_counter = 0, wstatus = 0;

    word_counter = fill_command(command, numb, text, pipefd, word_counter, out, &pipe_counter, log_file);
    if (word_counter >= numb)
        {
        close(log_file);
        finishing(pipefd, buf, text, pipe_counter);
        return 0;
        }
    int fork_code = fork();
    if (fork_code == 0)
        {
        execvp(command[0], command);
        return 0;
        perror("exec_problems\n");
        } 
    else 
        {
        while (word_counter <= numb)
            {
            dup2(pipefd[pipe_counter][0], STDIN_FILENO);
            word_counter = fill_command(command, numb, text, pipefd, word_counter, out, &pipe_counter, log_file);
            if (word_counter < numb)
                {
                pipe_counter++;
                pipe2(pipefd[pipe_counter], O_CLOEXEC);
                fork_code = fork();
                if (fork_code == 0)
                    {
                    dup2(pipefd[pipe_counter][1], STDOUT_FILENO);
                    execvp(command[0], command);
                    write(log_file, "wait has ended in while\n", 50);
                    return 0;
                    perror("exec_porblems\n");
                    }
                }
            }
        waitpid(fork_code, &wstatus, 0);
        close(log_file);
        finishing(pipefd, buf, text, pipe_counter);
        return 0;
        }
    }    

int fill_command(char* command[], int numb, struct str* text, int pipefd[][2], int word_counter, int out, int* pipe_counter, int log_file)
    {
            char write_buf[100] = {'\0'};

    for (int i = 0; i <= numb; i++)
        command[i] = NULL;
        
    int word_counter_last = word_counter;
    for (; word_counter < numb && strcmp(text[word_counter].s, "|") != 0; word_counter++)
        {
        command[word_counter - word_counter_last] = text[word_counter].s; 
        sprintf(write_buf, "word_counter = %d, text[word_counter] = %s\n", word_counter, text[word_counter].s);
        write(out, write_buf, sizeof(write_buf) + 1);
        }
    
        
    if (word_counter >= numb)
        {
        int wstatus = 0;
        for (int i = 0; command[i] != NULL; i++)
            {
            printf("start\n");
            printf("\"%s\"\n", command[i]);
            printf("end\n");
            }
        
        int fork_code = fork();
        if (fork_code == 0)
            {
            printf("%s, %s, %s\n", command[0], command[1], command[2]);
            dup2(out, pipefd[*pipe_counter][1]);
            execvp(command[0], command);
            }
        waitpid(fork_code, &wstatus, 0);
        }
    word_counter++; //We have stopped on | 
    return word_counter;
    }


void finishing(int pipefd[][2], char* buf, struct str* text, int pipe_counter)
    {
    for (int i = 0; i < pipe_counter; i++)
        {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
        }
    free(text);
    free(buf);
    }

/// \brief The function count number of lines in the text
int linecount(const char* buf, const int value)
    {
    int linecounter = 0;

    for (int i = 0; i < value; i++)  // считаем сколько строк
        if (buf[i] == '\n' || buf[i] == ' ')
            linecounter++;
    return linecounter;
    }



/// \brief The function fill array of struct from bufer
void filltext(char* buf, struct str* text, const int numb)
    {
    int j = 0;
    int i = 0;

    for (i = 0; i < numb; i++)
        {
        text[i].s = buf + j;
        text[i].s[text[i].l] = '\0';
        j += text[i].l + 1;
        }
    }



/// \brief The function count length of each string in text
void length_of_strings(const char* buf, struct str* text, const int numb, int value)
    {
    for (int i = 0; i < numb; i++)
        for (int j = 0; j < value - 2; j++)
            if ((buf[j] == '\n') || (buf[j] == '\0') || buf[j] == ' ')
                {
                text[i].l = j;
                buf += j + 1;
                break;
                }
    }

char* starting(int* numb, struct str** text, const char* file_name, int* big_buf_size)
    {
    struct stat statistica;
    int stat_error = stat (file_name, &statistica);
    assert(stat_error == 0);
    int file_in = open(file_name, O_RDONLY);
    if (file_in == -1)
        exit(-1);
    char* buf = (char*) calloc (statistica.st_size, sizeof(char));
    *big_buf_size = read(file_in, buf, statistica.st_size);
    *numb = linecount(buf, statistica.st_size);
    *text = (struct str*) calloc (*numb, sizeof(struct str));
    length_of_strings(buf, *text, *numb, statistica.st_size);
    filltext(buf, *text, *numb);
    return buf;
    }


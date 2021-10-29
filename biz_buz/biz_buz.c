#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
const int buf_size = 1024;

int main (int argc, char **argv)
    {
    int file_source = open(argv[1], O_RDONLY);
    int file_destination = open(argv[2], O_WRONLY);
    if (file_source == -1)
        perror("open_source");
    if (file_destination == -1)
        perror("open_destination");
    
    }

void analysing(int file_source, int file_destination)
    {
    int s = 0, last_word = 0;
    int is_reading_continuing = 0;
    int carriage = 0;
    char buf[buf_size];
    while (full_read)
        {
        symb_was_red = read(file_source, buf, buf_size);
        int full_read = (symb_was_red == buf_size);
        bufer_handling(bufer, symb_was_red, file_destination, &carriage, &is_reading_continuing, &s, &last_word);
        }
    }

void bufer_handling(char* buf, int size, int file_destination, int* carriage, int* is_reading_continuing, int* s, int* last_word)
    {
    int returned_size = 0;
    for (int j = 0; j < size; j++)
        {
        if (buf[j] == ' ' || buf[j] == '\n' || buf[j] == EOF || buf[j] == '\0')
            {
            if (*is_reading_continuing)
                {
                int tmp = Check_mod_3(buf + (*last_word) * sizeof(char), buf_size - (*last_word))
                }
            else
                {
                Number_checker(buf + (*last_word) * sizeof(char), j - (*last_word), &returned_size);
                if (write(file_destination, buf + (*last_word) * sizeof(char), returned_size) == returned_size)
                    (*carriage) += returned_size;
                else exit(1);
                (*last_word) = j + 1;
                }
            }
        }
    if (!(buf[buf_size - 1] == ' ' || buf[buf_size - 1] == '\n' || buf[buf_size - 1] == EOF || buf[buf_size - 1] == '\0'))
        {
        int tmp = Check_mod_3(buf + (*last_word) * sizeof(char), buf_size - (*last_word))
        if (tmp != 4)
            {
            (*is_reading_continuing) = 1;
            s += tmp;
            s %= 3;
            }
        if (write(file_destination, buf + (*last_word) * sizeof(char), buf_size - (*last_word)) == buf_size - (*last_word))
            (*carriage) += buf_size - (*last_word);
        else exit(2);
        }
    }

void skip_spaces(char* buf, int size, int* j)
    {
    while ((buf[*j] == ' ' || buf[*j] == '\n') && (*j) < size)
        (*j)++; 
    }

int Check_mod_3(char* word, int size)
    {
    if (word[0] == '-' || (word[0] >= '0' && word[0] <= '9'))
        {
        int s = 0; 
        for (int i = 0; i < size; i++)
            {
            if (!(word[i] >= '0' && word[i] <= '9'))
                {
                printf("%c not a number\n", word[i]);
                return 4;
                }
            s += word[i] - '0';
            s %= 3;
            }
        return s;
        }
    else return 4;
    }

char* Number_checker(char* word, int size, int* returned_size)
    {
    for (int i = 0; i < size; i++)
        putchar(word[i]);
    putchar('\n');

    *returned_size = size;
    if (word[0] == '-' || (word[0] >= '0' && word[0] <= '9'))
        {
        int s = 0; 
        for (int i = 0; i < size; i++)
            {
            if (!(word[i] >= '0' && word[i] <= '9'))
                {
                printf("%c not a number\n", word[i]);
                return word;
                }
            s += word[i] - '0';
            s %= 3;
            }
        if (s == 0 && word[size - 1] == '5')
            {
            *returned_size = 8;
            return "bis_bus ";
            }
        else if (s == 0)
            {
            *returned_size = 4;
            return "bis ";
            }
        else if (word[size - 1] == '5')
            {
            *returned_size = 4;
            return "bus ";
            } 
        }
    else 
        return word;
    }

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

typedef struct flags {
            int carriage;
            int begin_of_last_word;
            int checking_mod_3;
            char is_numb;
            char is_first_letter;
            char prev_letter;
            } Flags;
const long MAX_CALLOC_SIZE = 1;

void reading_little_files(int file_source, int file_destination, int size);            
void letter_handler(char letter, Flags* flags, int file_dest);
void easy_write(int file_dest, char* buf, int size, int* carriage);
void handling_ending_the_word(int file_dest, Flags* flags);
void reading_big_files(int file_source, int file_dest, int file_size);
char* Number_checker(char* word, int size, int* returned_size, int* is_numb);
char* analysing_for_little_files(char* str, long long size, int file_descr);



int main(int argc, char** argv)
    {
    clock_t start, end;
    start = clock();
    int file_source = open(argv[1], O_RDONLY | O_CREAT);
    int file_destination = open(argv[2], O_WRONLY | O_CREAT);
    if (file_source == -1)
        perror("open_source");
    if (file_destination == -1)
        perror("open_destination");
    struct stat statistica;
    int stat_error = stat (argv[1], &statistica);
    assert(stat_error == 0);
    printf("SIZE = %lu\n", statistica.st_size);
    
    if (statistica.st_size > MAX_CALLOC_SIZE)
        reading_big_files(file_source, file_destination, statistica.st_size);//slow version for big files
    else 
        reading_little_files(file_source, file_destination, statistica.st_size);

    close(file_source);
    close(file_destination);
    end = clock();
    printf("Well done, time = %lf\n", (float) (end - start) / (CLOCKS_PER_SEC));
    }


void reading_little_files(int file_source, int file_destination, int size)
    {
    char* buf = (char*) calloc (size + 1, sizeof(char));
    assert(buf != NULL);

    if (read(file_source, buf, size) < 0)
        perror("read");
    analysing_for_little_files(buf, size, file_destination);
    free(buf);
    }


void reading_big_files(int file_source, int file_dest, int file_size)
    {
    Flags flags = {0, 0, 0, 1, 1, 0};
    char letter;
    for (int i = 0; i < file_size; i++)
        {
        flags.prev_letter = letter;
        if (!read(file_source, &letter, 1))
            perror("reading error");
        letter_handler(letter, &flags, file_dest);
        easy_write(file_dest, &letter, 1, &(flags.carriage));
        }
    if (letter != ' ' && letter != '\n') handling_ending_the_word(file_dest, &flags);
    ftruncate(file_dest, flags.carriage);
    }


void letter_handler(char letter, Flags* flags, int file_dest)
    {
    if (letter == ' ' || letter == '\n' || letter == EOF || letter == '\0') 
            handling_ending_the_word(file_dest, flags);
    else if (flags -> is_numb)
        {
        if (flags -> is_first_letter && letter == '-')
            flags -> is_numb = 1;
        else if (letter >= '0' && letter <= '9')
            {
            flags -> is_numb = 1;
            flags -> checking_mod_3 = (flags -> checking_mod_3 + (letter - '0')) % 3;
            }
        else flags -> is_numb = 0;
        }
    }


void easy_write(int file_dest, char* buf, int size, int* carriage)
    {
    if (!write(file_dest, buf, size))
        {
        perror("writing_error");
        }
    (*carriage) += size;
    }


void handling_ending_the_word(int file_dest, Flags* flags)
    {
    if (flags -> is_numb)
        {
        if ((flags -> checking_mod_3 == 0) && (flags -> prev_letter == '5' || flags -> prev_letter == '0'))
            {
            lseek(file_dest, flags -> begin_of_last_word, SEEK_SET);
            flags -> carriage = flags -> begin_of_last_word;
            easy_write(file_dest, "biz_buz", 7, &(flags -> carriage));
            }
        else if (flags -> checking_mod_3 == 0)
            {
            lseek(file_dest, flags -> begin_of_last_word, SEEK_SET);
            flags -> carriage = flags -> begin_of_last_word;
            easy_write(file_dest, "biz", 3, &(flags -> carriage));
            }
        else if (flags -> prev_letter == '5' || flags -> prev_letter == '0')
            {
            lseek(file_dest, flags -> begin_of_last_word, SEEK_SET);
            flags -> carriage = flags -> begin_of_last_word;
            easy_write(file_dest, "buz", 3, &(flags -> carriage));
            }
        }
    flags -> is_first_letter = 1;
    flags -> is_numb = 1;
    flags -> begin_of_last_word = flags -> carriage + 1;
    flags -> checking_mod_3 = 0;
    }



char* analysing_for_little_files(char* str, long long size, int file_descr)
    {
    int returned_size = 0, is_numb = 0;
    long long curr_word = 0;   
    for (int i = 0; i < size; i++)
        {
        if (str[i] == ' ')
            {
            char* word = Number_checker(str + curr_word * sizeof(char), i - curr_word, &returned_size, &is_numb);
            
            while (str[i] == ' ')
                i++;
            if (is_numb)
                write(file_descr, word, returned_size);
            else write(file_descr, str + curr_word * sizeof(char), i - curr_word);
            curr_word = i;
            }  
        }
    return 0;
    }

char* Number_checker(char* word, int size, int* returned_size, int* is_numb)
    {
    *returned_size = size;
    *is_numb = 0;
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
            *is_numb = 1; 
            return "bis_bus ";
            }
        else if (s == 0)
            {
            *is_numb = 1; 
            *returned_size = 4;
            return "bis ";
            }
        else if (word[size - 1] == '5')
            {
            *is_numb = 1; 
            *returned_size = 4;
            return "bus ";
            } 
        }
    return word;
    }
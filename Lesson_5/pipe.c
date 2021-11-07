#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>

#define bufFd 666
int mainPid;

int getFileSize(int fd);
char* file2arr(int fd, int* arrLen);
int numWords(char* arr,int lenArr);
int getWordLen(char* arr, int position, int arrLen);
int nextWord(char* arr, int position, int arrLen);
char** arr2matrix(char* arr,int lenArr, int* pSize); // Также добавляет Null перед каждой |
void printMatrix(char** matrix, int size);
int numOfBarArr(char *arr, int len); //возвращает количество |
int numOfBarMatrix(char** matrix, int size);
int lastBarArr(char *arr, int len); //Находит последний | в массиве
int lastBarMatrix(char ** matrix, int size);
void freeMatrix(char** matrix, int realSize);

void doBar(char** matrix, int size, int realSize){
    int numBar = numOfBarMatrix(matrix, size);
    if (numBar == 0) { // Значит, здесь находится какая-то программа, которую следует запустить
        execvp(matrix[0], matrix);
        printf("Command %s works  incorrect\n", matrix[0]);
        freeMatrix(matrix, realSize);
    }

    if(numBar == 1){ //
        int fd[2];
        pipe(fd);
        int forked = fork();
        if (forked == 0){ // Выполняет процесс слева от я|
            //printf("Hello there\n");
            dup2(fd[1], STDOUT_FILENO);
            execvp(matrix[0], matrix);
            // Если произошла ошибка, очищаемся и завершаем процесс
            dup2(bufFd, STDOUT_FILENO);
            printf("Command \"%s\" works incorrect\n", matrix[0]);
            //printf("ppid = %i, mainPid = %i\n", getppid(), mainPid);
            freeMatrix(matrix, realSize);
            close(fd[0]);
            close(fd[1]);
            kill(getpid(), SIGUSR1);
            exit(1);
        }
        else if(forked > 0){ // Родитель
            int status = 0;
            close(fd[1]);
            waitpid(forked, &status, 0);
            if (status == 0) { // Потомок завершился без ошибки
                int fd1[2];
                pipe(fd1);
                int barI = lastBarMatrix(matrix, size);
                dup2(fd[0], STDIN_FILENO);
                execvp((matrix[barI + 1]), matrix + barI + 1);
                dup2(bufFd, STDOUT_FILENO);
                printf("Command \"%s\" works incorrect\n", matrix[barI + 1]);
                freeMatrix(matrix, realSize);
                close(fd[0]);
                close(fd[1]);
                if (getpid() != mainPid) {
                    kill(getpid(), SIGUSR1);
                }
                exit(1);
            } else{ // Потомок завершился с ошибкой
                freeMatrix(matrix, realSize);
                close(fd[1]);
                close(fd[0]);
                if(getpid() != mainPid)
                    kill(getpid(), SIGUSR1);
                exit(1);
            }
        }
    }

    if(numBar > 1){
        int fd[2];
        pipe(fd);
        int forked = fork();
        if(forked == 0){ // рекурсивно выполняем что слева от последнего |
            dup2(fd[1], STDOUT_FILENO);
            int barI = lastBarMatrix(matrix, size);
            doBar(matrix, barI, realSize); // в хорошем случае подменяется процессом команды
            kill(getpid(), SIGUSR1);
            exit(1);
        }else { // выполняет что справа от последнего |
            int status = 0;
            close(fd[1]);
            waitpid(forked, &status, 0);
            if (status == 0){ // Потомок завеошился без ошибок
                int fd1[2];
                pipe(fd1);
                int barI = lastBarMatrix(matrix, size);
                dup2(fd[0], STDIN_FILENO);
                execvp((matrix[barI + 1]), matrix + barI + 1);
                dup2(bufFd, STDOUT_FILENO);
                printf("Command \"%s\" works incorrect\n", matrix[barI + 1]);
                freeMatrix(matrix, realSize);
                close(fd[0]);
                close(fd[1]);
                if (getpid() != mainPid) {
                    kill(getpid(), SIGUSR1);
                }
                exit(1);
            } else{
                freeMatrix(matrix, realSize);
                close(fd[1]);
                close(fd[0]);
                if(getpid() != mainPid)
                    kill(getpid(), SIGUSR1);
                exit(1);
            }
        }
    }
}

int main(int argc, char** argv) {
    mainPid = getpid();
    dup2(STDOUT_FILENO, bufFd);
    int fd_from = open(argv[1], O_RDONLY);
    if(fd_from == -1){
        printf("Can not open the file\n");
        exit(-1);
    }
    int len;
    char *arr = file2arr(fd_from, &len);
    close(fd_from);
    //printf("Next word on the %i position\n", nextWord(arr, 31, len));
    int sizeMatrix;
    char** matrix = arr2matrix(arr, len, &sizeMatrix);
    free(arr);
    doBar(matrix, sizeMatrix, sizeMatrix);
    return 0;
}

int getFileSize(int fd){
    if(fd == -1)
        return -1;

    struct stat buf;
    fstat(fd, &buf);
    off_t file_size = buf.st_size;
    return file_size;
}

char* file2arr(int fd, int* arrLen){
    int fileLen = getFileSize(fd);
    char* arr = calloc(fileLen, sizeof(char));
    if(arr == NULL){
        printf("File is too big\n");
        exit(1);
    }
    read(fd, arr, fileLen);
    *arrLen = fileLen;
    return arr;
}

int numWords(char* arr,int lenArr){
    int numOfWords = 0;
    if(!isspace(arr[0])){
        numOfWords++;
    }
    for (int i = 0; i < lenArr - 1; ++i) {
        if(isspace(arr[i]) && !isspace(arr[i+1])){
            numOfWords++;
        }

        if(arr[i] == '|'){
            if(!isspace(arr[i+1])){
                numOfWords++;
            }
            if(i != 0 && !isspace(arr[i - 1])){
                numOfWords++;
                if (arr[i - 1] == '|'){
                    numOfWords --;
                }
            }
        }
    }
    if(arr[lenArr - 1] == '|'){
        if(lenArr - 1 != 0 && !isspace(arr[lenArr - 1 - 1])){
            numOfWords++;
            if (arr[lenArr - 1 - 1] == '|'){
                numOfWords --;
            }
        }
    }

    return numOfWords;
}

int getWordLen(char* arr, int position, int arrLen){
    while (position < arrLen && isspace(arr[position])){
        //printf( "Hello there\n");
        position++;
    }

    if (position == arrLen){
        return 0;
    }

    if(arr[position] == '|'){
        return 1;
    }

    int result = 0;
    while (position < arrLen && !isspace(arr[position]) && arr[position] != '|'){
        position++;
        result++;
    }

    return result;
}

int nextWord(char* arr, int position, int arrLen){
    while (position < arrLen){
        if (!isspace(arr[position])){
            break;
        }
        position++;
    }

    return position;
}

char** arr2matrix(char* arr,int lenArr, int* pSize){
    int nWords = numWords(arr, lenArr) + numOfBarArr(arr, lenArr);
    *pSize = nWords + 1;
    char **matrix = (char**) calloc(nWords + 1, sizeof(char*));
    int position = 0;
    for(int i = 0; i < nWords; i++){
        int lenWord = getWordLen(arr, position, lenArr);
        matrix[i] = (char*) calloc(lenWord + 1, sizeof(char));
        position = nextWord(arr, position, lenArr);
        for (int j = 0; j < lenWord; ++j) {
            matrix[i][j] = arr[position];
            position ++;
        }
        if(matrix[i][0] == '|'){
            i++;
            matrix[i] = NULL;
            char* buf = matrix[i];
            matrix[i] = matrix[i - 1];
            matrix[i - 1] = buf;
        }
    }
    matrix[nWords] = NULL;
    return matrix;
}

void printMatrix(char** matrix, int size){
    printf("Size of matrix: %i\n", size);
    for (int i = 0; i < size; ++i) {
        printf("%i) %s\n", i, matrix[i]);
    }
}

int numOfBarArr(char *arr, int len){
    int result = 0;
    for (int i = 0; i < len; ++i) {
        if(arr[i] == '|'){
            result++;
        }
    }

    return result;
}

int numOfBarMatrix(char** matrix, int size){
    int result = 0;
    for (int i = 0; i < size; ++i) {
        if(matrix[i] != NULL)
            if(matrix[i][0] == '|'){
                result++;
            }
    }
    return result;
}

int lastBarArr(char *arr, int len){
    int i = len - 1;
    while (arr[i] != '|'){
        i--;
    }

    return i;
}

int lastBarMatrix(char ** matrix, int size){
    for (int i = size - 1; i >= 0; --i) {
        if (matrix[i] != NULL && matrix[i][0] == '|'){
            return i;
        }
    }

    return -1;
}

void freeMatrix(char** matrix, int realSize){
    for(int i = 0; i < realSize; i++){
        if (matrix[i] != NULL){
            free(matrix[i]);
        }
    }
    free(matrix);
}
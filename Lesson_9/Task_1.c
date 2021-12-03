#include <dirent.h>
#include <stdio.h>

int main(int argc, char** argv)
    {
    DIR* dir = opendir(argv[1]);
    while (1)
        {
        struct dirent* file_info = readdir(dir);
        if (file_info == NULL)
            break;
        if (file_info -> d_type == DT_DIR)
            printf("%s\n", file_info -> d_name);
        }
    }
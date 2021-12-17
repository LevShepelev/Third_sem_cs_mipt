#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/wait.h>
#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <sys/inotify.h>

#define DEBUG

void logprint (const char* mestype, const char* mes, const char* param);
int dirnamecopy (char* dist, const char* source);
void param_changing();
void working(char* s1, char* s2);
void finishing();
void copy_proccess();
int dir_routine(const char* dest, const char* source);
void file_exist_checking(const char* dest, const char* source);
void watching();
char* get_path(int watch_id);
void mode_changing();
void checking_paths();
void My_copy(char* source, char* dest);

typedef struct dir_info_t { int watch_id; char* name; } dir_info_t;

struct sembuf sem_lock = {0, -1, 0};
struct sembuf sem_unlock = {0, 1, 0};
char source[PATH_MAX], dest[PATH_MAX], dir[PATH_MAX];
long delay;
int child_pid;
int semid;
FILE *file_out, *config;
int inotify_fd = 0;
dir_info_t* inotify_array = NULL;
int inotify_top = -1;
int exit_flag = 1, inotify_mode = 1, change_flag = 1;

void logprint (const char* mestype, const char* mes, const char* param)
    {
    char s[100];
    time_t tim = time (NULL);
    strftime (s, 100, "[%a %b %d %H:%M:%S %Y]", localtime (&tim));
    fprintf (file_out, "%s %s: %s %s\n", s, mestype, mes, param);
    fflush (file_out);
    }

int dirnamecopy (char* dist, const char* source)
    {
	strcpy(dist, source);
    int len = strlen(source);
            
    if (len == 0 || dist[len - 1] != '/')
        {
            dist[len] = '/';
            return len + 1;
        }

    return len;
    }

int dir_routine(const char* dest_local, const char* source_local)
    {
	DIR *dirs, *dird;
	char *paths = calloc (PATH_MAX, 1), *pathd = calloc (PATH_MAX, 1); 
	char *ends = paths, *endd = pathd;
	struct dirent *d;
	struct stat sinfo, dinfo;
    errno = 0;
	dirs = opendir(source_local);
    inotify_top++;
    inotify_array[inotify_top].watch_id = inotify_add_watch(inotify_fd, source_local, IN_CREATE | IN_DELETE | IN_MODIFY);
    inotify_array[inotify_top].name = calloc(PATH_MAX, sizeof(char));//semaphores
    int i = 0;
    for (; source_local[i] == source[i] && i < strlen(source); i++)
        ;
    strncpy(inotify_array[inotify_top].name, source_local + i * sizeof(char), strlen(source_local) - strlen(source));
    printf("watch was, watch_id = %d set name = %s\n", inotify_array[inotify_top].watch_id , inotify_array[inotify_top].name);
    dird = opendir(dest_local);
    if (!dird)
        {
        logprint ("INFO", "Created directory", dest_local);
        mkdir (dest_local, 0777);
        }
    dird = opendir (dest_local);
    /*if (dirs)
        printf("succes opening s\n");
    if (dird)
        printf("succes opening d\n");*/
	if (dirs)
        {
		ends += dirnamecopy (paths, source_local);
        endd += dirnamecopy (pathd, dest_local);
        
		while ((d = (struct dirent *)readdir(dirs)) != NULL)
            {
			if (strcmp(d->d_name, ".") == 0 || (strcmp(d->d_name, "..") == 0))
			    continue;
			strcpy(ends, d->d_name);
            strcpy(endd, d->d_name);
			if (!stat(paths, &sinfo))
                {
				if (S_ISDIR(sinfo.st_mode))    
                    dir_routine(pathd, paths);
                    
				else if (S_ISREG(sinfo.st_mode)|| S_ISLNK(sinfo.st_mode) || S_ISFIFO(sinfo.st_mode))                     
                    if (stat(pathd, &dinfo) || difftime(dinfo.st_mtim.tv_sec, sinfo.st_mtim.tv_sec) <= 0)
                        {
                        char tmp[1024] = "cp ";
                        strcat(tmp, paths);
                        strcat(tmp, " ");
                        strcat(tmp, pathd);
                        My_copy(paths, pathd);
                        logprint ("INFO", "Copied file", pathd);
                        }
			    }
		    }
	    }
    else
        logprint ("ERROR", "Cant open directory", source);

    free (paths);
    free (pathd);
    /*if (!closedir(dirs))
        printf("success closing s\n");
    if (!closedir(dird))
        printf("success closing d\n");*/
	return 0;
    }

int main(int argc, char** argv)
    {
    struct sigaction act_fin;
    memset(&act_fin, 0, sizeof(act_fin));
    act_fin.sa_handler = finishing;
    sigfillset(&act_fin.sa_mask);
    sigaction(SIGTERM, &act_fin, NULL);

    struct sigaction param_chg;
    memset(&param_chg, 0, sizeof(param_chg));
    param_chg.sa_handler = param_changing;
    sigfillset(&param_chg.sa_mask);
    sigaction(SIGUSR1, &param_chg, NULL);

    struct sigaction mode_chg;
    memset(&mode_chg, 0, sizeof(mode_chg));
    mode_chg.sa_handler = mode_changing;
    sigfillset(&mode_chg.sa_mask);
    sigaction(SIGUSR2, &mode_chg, NULL);

    inotify_array = (dir_info_t*) calloc(100000, sizeof(dir_info_t));
    inotify_fd = inotify_init(); 
    int pid = fork();
    if (pid < 0)
        {
        logprint ("ERROR", "Can't fork", NULL);
        exit (-1);
        }
    #ifndef DEBUG
    else if (pid > 0)
        exit (0);
    #endif
    #ifdef DEBUG
    else if (pid == 0)
        exit (0);
    #endif
    
    printf("pid = %d\n", getpid());
    getcwd(dir, PATH_MAX);
    int len = strlen (dir);

    if (len == 0 || dir[len - 1] != '/')
        {
        dir[len] = '/';
        len++;
        }

    strcpy (dir + len, "copydaemon.cfg");
    config = fopen(dir, "r");
    if (config == NULL)    
        {
        logprint ("ERROR", "Cant open config file. Please, use configurator", NULL);
        exit (-1);
        }
    fscanf (config, "%ld", &delay);
    fclose (config);
    config = fopen(dir, "w");
    fprintf(config, "%ld\n%s\n%s", delay, argv[1], argv[2]);
    fclose(config);
    #ifndef DEBUG                                             
    if (setsid () < 0)             
        exit (-1);
    #endif
    umask(0);
    chdir ("/");
    #ifndef DEBUG                                             
    fclose (stdin);
    fclose (stdout);
    fclose (stderr);
    #endif
    working(argv[1], argv[2]);
    printf("success_end\n");
	return 0;
    }




void file_exist_checking(const char* dest_local, const char* source_local)
    {
    DIR *dirs, *dird;
	char *paths = calloc (PATH_MAX, 1), *pathd = calloc (PATH_MAX, 1); 
	char *ends = paths, *endd = pathd;
	struct dirent *d;
	struct stat sinfo, dinfo;

	dirs = opendir(source_local);	
    dird = opendir(dest_local);
    if (!dird)
        {
        logprint ("INFO", "Created directory", dest_local);
        mkdir (dest_local, 0777);
        }
    dird = opendir (dest_local);
    /*if (dirs)
        printf("succes opening s\n");
    if (dird)
        printf("succes opening d\n");*/
	if (dirs)
        {
		ends += dirnamecopy (paths, source_local);
        endd += dirnamecopy (pathd, dest_local);
        while ((d = (struct dirent*) readdir(dird)) != NULL)
            {
            if (strcmp(d->d_name, ".") == 0 || (strcmp(d->d_name, "..") == 0))
			    continue;
            strcpy(ends, d->d_name);
            strcpy(endd, d->d_name);
            
            if (!stat(pathd, &dinfo))
                {
                if (S_ISDIR(dinfo.st_mode)) 
                    file_exist_checking(pathd, paths);
                
                else if ((S_ISREG(dinfo.st_mode) || S_ISLNK(sinfo.st_mode) || S_ISFIFO(sinfo.st_mode)) && stat(paths, &sinfo) && (strcmp(d -> d_name, "copydaemon.log") != 0)) 
                    {//add another files        
                    remove(pathd);
                    logprint ("INFO", "Delete file", pathd);
                    }	
                }
            }
        }
    /*if (!closedir(dirs))
        printf("success closing s\n");
    if (!closedir(dird))
        printf("success closing d\n");*/
    free(pathd);
    free(paths);
    }

void finishing()
    {
    semctl(semid, 1, IPC_RMID);
    for (int i = 0; i < inotify_top; i++)
        free(inotify_array[i].name);
    free(inotify_array);
    close(inotify_fd);
    fclose(file_out);
    printf("finished\n");
    exit_flag = 0;
    }


void param_changing()
    {
    char new_source[4096] = { '\0' }, new_dest[4096] = { '\0' };
    if (semctl(semid, 0, GETVAL) == 0)
        semop(semid, &sem_unlock, 1);
    config = fopen(dir, "r");
    fseek(config, 0, SEEK_SET);
    fscanf(config, "%ld%s%s", &delay, new_source, new_dest);
    for (int i = 0; i < strlen(source); i++)
        if (new_source[i] == '\n')
            new_source[i] = '\0';
    for (int i = 0; i < strlen(dest); i++)
        if (new_dest[i] == '\n')
            new_dest[i] = '\0';
    if (strcmp(new_source, source) != 0)
        logprint("INFO", "source dir has changed", new_source); 
    if (strcmp(new_dest, dest) != 0)
        logprint("INFO", "source dir has changed", new_dest); 
    strcpy(source, new_source);
    strcpy(dest, new_dest);
    fclose(config);
    char tmp[PATH_MAX] = {'\0'};
    strcpy(tmp, dest);
    strcpy(tmp + strlen(tmp), "/copydaemon.log");
    fclose(file_out);
    file_out = fopen (tmp, "a+");
    checking_paths();
    file_exist_checking(dest, source);
    dir_routine(dest, source);
    }

void copy_proccess()
    {
    while (exit_flag && change_flag)
        {
        semctl(semid, 0, GETVAL);
        semop(semid, &sem_lock, 1);
        file_exist_checking(dest, source);
        dir_routine(dest, source);
        semop(semid, &sem_unlock, 1);
        sleep (delay);
        }
    }

void checking_paths()
    {
    if (strncmp(source, dest, strlen(source) + 1) == 0)
        {
        printf("ERROR: archieve inside archiviening directory\n");
        exit(EXIT_FAILURE); 
        }
    }

void working(char* s1, char* s2)
    {
    char tmp[PATH_MAX] = {'\0'};
    strcpy(source, s1);
    strcpy(dest, s2);
    strcpy(tmp, dest);
    strcpy(tmp + strlen(tmp), "/copydaemon.log");
    file_out = fopen (tmp, "a+");
    if (file_out == NULL)
        exit (-1);
    int key = 0;
    checking_paths();
    if((key = ftok(tmp, 0)) < 0)
        {
        logprint ("ERROR", "Cant gey key for file", NULL);
        exit(-1);
        }

    if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0)
        {
        logprint("ERROR", "Cant gey semid", NULL);
        exit(-1);
        }
    semctl(semid, 0, SETVAL, 1);
    logprint ("INIT", "source is ", &source[0]);
    logprint ("INIT", "destination is ", &dest[0]);
    dir_routine(s2, s1);
    file_exist_checking(s2, s1);
    while (exit_flag)
        {
        if (inotify_mode)
            logprint("INIT", "inotify mode was set", "");
        else logprint("INIT", "classic mode was set", "");
        change_flag = 1;
        if (!inotify_mode) 
                copy_proccess();
        else watching();
        }
    }

void watching()
    {
    int len = 0;
    char* ptr = NULL;
    const struct inotify_event *event;
    char buf[4096];
    char paths[4096];
    char pathd[4096];
    while (exit_flag && change_flag)
        {
        len = read(inotify_fd, buf, sizeof(struct inotify_event) + PATH_MAX);
        if (len == -1 && errno != EAGAIN) 
            break;
            
        
        for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) 
            {
            event = (const struct inotify_event *) ptr;
            //printf("get_path = %s,  source = %s, name = %s\n", get_path(event -> wd), source, event -> name);
            printf("get_path = %s\n", get_path(event -> wd));
            strcat(strcpy(paths, source), get_path(event -> wd));
            strcat(strcpy(pathd, dest), get_path(event -> wd));
            //printf("event -> wd = %d, event -> name = %s\n", event -> wd, event -> name);
            char tmp_file_name[4096] = {'\0'};
            struct stat sinfo;

            if (event -> mask & IN_CREATE)
                {
                strcpy(tmp_file_name, paths);
                strcat(strcat(tmp_file_name, "/"), event -> name);
                if (!stat(tmp_file_name, &sinfo))
                    {
				    if (S_ISREG(sinfo.st_mode) || S_ISLNK(sinfo.st_mode) || S_ISFIFO(sinfo.st_mode))                     
                        {
                        char tmp[4096];
                        strcpy(tmp, pathd);
                        strcat(strcat(tmp, "/"), event -> name);
                        My_copy(tmp_file_name, tmp);
                        logprint ("INFO", "Copied file", pathd);
                        }
                    else if (S_ISDIR(sinfo.st_mode))
                        {
                        errno = 0;
                        mkdir(strcat(strcat(strcpy(tmp_file_name, pathd), "/"), event -> name), 0777);
                        dir_routine(dest, source);
                        logprint("INFO", "Copied directory", tmp_file_name);
                        }
			        }
                perror("stat\n");
                }

            else if (event -> mask & IN_DELETE)
                {
                strcpy(tmp_file_name, pathd);
                strcat(strcat(tmp_file_name, "/"), event -> name);
                printf("delete %s\n", tmp_file_name);
                if (!stat(tmp_file_name, &sinfo))
                    {
                    if (S_ISREG(sinfo.st_mode) || S_ISLNK(sinfo.st_mode) || S_ISFIFO(sinfo.st_mode))
                        {
                        remove(tmp_file_name);
                        logprint ("INFO", "Delete file", tmp_file_name);
                        printf("IN_DELETE\n");
                        }
                    if (S_ISDIR(sinfo.st_mode))
                        {
                        rmdir(tmp_file_name);
                        logprint ("INFO", "Delete directory", pathd);
                        }
                    }
                printf("IN_DELETE\n");
                }

            else if (event -> mask & IN_MODIFY)
                {
                strcpy(tmp_file_name, paths);
                char tmp[4096];
                strcpy(tmp, pathd);
                strcat(strcat(tmp, "/"), event -> name);
                strcat(strcat(tmp_file_name, "/"), event -> name);
                if (!stat(tmp_file_name, &sinfo))
                    {
				    if (S_ISREG(sinfo.st_mode))                     
                        {
                        My_copy(tmp_file_name, tmp);
                        logprint ("INFO", "Modified file", tmp_file_name);
                        }
			        }
                len = read(inotify_fd, buf, sizeof(struct inotify_event) + PATH_MAX);
                }
            }
        }
    }

char* get_path(int watch_id)
    {
    for (int i = 0; i <= inotify_top; i++)
        {
        printf("watch_id = %d, name = %s\n", inotify_array[i].watch_id, inotify_array[i].name);
        if (inotify_array[i].watch_id == watch_id)
            {
            printf("yes\n");
            return inotify_array[i].name;
            }
        }
    printf("path hasnt found\n");
    return "";
    }

void mode_changing()
    {
    inotify_mode = (inotify_mode + 1) % 2;
    change_flag = 0;
    }

void My_copy(char* loc_source, char* loc_dest)
{
int in, out;
int  n;
char buf [512];
in = open(loc_source, O_RDONLY);
out = open(loc_dest, O_CREAT | O_TRUNC | O_WRONLY, 0777);
while ((n = read(in, buf, sizeof buf)) > 0)
  {
    if (write(out, buf, n) == -1 )
        printf("problem_with_writing to file in copying\n");
  }
close(in);
close(out);
}
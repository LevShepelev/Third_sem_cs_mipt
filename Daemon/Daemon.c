
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

void logprint (const char* mestype, const char* mes, const char* param);
int dirnamecopy (char* dist, const char* source);
void delay_changing();
void finishing();
void copy_iteration();
int dir_routine(const char* dest, const char* source);
void file_exist_checking(const char* dest, const char* source);

struct sembuf sem_lock = {0, -1, 0};
struct sembuf sem_unlock = {0, 1, 0};
char source[256], dest[256];
long delay;
int child_pid;
int semid;
FILE *file_out, *config;

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
	char *paths = calloc (256, 1), *pathd = calloc (256, 1); 
	char *ends = paths, *endd = pathd;
	struct dirent *d;
	struct stat sinfo, dinfo;
    errno = 0;
	dirs = opendir(source_local);	
    dird = opendir(dest_local);
    if (!dird)
        {
        logprint ("INFO", "Created directory", dest_local);
        mkdir (dest_local, 0777);
        }
    dird = opendir (dest_local);
    
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
                    
				else if (S_ISREG(sinfo.st_mode))                     
                    if (stat(pathd, &dinfo) || difftime(dinfo.st_mtim.tv_sec, sinfo.st_mtim.tv_sec) <= 0)
                        {
                        char tmp[1024] = "cp ";
                        strcat(tmp, paths);
                        strcat(tmp, " ");
                        strcat(tmp, pathd);
                        system (tmp);
                        logprint ("INFO", "Copied file", pathd);
                        }
			    }
		    }
	    }
    else
        logprint ("ERROR", "Cant open directory", source);

    free (paths);
    free (pathd);
        
	return 0;
    }

int main(int argc, char* argv[])
    {	
    struct sigaction act_fin;
    memset(&act_fin, 0, sizeof(act_fin));
    act_fin.sa_handler = finishing;
    sigfillset(&act_fin.sa_mask);
    sigaction(SIGINT, &act_fin, NULL);

    struct sigaction delay_chg;
    memset(&delay_chg, 0, sizeof(delay_chg));
    delay_chg.sa_handler = delay_changing;
    sigfillset(&delay_chg.sa_mask);
    sigaction(SIGUSR1, &delay_chg, NULL);

    config = fopen("/home/lev/Third_sem_cs_mipt/Daemon/copydaemon.cfg", "r");
    if (config == NULL)    
        {
        logprint ("ERROR", "Cant open config file. Please, use configurator", NULL);
        exit (-1);
        }
    int pid = fork();
    if (pid < 0)
        {
        logprint ("ERROR", "Can't fork", NULL);
        exit (-1);
        }
    else if (pid > 0)
        exit (0);

    printf("pid = %d\n", getpid());
    char dir[256], tmp[256] = {'\0'};
    getcwd(dir, 256);
    int len = strlen (dir);

    if (len == 0 || dir[len - 1] != '/')
        {
        dir[len] = '/';
        len++;
        }
        
    if (setsid () < 0)
        exit (-1);
    umask(0);
    chdir ("/");
    fclose (stdin);
    fclose (stdout);
    fclose (stderr);

    strcpy(source, argv[1]);
    strcpy(dest, argv[2]);
    fscanf (config, "%ld", &delay);

    strcpy(tmp, dest);
    
    strcpy(tmp + strlen(tmp), "/copydaemon.log");
    file_out = fopen (tmp, "a+");
    if (file_out == NULL)
        exit (-1);

    int key = 0;
    if((key = ftok(tmp, 0)) < 0)//to link with log_file
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

    while (1)
        {
        copy_iteration();
        }

	return 0;
    }




void file_exist_checking(const char* dest_local, const char* source_local)
    {
    DIR *dirs, *dird;
	char *paths = calloc (256, 1), *pathd = calloc (256, 1); 
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
                
                else if (S_ISREG(dinfo.st_mode) && stat(paths, &sinfo) && (strcmp(d -> d_name, "copydaemon.log") != 0)) 
                    {                  
                    remove(pathd);
                    logprint ("INFO", "Delete file", pathd);
                    }	
                }
            }
        }
    free(pathd);
    free(paths);
    }

void finishing()
    {
    semop(semid, &sem_lock, 1);
    semctl(semid, 1, IPC_RMID);
    fclose(file_out);
    fclose(config);
    exit(0);
    }

void delay_changing()
    {
    fseek(config, 0, SEEK_SET);
    semop(semid, &sem_lock, 1);
    fscanf (config, "%ld", &delay);
    file_exist_checking(dest, source);
    dir_routine(dest, source);
    semop(semid, &sem_unlock, 1);
    }

void copy_iteration()
    {
    semctl(semid, 0, GETVAL);
    semop(semid, &sem_lock, 1);
    file_exist_checking(dest, source);
    dir_routine(dest, source);
    semop(semid, &sem_unlock, 1);
    sleep (delay);
    }
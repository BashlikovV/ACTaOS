#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>

void search_file(char* dir_name, char file_name[256], int pid_count);
void mode_to_letters( int mode, char str[] );
int atoi( const char *c );

long long currentTime() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_usec;
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Error: Too few arguments~\n");
        return 1;
    }

    char *tmp = argv[3];
    int pid_count = atoi(tmp);

    search_file(argv[1], argv[2], pid_count);

    return 0;
}

void search_file(char *direc_name, char file_name[256], int pid_count) {
    pid_t pid[pid_count];
    int chaild_status;
    int process_count = 1;

    struct dirent *d;
    struct stat st;
    char dir_name[255];
    char new_name[255];
    int count = 0;

    DIR *d1 = opendir(direc_name);
    d = readdir(d1);

    strcpy(dir_name, direc_name);
    strcat(dir_name, "/");

    while (d != NULL) {
        if (d->d_type == DT_DIR) {
            if ((strcmp(d->d_name, ".") != 0) && (strcmp(d->d_name, "..") != 0)) {
                strcpy(new_name, direc_name);
                strcat(new_name, "/");
                strcat(new_name, d->d_name);

                process_count++;
                if ((pid[process_count] = fork()) == 0) {
                    printf("fork: %d; count = %d; time = %d; parent: %d\n", (int) getpid(), process_count, currentTime(), (int) getppid());
                    search_file(new_name, file_name, pid_count);
                    exit(0);
                }
            }
        }
        d = readdir(d1);
        count++;

        if (process_count >= pid_count) {
            process_count--;
            wait(&chaild_status);
        }

    }

    for (int i = 0; i < pid_count; i++) {
        wait(&chaild_status);
    }

    rewinddir(d1);
    d = readdir(d1);

    while (d != NULL) {
        if (d->d_type == DT_REG) {
            strcpy(new_name, direc_name);
            strcat(new_name, d->d_name);

            if (strcmp(d->d_name, file_name) == 0) {
                struct tm * timeinfo;
                char buff[20];

                char *path = direc_name;
                char tmp[255];
                tmp[0] = '/';
                tmp[1] = '\000';
                strcat(tmp, file_name);
                strcat(path, tmp);
                printf("\nPath: %s\n", path);

                printf("Name: %s\n", file_name);

                stat(direc_name, &st);
                timeinfo = localtime((const time_t *) &st.st_ctime);
                strftime(buff, 20, "%b %d %H:%M", timeinfo);

                char mode_str[11];
                mode_to_letters(st.st_mode, mode_str);

                printf("Size: %ld byte\n", st.st_size);
                printf("Creating data: %s\n", buff);
                printf("Descriptor: %lu\n", st.st_ino);
                printf("Access rights: %s\n\n", mode_str);
                return;
            }
        }
        d = readdir(d1);
    }
}

void mode_to_letters(int mode, char str[]) {
    strcpy( str, "----------" );           /* default=no perms */

    if ( S_ISDIR(mode) )  str[0] = 'd';    /* directory?       */
    if ( S_ISCHR(mode) )  str[0] = 'c';    /* char devices     */
    if ( S_ISBLK(mode) )  str[0] = 'b';    /* block device     */

    if ( mode & S_IRUSR ) str[1] = 'r';    /* 3 bits for user  */
    if ( mode & S_IWUSR ) str[2] = 'w';
    if ( mode & S_IXUSR ) str[3] = 'x';

    if ( mode & S_IRGRP ) str[4] = 'r';    /* 3 bits for group */
    if ( mode & S_IWGRP ) str[5] = 'w';
    if ( mode & S_IXGRP ) str[6] = 'x';

    if ( mode & S_IROTH ) str[7] = 'r';    /* 3 bits for other */
    if ( mode & S_IWOTH ) str[8] = 'w';
    if ( mode & S_IXOTH ) str[9] = 'x';
}

int atoi( const char *c ) {
    int value = 0;
    int sign = 1;
    if( *c == '+' || *c == '-' ) {
        if( *c == '-' ) sign = -1;
        c++;
    }
    while ( isdigit( *c ) ) {
        value *= 10;
        value += (int) (*c-'0');
        c++;
    }
    return value * sign;
}

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdlib.h>

int counter = 0;

typedef struct Arguments{
    char *dir_name;
    char file_name[255];
    int threads_count;
}Arguments;

void* search_file(void* args);
void mode_to_letters( int mode, char str[] );
int atoi( const char *c );

long long currentTime() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_usec / 1000;
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Error: Too few arguments~\n");
        return 1;
    }

    Arguments *arg = (Arguments*)malloc(sizeof(Arguments));

    arg->dir_name = argv[1];
    strcpy(arg->file_name, argv[2]);
    arg->threads_count = atoi(argv[3]);
    search_file(arg);

    return 0;
}

void* search_file(void *args) {
    Arguments *arg = (Arguments*) args;
    pthread_t thread[arg->threads_count];
    struct dirent *d;
    struct stat st;
    char *dir_name = malloc(sizeof(char) * 255);
    char *new_name = malloc(sizeof(char) * 255);

    strcpy(dir_name, arg->dir_name);

    DIR *d1 = opendir(dir_name);
    d = readdir(d1);

    //strcpy(dir_name, arg->dir_name);
    //dir_name = strcat(dir_name, "/");

    while (d != NULL) {

        if (counter >= arg->threads_count) {
            pthread_exit(EXIT_SUCCESS);
        }

        if (d->d_type == DT_DIR) {
            if ((strcmp(d->d_name, ".") != 0) && (strcmp(d->d_name, "..") != 0)) {
                strcpy(new_name, arg->dir_name);
                new_name = strcat(new_name, "/");
                new_name = strcat(new_name, d->d_name);

                Arguments *ar = (Arguments*) malloc(sizeof(Arguments));
                //ar->dir_name = (char *) malloc(sizeof(char));
                ar->dir_name = new_name;
                strcpy(ar->file_name, arg->file_name);
                ar->threads_count = arg->threads_count;

                counter++;
                pthread_create(&thread[counter], NULL, search_file, ar);
                printf("Thread: %lu; count: %d, time: %lld\n", pthread_self(), counter, currentTime());
                pthread_join(thread[counter], NULL);
                counter--;
            }
        }
        d = readdir(d1);
    }


    rewinddir(d1);
    d = readdir(d1);

    while (d != NULL) {
        if (d->d_type == DT_REG) {
            strcpy(new_name, arg->dir_name);
            new_name = strcat(new_name, d->d_name);

            if (strcmp(d->d_name, arg->file_name) == 0) {
                struct tm * timeinfo;
                char buff[20];

                char *path = arg->dir_name;
                //char tmp[255];
                char * tmp = malloc(sizeof(char) * 255);
                tmp[0] = '/';
                tmp[1] = '\000';
                tmp = strcat(tmp, arg->file_name);
                path = strcat(path, tmp);
                printf("\nPath: %s\n", path);

                printf("Name: %s\n", arg->file_name);

                stat(arg->dir_name, &st);
                timeinfo = localtime((const time_t *) &st.st_ctime);
                strftime(buff, 20, "%b %d %H:%M", timeinfo);

                char mode_str[11];
                mode_to_letters(st.st_mode, mode_str);

                printf("Size: %ld byte\n", st.st_size);
                printf("Creating data: %s\n", buff);
                printf("Descriptor: %lu\n", st.st_ino);
                printf("Access rights: %s\n\n", mode_str);
                return NULL;
            }
        }
        d = readdir(d1);
    }

    return NULL;
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

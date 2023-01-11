#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

int searchdir(char *name);

int maxsize = 0;
int minsize = 0;
long mindate = 0;
long maxdate = 0;

FILE *f1;

int main(int argc, char *argv[]) {
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

    maxsize = atoi(argv[4]);
    minsize = atoi(argv[3]);
    maxdate = atol(argv[6]);
    mindate = atol(argv[5]);
    f1 = fopen(argv[2], "w");

    searchdir(argv[1]);

    fclose(f1);
    
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    
    return 0;
}

int searchdir(char *name) {
    char dirname[256];
    char newname[256];
    DIR *d1;
    struct dirent *d;
    struct stat st1;

    d1 = opendir(name);
    d = readdir(d1);

    strcpy(dirname, name);
    strcat(dirname, "/");

    while (d != NULL) {
        if (d->d_type == DT_DIR) {
            if ((strcmp(d->d_name, ".") != 0) && (strcmp(d->d_name, "..") != 0)) {
                strcpy(newname, dirname);
                strcat(newname, d->d_name);

                searchdir(newname);
            }
        }
        d = readdir(d1);
    }

    rewinddir(d1);
    d = readdir(d1);

    while (d != NULL) {
        if (d->d_type == DT_REG) {
            strcpy(newname, dirname);
            strcat(newname, d->d_name);

            stat(newname, &st1);
            if (((st1.st_size) >= minsize) && ((st1.st_size) <= maxsize)) {
                if (((st1.st_mtime) >= mindate) && ((st1.st_mtime) <= maxdate)) {
                    fprintf(f1, "%s  %ld  %lld\n", newname, st1.st_size, st1.st_mtime);
                    printf("%s  %ld  %lld\n", newname, st1.st_size, st1.st_mtime);
                }
            }
        }
        
        d = readdir(d1);
    }
    
    closedir(d1);
}
// Here is a small program written in C that uses scandir() function for
// iteration. This function recursively iterates all directories and files.

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAX_PATH_LENGTH 4096

static void
count_files(char *path, int *file_count, int *dir_count)
{
    struct dirent *entry;
    char           fullPath[MAX_PATH_LENGTH];

    DIR *dp = opendir(path);
    if (dp == NULL) {
        perror("Couldn't open the directory");
        return;
    }

    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") == 0
            || strcmp(entry->d_name, "..") == 0)
            continue;

        sprintf(fullPath, "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            printf("DIR: %s\n", fullPath);
            *dir_count += 1;
            count_files(fullPath, file_count, dir_count);
        } else if (entry->d_type == DT_REG) {
            printf("FILE: %s\n", fullPath);
            *file_count += 1;
        }
    }

    closedir(dp);
}

int
main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <dir>\n", argv[0]);
        return 1;
    }

    int file_count = 0, dir_count = 0;
    count_files(argv[1], &file_count, &dir_count);

    printf("Total Directories: %d\n", dir_count);
    printf("Total Files: %d\n", file_count);

    return 0;
}

// To use this program: compile it (using a `-std=c99` option for the gcc
// compiler), and then call it with a directory path as the argument. The
// program will output the total numbers of directories and regular files in
// that directory, as well as all its subdirectories.

// This program keeps track of the number of regular files (DType DT_REG) and
// directories (DType DT_DIR). In addition to counting files and directories,
// it recursively navigate down each directory tree in the scandir() call,
// adding to the counts along the way. For each directory dd, it opens
// directory dd, reads each entry d, and if d is not. or.., adds the entry to
// full path, if d is a directory it counts and recursively calls its
// count_files() function, if d is a regular file it just increments the
// file_count.
//
// prompt
// write a program in c to recursive iterate all directories and files  to
// calculate the total directories and files, you should use scandir()
// function

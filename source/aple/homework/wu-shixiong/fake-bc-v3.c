#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 64

int
main(int argc, char *argv[])
{
    int   n;
    int   fd[2];
    pid_t pid;
    char  line[MAXLINE];

    if (argc != 2) {
        printf("usage: %s \"1 + 1\"\n", argv[0]);
        printf("usage: %s \"scale=2; 1 + 1/2\"\n", argv[0]);
        return -1;
    }

    if (pipe(fd) < 0) {
        printf("pipe error\n");
        exit(-1);
    }

    pid = fork();
    if (pid < 0) {
        printf(" fork error\n");
        return -1;
    } else if (pid > 0) { // parant
        close(fd[0]);     // close pipe read end
        sprintf(line, "%s\n", argv[1]);
        n = strlen(line);
        write(fd[1], line, n);
        // close fd[0]  child can reciev EOF
        close(fd[1]); // close write end of pipe for reader
        if (waitpid(pid, NULL, 0) < 0) {
            printf("waitpid error\n");
            return -1;
        }
        exit(0);
    } else {          // child
        close(fd[1]); // close pipe write end
        if (fd[0] != STDIN_FILENO) {
            if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO) {
                printf("child dump2 error to stdin\n");
            }
            close(fd[0]); // don't need this after dup2
        }

        if (execl("/usr/bin/bc", "/usr/bin/bc", (char *)NULL) < 0) {
            printf("child execl failed\n");
            exit(-1);
        }
    }
}

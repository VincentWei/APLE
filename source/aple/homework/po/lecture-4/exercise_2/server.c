#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>

#define UNIX_STREAM_PATH "/tmp/bc_unix_steam"
#define READ 0
#define WRITE 1

#define MAXLINE 1024
int handle_using_dup2(int sockfd)
{
    // handle input and output
    printf("start handling bc input/output:\n");
    ssize_t n;
    char buf[MAXLINE];
    pid_t child_pid;

    dup2(sockfd, STDIN_FILENO);
    dup2(sockfd, STDOUT_FILENO);
    // spawn bc process
    pid_t bc_pid;
    char *const spawn_argv[] = {"bc", NULL};
    int ret = posix_spawnp(&bc_pid, "bc", NULL, NULL, spawn_argv, NULL);
    if (ret != 0)
    {
        printf("spawn bc program error: %u\n", ret);
        exit(-1);
    }
}

int handle_using_pipe(int sockfd){
    // handle input and output
    printf("start handling bc input/output:\n");
    ssize_t n;
    char buf[MAXLINE];
    pid_t child_pid;

    // helper to redirect stdio
    int pipeR[2]; // parent read child write
    int pipeW[2]; // parent write child read

    pipe(pipeR);
    pipe(pipeW);

    child_pid = fork();
    if (child_pid == 0)
    { // read from bc and write to socket

        close(pipeW[WRITE]);
        close(pipeR[READ]);

        dup2(pipeW[READ], STDIN_FILENO);
        dup2(pipeR[WRITE], STDOUT_FILENO);

        // spawn bc process
        pid_t bc_pid;
        char *const spawn_argv[] = {"bc", NULL};
        int ret = posix_spawnp(&bc_pid, "bc", NULL, NULL, spawn_argv, NULL);
        if (ret != 0)
        {
            printf("spawn bc program error: %u\n", ret);
            exit(-1);
        }

        close(pipeW[READ]);
        close(pipeR[WRITE]);
    }
    else
    { // read from socket and write to bc
        close(pipeW[READ]);
        close(pipeR[WRITE]);

        while ((n = read(sockfd, &buf, MAXLINE)) > 0)
        {
            printf("receive buf from socket: %s", buf);
            n = write(pipeW[WRITE], &buf, n);
            memset(buf,0,MAXLINE);
            if (n < 0)
            {
                printf(" write to bc error: %s\n", strerror(errno));
                exit(-1);
            }

            n = read(pipeR[READ], &buf, MAXLINE);
            if (n < 0)
            {
                printf(" read from bc error: %s\n", strerror(errno));
                exit(-1);
            }
            printf("receive buf from bc: %s\n", buf);

            n = write(sockfd, &buf, n);
            if (n < 0)
            {
                printf(" write to socket error: %s\n", strerror(errno));
                exit(-1);
            }

        }

        if (n < 0)
        {
            printf(" read from socket error: %s", strerror(errno));
            exit(-1);
        }

        close(pipeW[WRITE]);
        close(pipeR[READ]);

        waitpid(child_pid, NULL, 0);
    }
}

int start_listen()
{
    int ret;
    int serv_fd, conn_fd;
    pid_t child_pid;
    socklen_t cliaddr_len;
    struct sockaddr_un serv_addr, cli_addr;

    serv_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (serv_fd < 0)
    {
        printf("server socket error: %s\n", strerror(errno));
        return -1;
    }

    unlink(UNIX_STREAM_PATH);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_LOCAL;
    strncpy(serv_addr.sun_path, UNIX_STREAM_PATH, sizeof(serv_addr.sun_path) - 1);

    ret = bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    ret = listen(serv_fd, 1);
    if (ret)
    {
        printf("listen error: %s\n", strerror(errno));
        return -1;
    }
    printf("server start listening...\n");

    while (1)
    {
        cliaddr_len = sizeof(cli_addr);
        if ((conn_fd = accept(serv_fd, (struct sockaddr *)&cli_addr, &cliaddr_len)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                printf("accept error: %s\n", strerror(errno));
                return -1;
            }
        }
        printf("got a client connection.\n");

        if ((child_pid = fork()) == 0)
        {
            /* child close listening socket and process connected socket */
            close(serv_fd);
            handle_using_dup2(conn_fd);
            // handle_using_pipe(conn_fd);
            exit(0);
        }
        /* parent close connected socket */
        close(conn_fd);
    }
}

int main()
{
    return start_listen();
}
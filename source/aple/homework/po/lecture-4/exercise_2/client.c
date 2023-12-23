#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define UNIX_STREAM_PATH "/tmp/bc_unix_steam"

#define MAXLINE 1024

int conn(){
    int ret;
    int conn_fd;
    pid_t child_pid;
    struct sockaddr_un serv_addr;

    char buf[MAXLINE];
    ssize_t num_read;

    conn_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (conn_fd < 0) {
        printf("server socket error: %s\n", strerror(errno));
        return -1;
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_LOCAL;
    strncpy(serv_addr.sun_path, UNIX_STREAM_PATH, sizeof(serv_addr.sun_path) - 1);

    ret = connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret) {
        printf("connect error: %s\n", strerror(errno));
        return -1;
    }

    child_pid = fork();
    if(child_pid == 0) { //child process for reading
        while((num_read = read(conn_fd, &buf, MAXLINE)) > 0) {
            write(STDOUT_FILENO, &buf, num_read);
        }
    } else {
        while((num_read = read(STDIN_FILENO, &buf, MAXLINE)) > 0) {
            num_read = write(conn_fd, buf, num_read);
            if (num_read == -1) {
                printf("write error\n");
                return -1;
            }
        }
    }

    waitpid(child_pid, NULL, 0);
}

int main() {
    return conn();
}
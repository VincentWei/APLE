#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

int client_check_unix_socket(const char *path)
{
    int ret = -1;

    if (access(path, R_OK | W_OK) == 0) {
        int fd;
        if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
            goto done;
        }

        /* fill socket address structure w/server's addr */
        struct sockaddr_un unix_addr;
        memset(&unix_addr, 0, sizeof(unix_addr));
        unix_addr.sun_family = AF_UNIX;
        strcpy(unix_addr.sun_path, path);
        size_t len = sizeof(unix_addr.sun_family) + strlen(path) + 1;
        if (connect(fd, (struct sockaddr *)&unix_addr, len) == 0) {
            ret = 0;
        }
        close(fd);
    }

done:
    return ret;
}


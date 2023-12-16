#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * Q: 小白请教一下，Linux下，多进程并发写文件，会不会出现多个进程写的内容交错
 *    混杂？
 *
 * A1: 我写了个C语言demo试了试，在主进程中打开了文件，把文件描述符传递到各个
 *     子进程。 根据输出现象来看，每个进程往文件写内容的时候会获取文件描述符
 *     的锁，从而每个进程写入的数据的是有序的。 不加锁会乱序就像printf一样
 *
 * 我的理解：
 * 所有的子进程共享同一个写 buffer，该 buffer 默认是 line buffer 模式，同时
 * 该buffer 可以通过setvbuf 来改变大小和模式， 无论数据是在用户空间(fflush)，
 * 还是内核中(fsync), 只要进程调度发生在写的过程中, 都会出现写错乱的情况。
 * 当 setvbuf 时指定的 bufsize 小于文件系统的 block size时, 都容易出现错乱
 * 的情况, 越小越容易出现错乱的情况，bufsize=1 或者 _IONBUF 最容易出现。
 *
 *   涉及到三个关键函数: fflush fsync setvbuf
 *   fflush 属于标准IO库,只是把 user-space buffered 数据更新到 stream 中。
 *   fsync 属于OS kernel, 把所有缓存的数据写入到物理存储设备中。
 *   setvbuf 设置 stream 的缓冲区大小
 *
 *   In summary, fflush is used for flushing the stream's buffer and writing
 *   the data to the file, and fsync is used for ensuring that the file
 *   changes are physically stored on the disk. They operate at different
 *   levels, and the setvbuf function can affect the behavior of fflush in
 *   terms of how buffering is handled.
 *
 *  多进程同时写入同一个文件，文件内容一定会错乱的必要条件：
 *  只要单进程一次写入的数据大于 setvbuf 的 buffer 大小必然会出现错乱情况。
 *
 *  怎么让文件错误出现的更容易：
 *   setvbuf 的 size = 1 或者 设置为无 buffer 模式  _IONBF
 *   同时单进程一次写入的文件的数据变大，比如一次写入 8096 字节
 *
 *  怎么容易制作不会错乱的假象：
 *   单进程一次写入的数据尽量少
 *   同时写之前调用fflush
 *
 * 要避免多个进程写同一个文件时不出错内容错乱的情况需要满足两个条件:
 *  1. 写文件时必须要添加 lock (fcntl 设置 F_SETLKW or F_SETLKW64)
 *  2. setvbuf 的 buffer size 要大于单次写入文件的最大长度
 *
 * */

#define NUM_PROCESSES 10  // 进程数
#define WRITE_COUNTS  100 // 每个进程写入同一个文件的次数
// 只要  MAX_WRITE_BYTES > VBUF_SIZE 大小就容易出现错乱，无论是否 fflush&fsync
#define VBUF_SIZE       (1024 * 2)
#define MAX_WRITE_BYTES (1024 * 1)

static void
writeToFile(int processID, FILE *file)
{
    sleep(1);
    for (int i = 0; i < WRITE_COUNTS; ++i) {
        // fflush(file);
        // fsync(fileno(file)); // 系统调用开销更大
        for (int j = 0; j < (MAX_WRITE_BYTES - 1); j++) {
            fprintf(file, "%d", processID);
        }
        fprintf(file, "\n"); // 写入的换行符占用最后一个字节
        // fflush(file);
        // fsync(fileno(file));
    }
}

static void
writeToFileLock(int processID, FILE *file)
{
    struct flock lock;
    lock.l_type   = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 0;
    lock.l_len    = 0; // lock all bytes
    sleep(1);
    for (int i = 0; i < WRITE_COUNTS; ++i) {
        if (fcntl(fileno(file), F_SETLKW, &lock) != -1) {
            for (int j = 0; j < (MAX_WRITE_BYTES - 1); j++) {
                fprintf(file, "%d", processID);
            }
            fprintf(file, "\n"); // 写入的换行符占用最后一个字节
            fflush(file);
            lock.l_type = F_UNLCK;
            fcntl(fileno(file), F_SETLK, &lock);
        }
    }
}

int
main(void)
{
    FILE *file;
    file = fopen("output.txt", "w");
    char mybuf[VBUF_SIZE];
    // setvbuf(file, NULL, _IONBF, VBUF_SIZE);
    setvbuf(file, mybuf, _IOFBF, VBUF_SIZE);

    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    pid_t pid;

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            // writeToFile(i, file);
            writeToFileLock(i, file);
            exit(EXIT_SUCCESS);
        }
    }

    // Parent process waits for all child processes to complete
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        wait(NULL);
    }

    fclose(file);

    // 通过单行文件大小来判断是否有错乱发生
    char   *line = NULL;
    ssize_t nread;
    size_t  len   = 0;
    int     count = 0;

    file = fopen("output.txt", "r");
    while ((nread = getline(&line, &len, file)) != -1) {
        count++;
        if ((len - 1) != MAX_WRITE_BYTES) { // -1 null-terminated
            printf("ERROR line:%d  is wrong %ld !=%d nread =%ld\n", count, len,
                   MAX_WRITE_BYTES, nread);
        }
    }
    free(line);

    fclose(file);

    return 0;
}

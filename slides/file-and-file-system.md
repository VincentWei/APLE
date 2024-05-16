# 文件和文件系统

1. 内存管理回顾：OOM 的产生
1. STDIO vs 文件读写系统调用
1. 文件描述符
1. 文件共享、重定向及文件锁
1. 终端和伪终端
1. 文件系统操作

		
## 内存管理回顾

- 焦点问题：OOM 的产生
   1. 进程的虚拟地址使用达到设定的资源限制 `getrlimit()`/`setrlimit()`
   1. 进程的虚拟地址空间被耗尽
   1. 机器的物理内存（交换空间）被耗尽

		
## STDIO vs 文件读写系统调用

- 问题
   1. STDIO 接口的主要功能有哪些？
   1. 读写文件时应使用 STDIO 还是系统调用，哪个效率高？

	
- STDIO 的缓冲模式
   1. 行缓冲
   1. 块缓冲/完全缓冲
   1. 无缓冲
   1. 改变缓冲模式 `setvbuf()`/`setbuf()`/`setbuffer()`/`setlinebuf()`
   1. 问题：键盘上的输入何时才能被程序获得？

	
- 基本的文件读写系统调用
   1. `creat()`/`open()`
   1. `read()`/`write()`
   1. `lseek()`
   1. `close()`
   1. `fcntl()`

	
- `FILE` 和 `fd` 互操作
   1. `fdopen()`：已知文件描述符创建流对象
   1. `fileno()`：已知流对象获取对应文件描述符
- 鲜为人知的 STDIO 接口
   1. `fmemopen()`：将内存区域抽象为流对象
   1. `freopen()`：关闭一个流对象并使用新模式打开
   1. `funopen()`：自定义流对象
   1. `fropen()`：自定义只读流对象
   1. `fwopen()`：自定义只写流对象

	
- 从文件接口看抽象在软件设计中的重要性
   1. 所有支持读取/写入/定位/关闭这四个操作的东西，都可以被抽象为文件
   1. 真实的文件、内存区域、各种外设到管道、套接字…
- 从软件操作接口的多重层级看软件栈的构成
   1. 现代操作系统是不同层级的软件一层层堆砌起来的
   1. 不同层级的软件模块实现不同的机制，帮助上层软件实现不同的使用策略：内核的基本文件系统调用实现了基本的文件读写，为带有缓冲策略的 STDIO 提供实现基础
   1. 问题：`fflush()`/`fpurge()` 和 `fsync()` 有何区别？
   1. 示意图（下页）

	
<img class="r-frame" style="height:auto;width:80%;" src="assets/aple-fflush-fsync.png" />

		
## 文件描述符

- 文件描述符是一个非负整数（`int` 型）
- 每个进程维护有一个已使用的文件描述符表
- 分配新文件描述符时始终返回最小未被分配的文件描述符
- 从 `dup()` 到 `dup2()`
- 文件描述符相关的标志：
   1. 文件状态标志：`O_NONBLOCK`、`O_APPEND`、`O_ASYNC`
   1. close-on-exe 标志（文件描述符标志）
- Linux 内核如何维护打开的文件？
   1. 文件描述符相关的标志和文件读写位置在哪里保存？
   1. 示意图（下页）

	
<img class="r-frame" style="height:auto;width:100%;" src="assets/aple-fd-data-structures.png" />

		
## 文件共享、重定向及文件锁

- 父子进程共享所有数据和代码，也共享文件描述符
   1. colse-on-exe 标志的重要性
   1. 问题：fork() 的资源成本有多大？
- 通过 UNIX domain socket 发送文件描述符
   1. 分配并共享大块内存 `memfd_create()` -> `sendmsg()` -> `mmap()`
- 重定向的实现
   1. `dup2()`
- 文件锁
   1. `flock()` 仅用于父子进程之间，且只是建议锁（advisory lock）
   1. `flock()` 的锁粒度太大（整个文件）
   1. `fcntl()`/`lockf()` 可实现强制锁（mandatory lock）
   1. 为什么方同学的代码没有出现写入混乱？

	
- 程序源代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_PROCESSES 5
#define DATA_SIZE 5

void writeToFile(int processID, FILE *file) {
    sleep(1);
    for (int i = 0; i < DATA_SIZE; ++i) {
        fprintf(file, "Process %d: Data %d: fd %d\n", processID, i, fileno(file));
    }
}

int main(void) {
    FILE *file;
    file = fopen("output.txt", "w");

    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    ｝

    pid_t pid;
    for (int i = 0; i < NUM_ PROCESSES; ++i) {
        pid = fork();
        if (pid < 0) {
           perror("Fork failed");
           exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            // Child process
            writeToFile(i, file);
            exit(EXIT_SUCCESS);
        }
    }

    // Parent process waits for all child processes to complete
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        wait(NULL);
    }
    fclose(file);
    return 0;
｝
```

	
<img class="r-frame" style="height:auto;width:100%;" src="assets/aple-fwrite-sample.png" />

	
- 讨论：内核中打开的文件是以进程为单位维护还是全局的？

		
## 终端和伪终端

- terminal vs console
   1. 真实终端：`/dev/tty0 ~ 63`；`/dev/ttyS0`…
   1. 虚拟终端：`/dev/pts/0`…，`/dev/pts/ptmx` (`/dev/ptmx`)
   1. 别名：`/dev/tty`；`/dev/console`
- 终端模式
   1. `$ man ioctl_console`
- 伪终端
   1. 模拟真实终端行为，如 GUI 中的 xterm，telnet/ssh 等
   1. `posix_openpt()`, `getpt()`, `ptsname()`
- 示意图（下页）


	
<img class="r-frame" style="height:auto;width:100%;" src="assets/aple-pty.png" />

		
## 文件系统操作

- 常用接口
   1. STDC: `fopen()`, `remove()`, `rename()`
   1. POSIX: `creat()`, `unlink()`, `rmdir()`, `renameat()`, `mkdir()`, …
- 遍历目录项：`opendir()`, `closedir()`, `readdir()`,  `rewinddir()`,  `scandir()`, `seekdir()`, `telldir()`, …
- 文件属性：`chmod()`, `chown()`, `stat()`, `umask()`, …

	
- 案例分析
   1. 获取指定路径的挂载点
   1. 创建临时文件

		
## 作业

- 阅读开源文档及代码：
   1. Glib 中的文件操作和文件系统接口封装  
   <https://docs.gtk.org/glib>  
   <https://github.com/GNOME/glib>
- 改写方同学的程序，使之很容易重现写入混乱；进一步增强上述程序，使之彻底避免写入混乱。
- 编写一个程序，启动 `/bin/bc` 并通过伪终端驱动 `/bin/bc` 完成任意精度的四则表达式计算。
- 编写一个 Shell 脚本和一个 C 程序用于遍历文件系统，给出指定路径下所有文件和目录项的大小总和。

		
## 第三讲预习内容

- 信号及进程管理
   1. 信号的作用以及常见信号
   1. 信号的替代机制
   1. 进程的创建和执行一个新程序的过程
   1. 守护进程的创建


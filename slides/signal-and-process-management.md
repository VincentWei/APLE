# 信号和进程管理

- 文件和文件系统回顾
- 信号的概念、处置和沿革
- 信号相关函数
- Linux 特有的 signalfd 和 timerfd
- 进程的创建和程序的执行
- 会话、进程组和守护进程

		
## 文件和文件系统回顾

- 父子进程`共享`文件描述符的说法是否正确？

		
## 信号的概念、处置和沿革

- 信号是异步事件
   1. 异步/同步：不可预见/可预见
   1. 一般的 UNIX 系统有 30 或 31 种信号，Linux 有 64 种信号
   1. 信号有自己的名称和对应的编号  
   `$ kill -l`
   1. 正确区别信号和异常处理机制

	
### 常见信号的产生条件

- 用户在按下特定的键之后，将向该终端上的前台进程组发送信号。比如，`Ctrl+C`。
- 硬件异常会产生信号：比如被 0 除，无效内存引用等。
- `kill(2)` 系统调用可允许进程向其他进程或进程组发送任意信号。
- `kill(1)` 命令允许用户向进程发送任意信号。
- 软件设置的条件，比如 `SIGALARM`。

	
### Linux 上的信号

```console
$ kill -l
 1) SIGHUP	 2) SIGINT	 3) SIGQUIT	 4) SIGILL	 5) SIGTRAP
 6) SIGABRT	 7) SIGBUS	 8) SIGFPE	 9) SIGKILL	10) SIGUSR1
11) SIGSEGV	12) SIGUSR2	13) SIGPIPE	14) SIGALRM	15) SIGTERM
16) SIGSTKFLT	17) SIGCHLD	18) SIGCONT	19) SIGSTOP	20) SIGTSTP
21) SIGTTIN	22) SIGTTOU	23) SIGURG	24) SIGXCPU	25) SIGXFSZ
26) SIGVTALRM	27) SIGPROF	28) SIGWINCH	29) SIGIO	30) SIGPWR
31) SIGSYS	34) SIGRTMIN	35) SIGRTMIN+1	36) SIGRTMIN+2	37) SIGRTMIN+3
38) SIGRTMIN+4	39) SIGRTMIN+5	40) SIGRTMIN+6	41) SIGRTMIN+7	42) SIGRTMIN+8
43) SIGRTMIN+9	44) SIGRTMIN+10	45) SIGRTMIN+11	46) SIGRTMIN+12	47) SIGRTMIN+13
48) SIGRTMIN+14	49) SIGRTMIN+15	50) SIGRTMAX-14	51) SIGRTMAX-13	52) SIGRTMAX-12
53) SIGRTMAX-11	54) SIGRTMAX-10	55) SIGRTMAX-9	56) SIGRTMAX-8	57) SIGRTMAX-7
58) SIGRTMAX-6	59) SIGRTMAX-5	60) SIGRTMAX-4	61) SIGRTMAX-3	62) SIGRTMAX-2
63) SIGRTMAX-1	64) SIGRTMAX
```

	
### 信号的处置

1. 忽略信号。有两个信号永远不能忽略：`SIGKILL` 和 `SIGSTOP`，它们为超级用户提供了杀死和停止进程的必要方法。
   - 问题：忽略硬件异常产生的信号会出现什么样的问题?
1. 捕获信号。告诉内核在出现信号时调用我们自己定义的处理函数。比如可以通过处理 `SIGCHLD` 信号并调用 `waitpid(2)` 获得子进程的退出状态，以避免生成僵尸进程。
1.  使用默认动作。每个信号有其默认动作。

- 某些信号不能被忽略，也不能被捕获。

	
### POSIX 定义的信号

```
       信号         值      动作     注释
       -------------------------------------------------------------------------
       SIGHUP        1        A      Hangup detected on controlling terminal
                                     or death of controlling process
       SIGINT        2        A      Interrupt from keyboard
       SIGQUIT       3        A      Quit from keyboard
       SIGILL        4        A      Illegal Instruction
       SIGABRT       6        C      Abort signal from abort(3)
       SIGFPE        8        C      Floating point exception
       SIGKILL       9       AEF     Kill signal
       SIGSEGV      11        C      Invalid memory reference
       SIGPIPE      13        A      Broken pipe: write to pipe with no readers
       SIGALRM      14        A      Timer signal from alarm(2)
       SIGTERM      15        A      Termination signal
       SIGUSR1   30,10,16     A      User-defined signal 1
       SIGUSR2   31,12,17     A      User-defined signal 2
       SIGCHLD   20,17,18     B      Child stopped or terminated
       SIGCONT   19,18,25            Continue if stopped
       SIGSTOP   17,19,23    DEF     Stop process
       SIGTSTP   18,20,24     D      Stop typed at tty
       SIGTTIN   21,21,26     D      tty input for background process
       SIGTTOU   22,22,27     D      tty output for background process
-------------------------------------------------------------------------------
    * 信号值是 Linux 系统在不同平台上的取值, 第一个数对 Alpha 和 Sparc 有效, 中
      间的数对 i386 和 PowerPC 有效, 最后一个数对 MIPS 有效. "-" 表现缺少该信号.

    * A: 默认动作是终止(abort)进程
    * B: 默认动作是忽略信号
    * C: 默认动作是吐核（core dumped)
    * D: 默认动作是停止(stop)进程
    * E: 信号不能被捕获
    * F: 信号不能被忽略
```

	
### 信号的沿革

1. 信号出现在 UNIX 的早期版本中。
1. 早期的信号模型是不可靠的，信号可能会丢失，也很难处理临界段。
1. UNIX 的两个重要分支 BSD 和 System V 分别对早期的信号进行了扩展；但这两个系统的扩展并不兼容。
1. POSIX 统一了这两种实现，最终提供了可靠的信号模型。
1. Linux 从 2.6.27 开始提供 signalfd 机制，用于通过文件描述符来处理信号。

	
### 信号处理函数的可重入性

1. 函数中使用全局变量或静态变量就容易导致不可重入。
1. 在信号处理函数（异步）、多个线程（并发）中调用不可重入的函数会导致严重的逻辑问题。
1. 但在非异步和非并发情况下，不可重入函数也可能导致问题。
1. 本质上，所有的不可重入函数都通过全局变量维护上下文信息。
1. 信号处理函数应被设计为可重复函数。
1. 由于 `errno` 的特殊性，如果信号处理函数调用了可能会修改 `errno` 的函数，则应该保存并恢复 `errno` 的值。

	
### 打断的系统调用

- 进程调用“慢”系统调用时，如果产生信号，内核会终止该系统调用，以便让进程有机会处理信号。
- 慢系统调用：可能会永远阻塞的系统调用
   1. 从终端设备、管道和网络设备中读取数据。
   1. 向上述文件写入数据。
   1. 某些设备上的文件打开。
   1. `pause()` 、`wait()`、`sleep()` 等系统调用。
   1. 某些 `ioctl()` 操作，某些进程间通讯函数。
- 发生中断系统调用时, 被中断的系统调用返回错误值, 而 `errno` 被设置为 `EINTR`。
- BSD/Linux 对某些系统调用被打断时，可自动重新启动该系统调用。


	
### Linux 特有信号

```
       Signal       Value     Action   Comment
       ---------------------------------------------------------------------
       SIGTRAP        5         CG     Trace/breakpoint trap
       SIGIOT         6         CG     IOT trap. A synonym for SIGABRT
       SIGEMT       7,-,7       G
       SIGBUS      10,7,10      AG     Bus error
       SIGSYS      12,-,12      G      Bad argument to routine (SVID)
       SIGSTKFLT    -,16,-      AG     Stack fault on coprocessor
       SIGURG      16,23,21     BG     Urgent condition on socket (4.2 BSD)
       SIGIO       23,29,22     AG     I/O now possible (4.2 BSD)
       SIGPOLL                  AG     A synonym for SIGIO (System V)
       SIGCLD       -,-,18      G      A synonym for SIGCHLD
       SIGXCPU     24,24,30     AG     CPU time limit exceeded (4.2 BSD)
       SIGXFSZ     25,25,31     AG     File size limit exceeded (4.2 BSD)
       SIGVTALRM   26,26,28     AG     Virtual alarm clock (4.2 BSD)
       SIGPROF     27,27,29     AG     Profile alarm clock
       SIGPWR      29,30,19     AG     Power failure (System V)
       SIGINFO      29,-,-      G      A synonym for SIGPWR
       SIGLOST      -,-,-       AG     File lock lost
       SIGWINCH    28,28,20     BG     Window resize signal (4.3 BSD, Sun)
       SIGUNUSED    -,31,-      AG     Unused signal
-------------------------------------------------------------------------------
    * 信号值是 Linux 系统在不同平台上的取值, 第一个数对 Alpha 和 Sparc 有效, 中
      间的数对 i386 和 PowerPC 有效, 最后一个数对 MIPS 有效. "-" 表现缺少该信号.

    * A: 默认动作是终止进程
    * B: 默认动作是忽略信号
    * C: 默认动作是 core 转储
    * D: 默认动作是停止进程
    * E: 信号不能被捕获
    * F: 信号不能被忽略
    * G: 非 POSIX 信号
```

	
### Linux 的信号处理

- Linux 中没有信号的优先级，也不会对信号做排队处理。
- Linux 的每个进程有两个 64 位字，分别表示当前挂起的信号以及阻塞的（blocked）信号。64 位字的每一位对应一个信号。
- 如果产生信号，且该信号不是 SIGKILL 或 SIGSTOP，则当该信号没有被阻塞时才会被处理；否则就是挂起的（pending）信号。
- Linux 用 sigaction 结构表示每个信号的处理方式。
- 信号发生时并不会立即得到处理，处理时机：
   1. 每次进程从系统调用中退出时；
   1. 内核在调度程序中选择执行该进程时。

	
- 如果有任何一个未被阻塞的信号发出，内核就根据 `sigaction` 结构数组中的信息进行处理：
   1. 检查对应的 `sigaction` 结构，如果该信号不是 `SIGKILL` 或 `SIGSTOP` 信号，且被忽略，则不处理该信号。
   1. 如果该信号利用默认的处理程序处理，则由内核处理该信号，否则转向第 3 步。
   1. 该信号由进程自己的处理程序处理，内核将修改当前进程的调用堆栈帧，并将进程的程序计数寄存器修改为信号处理程序的入口地址。此后，指令将跳转到信号处理程序，当从信号处理程序中返回时，实际就返回了进程的用户模式部分。

	
### 不可靠信号处理机制

```c
#include <signal.h>
void (*signal(int signum, void (*handler)(int)))(int);
```

- 利用 `signal()` 函数可设置某个信号的关联动作：捕获、忽略和默认处理。
- `handler` 可取如下值:
   * 用户定义的信号处理器
   * `SIG_IGN`：忽略信号
   * `SIG_DFL`：重置为信号的默认处理
- 返回值为先前的设置, 若发生错误, 则返回 `SIG_ERR`。
- Linux 的 `signal()` 函数有如下特点：
   * 在可靠信号系统调用的基础上实现，是库函数。
   * 默认的原语类似 System V，即信号发生时，信号的处置重置为默认值。
   * 如果包含 `<bsd/signal.h>`，则具有 BSD `signal()` 的原语，即发生信号时，信号处置的设置值不变。
- `SIG_IGN`, `SIG_DEF` 和 `SIG_ERR` 的定义：

```c
    #define SIG_ERR (void (*)()) -1
    #define SIG_DFL (void (*)()) 0
    #define SIG_IGN (void (*)()) 1
```

	
- 发生信号时，信号的关联动作被重置为默认设置。这可能导致信号丢失：
   - 在进入 `sig_int` 与再次调用 `signal()` 之间发生的 `SIGINT` 信号将不会被捕获，而由于默认设置将最终导致进程终止。

```c
    void sig_int ();             /* my signal handling function */
    ...
    signal (SIGINT, sig_int);    /* establish handler */
    ...

    sig_int ()
    {
        signal (SIGINT, sig_int);
                                /* reestablish handler for next occurrence */
        ...                     /* process the signal ... */
    }
```

	
- 无法暂时关闭信号（即阻塞信号），而只能忽略信号，这同样会导致信号丢失。
   - 正常情况下, 下面的程序将在发生 `SIGINT` 信号时继续运行。
   - 但在测试 `sig_int_flag` 的值为零, 然后在调用 `pause()` 之前发生了 `SIGINT` 信号，则 `pause()` 函数将导致进程永远休眠。

```c
    int sig_int_flag;                  /* set nonzero when signal occure */

    int main (void)
    {
        void sig_int ();               /* my signal handling function */
        ...
        signal (SIGINT, sig_int);      /* establish handler */
        ...
        while (sig_int_flag == 0)
            pause ();                  /* go to sleep, waiting for signal */
        ...
    }

    void sig_int ()
    {
        signal (SIGINT, sig_int);
                               /* reestablish handler for next occurrence */
        sig_int_flag = 1;      /* set flag for main loop to examine */
    }
```

	
### 可靠信号处理

- 信号产生：当导致信号的事件发生时，也就是发送到进程时。
- 信号递达：当信号的动作发生时。
- 进程可阻塞信号的递达。产生被进程阻塞的信号时，如果信号的动作是默认动作和捕获信号，则下列情形发生之前，信号将保持挂起：
   - 进程解开信号的阻塞
   - 改变动作为忽略
- 系统在信号递达时才考虑如何处理阻塞信号，而不是信号产生时
- 如果阻塞信号再次产生，则系统可选择多次递达信号或只递达一次。前者称为排队信号。常见系统对信号不进行排队，Linux 也不排队。
- 每个进程有一个定义当前阻塞信号集的信号掩码。
- 信号集和 `select()` 函数中的文件描述符集一样, 是 POSIX 引人的新数据类型, 用来定义信号集合。

	
- 信号集及操作

```c
#include <signal.h>

int sigemptyset (sigset_t *set);
int sigfillset (sigset_t *set);
int sigaddset (sigset_t *set, int signum);
int sigdelset (sigset_t *set, int signum);
int sigismember (const sigset_t *set, int signum);
```

	
- 可靠信号系统调用

```c
#include <signal.h>

struct sigaction {
    void (*sa_handler) (int);
    void (*sa_sigaction) (int, siginfo_t *, void *);
    sigset_t sa_mask;
    int sa_flags;
}

int sigaction (int signum,  const  struct  sigaction  *act, struct sigaction *oldact);
int sigprocmask (int  how,  const  sigset_t *set, sigset_t *oldset);
int sigpending (sigset_t *set);
int sigsuspend (const sigset_t *mask);
```

	
- `sigaction()`：用于改变进程在接收到信号时的动作。
   * 可指定除 `SIGKILL` 和 `SIGSTOP` 之外的信号的动作 (`act->sa_handler`)，可取 `SIG_DFL`， `SIG_IGN` 和用户定义函数。
   * 可指定在处理 `signum` 信号时，应该阻塞的信号集 (`act->sa_mask`)。
   * 可指定信号处理过程中的行为 (`act->sa_flags`)。
   * `oldact` 非空时, 可返回先前的设置。
- `sigprocmask()`：用于改变进程的当前阻塞信号集.
   * `how` 可取 `SIG_BLOCK`、`SIG_UNBLOCK` 和 `SIG_MASKSET`；前两个动作分别在当前阻塞信号集中添加或删除由 `set` 指定的信号集，`SIG_MASK` 用于完全设置阻塞信号集。
   * `oldset` 非空时，可返回先前的设置。
- `sigpending()`：用于检验挂起的信号。
- `sigsuspend()`： 用于在接收到某个信号之前, 临时用 `mask` 替换进程的信号掩码，并暂停进程执行。
    * 该函数返回后将恢复调用之前的信号掩码。
    * 该系统调用始终返回 -1, 并将 `errno` 设置为 `EINTR`。
    * 该系统调用实际是阻塞并暂停两个动作的原子操作。

		
## 信号相关函数

```c
#include <sys/types.h>
#include <signal.h>

int kill (pid_t pid, int sig);
int raise (int sig);
```

- `kill()`: 向其它进程发送信号。
- `raise()`: 向当前进程发送信号。
- `kill()` 的 `pid` 参数的四种条件：
   * `pid > 0：信号发送到 PID 为 `pid` 的进程。
   * `pid == 0`：信号发送到与发送进程处于同一进程组的进程。
   * `pid < -1`：信号发送到进程组 ID 等于 `-pid` 的所有进程。
   * `pid == -1`：POSIX 未指定；Linux 发送到进程表中所有的进程，除第一个进程之外。
   * `sig == 0` 时，不会发送任何信号，但仍然执行错误检查，因此可用来检查是否有向指定进程发送信号的许可。


	
```c
#include <unistd.h>
unsigned int alarm (unsigned int seconds);

#include <sys/time.h>
int getitimer (int which, struct itimerval *value);
int setitimer (int which, const struct itimerval *value, struct itimerval *ovalue);
```

- `alarm()` 可设置定时器；定时器到期时，将产生 `SIGALRM` 信号。
- `setitimer()` 是另一个设置间隔定时器的函数，更加灵活。
- `alarm()` 的返回值为先前尚未过期的定时器所剩余的时间。
- `alarm()` 和 `setitimer()` 使用同一个定时器，因此会互相影响。
- `setitimer()` 定时精度要比 `alarm()` 高，同时可以按照进程的执行时间（用户态时间、核心态时间或两者）进行计时。

	
```c
#include <unistd.h>

int pause (void);
```

- `pause()` 暂停当前进程并等待信号。

	
```c
       #include <stdlib.h>
       void abort (void);
```

- `abort()` 向当前进程发送 `SIGABRT` 信号，从而可导致程序非正常终止。
   * 如果捕获 `SIGABRT` 信号，且信号处理器不返回，则 `abort()` 不能终止进程。
   * `abort()` 终止进程时，所有打开的流均会被刷新并关闭。
   * 如果 `SIGABRT` 被阻塞或忽略，`abort()` 将覆盖这种设置。

	
```c
#include <unistd.h>

unsigned int sleep (unsigned int seconds);
```

- `sleep()` 休眠指定的秒数。
   * 经过指定的时间之后，`sleep()` 返回。
   * 在返回之前如果接收到信号，则 `sleep()` 返回剩余的秒数。

		
## 线程中的信号处理

- 按照 POSIX 标准，异步（外部）信号发送到整个进程。
- 所有线程共享同一个设置，即通过 `sigaction()` 设置的线程处置方法。
- 每个线程有自己的信号掩码，内核根据该掩码决定将信号发送到哪个线程。
- Linux 历史上有两个线程实现（LinuxThreads 和 NPTL），LinuxThreads 不兼容 POSIX，NPTL 实现则完整兼容 POSIX 标准。
- `pthread_sigmask()` 用来定义线程的信号掩码，其接口与 `sigprocmask()` 一样。
- `pthread_kill()` 向特定的线程发送信号。
- `sigwait()`/`sigtimedwait()` 暂停调用线程，直到信号集中定义的某个信号递达调用线程。

		
## Linux 特有的 signalfd 和 timerfd

- signalfd：通过文件描述符接收信号
- timerfd：通过文件描述符处理定时器

```c
#include <sys/signalfd.h>

int signalfd(int fd, const sigset_t *mask, int flags);

#include <sys/timerfd.h>

int timerfd_create(int clockid, int flags);
int timerfd_settime(int fd, int flags,
                   const struct itimerspec *new_value,
                   struct itimerspec *old_value);
int timerfd_gettime(int fd, struct itimerspec *curr_value);
```

		
## 进程的创建和程序的执行

- `fork()`：派生子进程
   - fork 之后，子进程和父进程共享打开的文件，这是实现管道等 IPC 机制的基础。
   - Linux 利用按需分页 (paging on demand) 和写时复制 (copy-on-write) 方式派生子进程。
- `vfork()`：用于派生后立即执行的情况
   - 不复制父进程的全部地址空间。
   - `vfork()` 保证子进程首先运行, 直到调用 `exec()` 或 `exit()` 之后，父进程才能得到运行机会。

	
### 进程的退出

- `exit()` 是标准C库函数。
- `exit()` 调用由 `atexit()` 注册的清除函数, 并关闭所有的标准 I/O 流。
- `exit()` 调用 `_exit()` 处理与 UNIX 相关的内容，比如文件描述符，作业控制等。
- 进程的退出状态非常重要，一般以 0 值表示正常退出。
- `_exit()` 是系统调用。

	
### 等待子进程退出

- 进程的退出状态必须用 `wait()` 函数由父进程获取，否则形成僵尸进程。
- `wait()` 等待某个子进程退出；`waitpid()` 可等待指定的子进程退出，可指定 `WNOHANG` 选项而不阻塞调用进程的执行。
- `wait3()` 和 `wait4()` 分别是 `wait()` 和 `waitpid()` 的扩展调用，它们可以返回子进程的资源使用情况。

```c
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait (int *status)
pid_t waitpid (pid_t pid, int *status, int options);

#define _USE_BSD

#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>

pid_t wait3 (int *status, int options, struct rusage *rusage);
pid_t wait4 (pid_t pid, int *status, int options, struct rusage *rusage);
```

	
### 执行程序

- 在进程中执行一个程序，将完整替代当前进程的数据和代码（但保留没有 close-on-exec 标志的文件描述符），并从程序的 `main()` 函数入口处继续执行。
- `system()` 函数的实现。

	
### 进程关系

- 进程的派生关系构成了一个以 `init` 进程为祖先的进程树。
- `pstree` 命令。

		
## 会话、进程组和守护进程

	
### 进程组

- Shell 上的一条命令行形成一个进程组。
- 每个进程属于一个进程组。
- 每个进程组有一个领头进程。
- 进程组的生命周期到组中最后一个进程终止, 或加入其他进程组为止。
- `getpgrp()`：获得进程组 id, 即领头进程的 pid。
- `setpgid()`：加入进程组和建立新的进程组
   - 进程只能将自身和其子进程设置为进程组 id。
   - 某个子进程调用 `exec()` 函数之后，就不能再将该子进程的 id 作为进程组的 id。
- 前台进程组和后台进程组

	
### 会话

- 一次登录形成一个会话。
- 一个会话可包含多个进程组，但只能有一个前台进程组。
- `setsid()` 可建立一个新的会话。
- 会话的领头进程打开一个终端之后, 该终端就成为该会话的控制终端 (SVR4/Linux)。
- 与控制终端建立连接的会话领头进程称为控制进程。
- 一个会话只能有一个控制终端。
- 产生在控制终端上的输入和信号将发送给会话的前台进程组中的所有进程。
- 终端上的连接断开时（比如网络断开），挂起信号将发送到控制进程。
- 示意图（下页）

	
<img class="r-frame" style="height:auto;width:100%;" src="assets/aple-session.png" />

	
### 守护进程

- 守护进程就是在后台执行, 没有控制终端或登录 Shell 的进程。
   1. 关闭文件描述符: `for (i=0; i < NOFILE; i++) close (i);`
   1. 甩开控制终端: 打开 `/dev/tty`，然后用 `ioctl()` 在该文件上执行 `TIOCNOTTY` 命令。
   1. 脱离会话和进程组
   1. 改变工作目录到根目录: `chdir (“/");`
   1. 进程的 umask 设置为 0: `umask (0);`
   1. 处理 `SIGCHLD` 信号, 为避免形成僵尸进程。

		
## 作业

1. 信号及进程管理
1. 分别使用 `setitimer()`/`alarm()`，`timerfd_create()` 、应用层轮询系统时间的方式实现一个可以设置多个定时器的模块。
1. 编写一个简单的守护进程程序，可替代 `cron` 的功能：
   - 读取指定配置文件，按给定的时间周期执行配置文件中指定的程序。
   - 将守护进程的执行情况以及各周期性执行的程序的输出和退出状态记录到日志文件中。

		
## 第四讲预习

- 进程间通讯
   1. 管道和命名管道
   1. SystemV 的进程间通讯机制：消息队列、共享内存、信号量
   1. BSD 引入的 UNIX 域套接字
   1. Linux 引入的 memfd、eventfd
   1. 思考：如何选择不同的 IPC 机制？

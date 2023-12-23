
# Homework
## Exercise 1
> 实现一个多进程的生产者/消费者模型：
>    - 父进程接收用户输入的一个目录，然后对其中的每个目录项启动一个子进程；
>    - 子进程递归统计每个目录项的磁盘占用情况；
>    - 父进程等待子进程完成统计工作，然后再输出其总和。

### 使用多线程实现生产者消费者
```
cd exercise_1/thread
make && ./test
```
### 使用多进程实现生产者消费者
```
todo!
```
## Exercise 2
> 基于 UNIX Domain Socket 实现一个服务器程序和客户程序：
> - 服务器进程接受来自客户进程的一个四则运算表达式，并返回求值结果（可调用 bc 程序完成表达式求值）。
> - 客户进程接受来自用户的输入，发送给服务器，等待服务器返回
结果并输出。

```
cd exercise_2
make 
./server.out
./client.out
```


# Notes
## 互斥锁和自旋锁(spin lock)
互斥锁: 一般是用户态
自旋锁: 用于多核CPU的忙等待，用户空间很少用到，一般是内核会用到
## 建议锁(advisory)和强制锁(mandatory)的区别
建议锁: 内核不管，结果自负，自己可以选择是否使用
## 信号量(semaphore用的非常多)
支持Up, Down操作。
- 最早用于同步，生产者消费者模型用的特别多。对于一个队列，有多个读者和写者，读完了就得等着。
- 也可以用于互斥: 比如只有0/1信号量
## 互斥锁(mutex)和读写锁(write lock: exclusive lock; read lock: shared lock)的区别

## file record lock
- lockf()可以锁住文件的某个区间，主要用于数据库；
- fcntl 也可以提供记录锁，或者文件的修改通知机制(文件夹下显示文件下载进度)

## SystemV
- 信号量(semaphore)
- 共享内存(shared memory),逐渐用memfd_create()和fd传递来取代SystemV
信号量的开销特别小。
SystemV信号量特点，一次可以创建信号量集合。

## UNIX域套接字
两种类型：
    - 可靠双向的面向连接，stream，流
    - 不可靠的无链接，datagram, 数据报
可以在进程见传递文件描述符。

UNIX域套接字，可以用于多线程，直接拿到fd就可以了。
int socketpair(int domain, ..)

### 数据报套接字
无需调用listen()和accept(), 不会产生SIGPIPE信号，因为他没有链接。
采用sendmsg()传递文件描述符，由于共享内存只能操作系统内存，而不能操作显存。

`cat /proc/net/unix`可以查看所有的UNIX域套接字,没有路径的就是没有bind的client。

### Linux特有的
memfd_create(), 基于文件描述符实现，可以用于select(), epoll()
eventfd(), 多个进程、线程之间可以代替信号量，开销非常小，并且可以和select，epoll一起用。有数据马上就返回。

## 进程和线程区别的
线程多了个条件变量和单次函数。

## Resources
- https://github.com/Akagi201/linux-0.11/
- https://github.com/Akagi201/tinnyhttpd/

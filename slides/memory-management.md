# 内存管理

1. 虚拟内存原理
1. C /C++ 程序中堆/栈的管理
1. 常见内存使用问题
1. 大型项目的内存管理
1. 脚本语言解释器的内存管理

		
## 虚拟内存原理

- MMU 和页表
   1. 内核始终以页（4KB）为单位分配物理内存
   1. 一个虚拟内存页可能对应一个物理内存页，也可能对应一个文件区域。
   1. 页表：逻辑地址到物理地址的映射表。
- 问题
   1. 页表是以进程为单位还是以处理器为单位？
   1. 页表本身保存在哪里？
   1. 估计一个最简单的 C 程序进程需要多少页表项？

	
- 虚拟内存技术带来的好处
  1. 给进程间的内存共享带来便利：多个进程使用同一个共享库时大部分数据（如代码）可在进程间共享。
  1. 通过交换分区/文件，来扩大可用的内存。
  1. 通过写时复制、用时分配等技术，提高系统整体性能。
  1. ……
- 虚拟内存技术带来的坏处
  1. 丧失确定性

		
## C/C++程序的堆/栈管理

### 接口

- 我们熟知的：
   1. C: `malloc`/`calloc`/`realloc`/`free`
   1. C++: `new`/`delete`
- 我们不熟悉的：
   1. `alloca`，`VLA`
   1. `posix_memalign`/`aligned_alloc`
   1. `brk`/`sbrk`
   1. `etext`/`edata`/`end`

	
### C/C++ 进程的内存布局

- text segment
- initialized data segment
- uninitialized data segment (BSS, block started by symbol)
- stack/stack frame
- 示意图

	
<img class="r-frame" style="height:auto;width:80%;" src="assets/aple-memory-layout.png" />

	
### 常用工具

- `nm` (name list, symbol table)
- `strings`
- `size`
- `readelf`
- `objdump`

	
### 其他分配内存的方法

1. 匿名内存映射 `mmap()`
1. memfd：`memfd_create()`
1. 问题：大块连续的虚拟内存，对应物理内存是否连续？

	
### 堆管理算法

1. 最简单的堆管理算法：Buddy 分配器
1. 内核中的 Slab 分配器

		
## 常见内存使用问题

- OOM
- 不对齐访问（unaligned access)
- 越界访问或非法指针
- 内存泄露
- …

	
### 术语

- use-after-free
- use-after-return
- use-after-scope
- heap-buffer-overflow/heap-buffer-underflow
- stack-buffer-overflow/stack-buffer-underflow
- global-buffer-overflow/global-buffer-underflow
- double-free
- memory leaks
- Initialization order bugs

	
### 内存使用问题的诊断

- 越界访问或非法地址导致的现象
- 不对齐访问导致的现象
- 出现内存泄露时的现象
- 栈被破坏时的现象
- 诊断内存使用问题的利器：efence、asan、valgrind

	
### 内存检测工具的实现原理

- 如何诊断内存的越界访问？
- 如何诊断释放后仍然使用？

	
### 堆内存的碎片化

- 什么情况下会出现堆的碎片化？
- 假如要支持碎片化整理，接口该如何设计？

		
## 大型项目的内存管理

- 自定义堆管理
   1. WTF 中的 `bmalloc`, `libpas`
   1. Glib 提供的 `g_slice` 分配接口

		
## 脚本语言解释器的内存管理

- JavaScript/Python
   1. 结构封装：JSValue/PyObject
   1. 垃圾回收；强引用/弱引用
- HVML 解释器 PurC 变体
   1. 结构封装：variant
   1. 引用计数，无垃圾回收器
   1. 跨线程的数据传递：move heap

		
## 高效使用内存

- 高效使用内存的技巧/代码分析
   1. 能用栈解决的问题，绝不用堆
   1. 尽量分配小块内存，避免频繁分配大块内存
   1. 当需要频繁、大量分配同样大小的内存时，使用 `g_slice` 或类似机制进行管理

		
## 作业

- 阅读开源代码：
   1. Glib 中 `g_slice` 分配器  
   <https://github.com/GNOME/glib>
   1. WTF 中的 `bmalloc` 分配器和 `libpas`  
   <https://github.com/WebKit/WebKit>
   1. PurC 中的变体管理及 move heap  
   <https://github.com/HVML/PurC>
- 用 Buddy 算法实现一个私有堆的管理模块，并和 C 库的 `malloc`/`free` 对比性能
   1. 要求使用线程局部存储来管理线程私有堆。
   1. 衡量私有堆的内存使用效率。

		
## 第二讲预习内容

- 文件及文件系统
   1. STDIO 的主要接口，文件描述符和 FILE 的关系
   1. 重定向和管道的实现机制
   1. 文件系统的操作接口
   1. Linux 中常用的文件系统类型


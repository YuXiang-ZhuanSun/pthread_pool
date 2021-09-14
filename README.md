# 线程池项目描述

基于POSIX Thread构建了一个线程池库。

线程池库包括`thread_pool`头文件和`thread_pool.c`函数库实现文件。

主要提供了以下接口：

```c
//create a thread pool
thread_pool * create_thread_pool(int thread_limit,int task_limit);
//thread_limit:线程池总线程数
//task_limit:最大任务数组大小，待领取任务来到此限制后再添加任务线程会睡眠直到有任务被领走

//add new task
int add_task(thread_pool* p, void *(*func)(), void * arg);
//func函数就是自定义的函数任务函数，用于线程领取任务后调用
//arg是func函数需要的void *类型（无类型指针类型）参数
```

使用者需要自定义`func`函数，作为**任务函数**：实际完成一个任务的函数，多线程程序会自觉领取任务后调用这个函数。

`func`函数原型如下：

```c
void * func(void * arg);
```

## 核心技术

* **线程池理念**：当需要执行的任务很多，不可能为每个任务创建一个线程。
  * 重复创建线程和回收线程的消耗大量CPU资源。
  * 如果有1,000,000个任务，能开1,000,000个线程吗？计算机CPU核心数量则很少。线程数应该与CPU核心数匹配。
* **生产者消费者模型：**环形任务数组，工作线程作为消费者，从线程池中领取任务，任何线程可以通过`add_task`函数往线程池中添加任务。
* **线程同步：**
  * 操作环形任务数组时，必须为互斥操作，所以使用了互斥锁mutex。
  * 当任务数组为空时，工作线程需要进入等待模式；当添加了新任务，需要唤醒工作线程，使用条件变量完成了这个过程。
  * 当任务数组为满时，添加新任务的线程进入等待模式，当有新的工作线程消耗了任务，会唤醒添加新任务的线程，完成任务添加。（这可能并不是一个很好的feature）

## 使用说明与样例

本项目提供了一个样例程序`test.c`使用了本线程池库，程序很简单：（读者可以由此学会该线程池使用方法）

```c
#include "thread_pool.h"

void * the_work(void * arg)
{
    usleep(1000);
    printf("n = %5ld, tid = %ld\n",(long)arg, pthread_self());
    usleep(1000);
    return NULL;
}

int main()
{
    thread_pool * p = create_thread_pool(8,100);
    for (long i = 0; i < 10000; i++)
    {
        add_task(p,the_work,(void *)i);
    }
    pthread_exit(NULL);
    return 0;
}
```

编译：

```c
ubuntu@VM-0-9-ubuntu:~/projects/thread_pool$ gcc thread_pool.c test.c -pthread
ubuntu@VM-0-9-ubuntu:~/projects/thread_pool$ ./a.out
```

运行结果节选：

```
n =  9980, tid = 140523279922944
n =  9981, tid = 140523305101056
n =  9982, tid = 140523321886464
n =  9983, tid = 140523330279168
n =  9984, tid = 140523313493760
n =  9985, tid = 140523296708352
n =  9991, tid = 140523330279168
n =  9990, tid = 140523321886464
n =  9989, tid = 140523305101056
n =  9992, tid = 140523313493760
n =  9988, tid = 140523279922944
n =  9987, tid = 140523271530240
n =  9986, tid = 140523288315648
n =  9993, tid = 140523296708352
n =  9998, tid = 140523279922944
n =  9999, tid = 140523271530240
n =  9997, tid = 140523313493760
n =  9996, tid = 140523305101056
n =  9995, tid = 140523321886464
n =  9994, tid = 140523330279168
```


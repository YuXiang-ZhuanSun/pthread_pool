#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//task struct
typedef struct 
{
    void * (*func)(void *); 
    void * arg;
}task;

//thread pool struct
typedef struct
{
    pthread_mutex_t mutex; //分配任务过程锁
    pthread_cond_t thread_cond;   //无任务线程等待条件变量
    pthread_cond_t task_cond;    //无空间任务等待条件变量
    pthread_t * threads;   //线程数组
    task * tasks;          //任务数组

    int thread_limit;      //线程数上限
    int task_limit;        //任务数上限
    int task_running_count;      //正在运行的任务数
    int task_waiting_count;      //正在等待的任务数
    int head_index;        //
    int tail_index;         
}thread_pool;


//thread wrap function
void * work(void *arg);

//create a thread pool
thread_pool * create_thread_pool(int thread_limit,int task_limit);

//add new task
int add_task(thread_pool* p, void *(*func)(), void * arg);

#endif
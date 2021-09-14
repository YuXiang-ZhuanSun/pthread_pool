#include "thread_pool.h"

void * work(void *arg)
{
    thread_pool * p = (thread_pool *)arg;
    while (1)
    {
        pthread_mutex_lock(&p->mutex);
        p->task_running_count--;
        while (p->task_waiting_count == 0)
        {
            pthread_cond_wait(&p->thread_cond,&p->mutex);
        }
        int new_index = p->head_index = (p->head_index + 1) % p->task_limit;
        void *(*work_func)(void *);
        work_func = p->tasks[new_index].func;
        void* work_arg = p->tasks[new_index].arg;
        p->task_running_count++;
        p->task_waiting_count--;
        pthread_cond_broadcast(&p->task_cond);
        pthread_mutex_unlock(&p->mutex);
        work_func(work_arg);
    }
    
}

thread_pool * create_thread_pool(int thread_limit,int task_limit)
{
    thread_pool * p = (thread_pool *)malloc(sizeof(thread_pool));
    
    pthread_mutex_init(&p->mutex,NULL);
    pthread_cond_init(&p->thread_cond,NULL);
    pthread_cond_init(&p->task_cond,NULL);
    pthread_mutex_lock(&p->mutex);

    p->threads =(pthread_t *)malloc(thread_limit*sizeof(pthread_t));
    p->thread_limit = thread_limit;
    p->tasks = (task *)malloc(task_limit*sizeof(task));
    p->task_limit = task_limit;
    p->head_index = p->tail_index = p->task_waiting_count = p->task_running_count = 0;
    for(int i =0;i<thread_limit;i++){
        pthread_create(&p->threads[i],NULL,work,p);
    }
    pthread_mutex_unlock(&p->mutex);
    return p;
}

int add_task(thread_pool* p, void *(*func)(), void * arg)
{
    pthread_mutex_lock(&p->mutex);

    while (p->task_waiting_count == p->task_limit)
    {
        pthread_cond_wait(&p->task_cond,&p->mutex);
    }
    p->task_waiting_count++;
    int new_index = p->tail_index = (p->tail_index + 1) % p->task_limit;
    p->tasks[new_index].func = func;
    p->tasks[new_index].arg = arg;
    pthread_cond_broadcast(&p->thread_cond);
    pthread_mutex_unlock(&p->mutex);
    return 0;
}
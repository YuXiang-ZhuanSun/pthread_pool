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
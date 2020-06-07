/*
Cross platform semaphores. Choice of public domain of MIT-0.

David Reid - mackron@gmail.com

This implements an anonymous semaphore type called pthread_sem_t which I think is something that should be part of the standard pthread.h header. The
motivation for this code is that Apple has decided to remove anonymous semaphores which means the semaphore type in semaphore.h is not usable if you want to
use an anonymous semaphore in a cross-platform manner.

This code is split into a header and an implementation section. The idea is that if you want to use this code you can copy the header section into a .h file
and then copy the implementation section into a .c file.

This is implemented in terms of a condition variable and a mutex.
*/
/* HEADER SECTION */
#include <pthread.h>

typedef struct
{
    int value;
    int valueMax;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} pthread_sem_t;

int pthread_sem_init(pthread_sem_t* pSem, int value, int valueMax);
int pthread_sem_destroy(pthread_sem_t* pSem);
int pthread_sem_wait(pthread_sem_t* pSem);
int pthread_sem_signal(pthread_sem_t* pSem);



/* IMPLEMENTATION SECTION */
#include <errno.h>

int pthread_sem_init(pthread_sem_t* pSem, int value, int valueMax)
{
    int result;

    if (pSem == NULL || valueMax == 0 || value > valueMax) {
        return EINVAL;
    }

    pSem->value    = value;
    pSem->valueMax = valueMax;

    result = pthread_mutex_init(&pSem->lock, NULL);
    if (result != 0) {
        return result;  /* Failed to create mutex. */
    }

    result = pthread_cond_init(&pSem->cond, NULL);
    if (result != 0) {
        pthread_mutex_destroy(&pSem->lock);
        return result;  /* Failed to create condition variable. */
    }

    return 0;
}

int pthread_sem_destroy(pthread_sem_t* pSem)
{
    if (pSem == NULL) {
        return EINVAL;
    }

    pthread_cond_destroy(&pSem->cond);
    pthread_mutex_destroy(&pSem->lock);

    return 0;
}

int pthread_sem_wait(pthread_sem_t* pSem)
{
    if (pSem == NULL) {
        return EINVAL;
    }

    pthread_mutex_lock(&pSem->lock);
    {
        /* We need to wait on a condition variable before escaping. We can't return from this function until the semaphore has been signaled. */
        while (pSem->value == 0) {
            pthread_cond_wait(&pSem->cond, &pSem->lock);
        }

        pSem->value -= 1;
    }
    pthread_mutex_unlock(&pSem->lock);

    return 0;
}

int pthread_sem_signal(pthread_sem_t* pSem)
{
    int result = 0;

    if (pSem == NULL) {
        return EINVAL;
    }

    pthread_mutex_lock(&pSem->lock);
    {
        if (pSem->value < pSem->valueMax) {
            pSem->value += 1;
            pthread_cond_signal(&pSem->cond);
        } else {
            result = ERANGE;
        }
    }
    pthread_mutex_unlock(&pSem->lock);

    return result;
}


/* TESTING SECTION */
#include <stdio.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/time.h>
#endif

pthread_mutex_t g_lock;
pthread_sem_t g_sem;
unsigned int g_counter = 0;
int g_done = 0;

void do_sleep(int milliseconds)
{
#if defined(_WIN32)
    Sleep(milliseconds);
#else
    struct timeval tv;
    tv.tv_sec  = milliseconds / 1000;
    tv.tv_usec = milliseconds % 1000 * 1000;
    select(0, NULL, NULL, NULL, &tv);
#endif
}

void* producer_thread(void* pUserData)
{
    (void)pUserData;

    while (g_done == 0) {
        int result;

        pthread_mutex_lock(&g_lock);
        {
            if (g_counter == 1000) {
                pthread_mutex_unlock(&g_lock);
                continue;
            }

            g_counter += 1; /* Produce data. */
            printf("Produced: %d\n", g_counter);
        }
        pthread_mutex_unlock(&g_lock);

        result = pthread_sem_signal(&g_sem);
        if (result != 0) {
            printf("Failed to signal semaphore. %d\n", g_counter);
        }
        
        //do_sleep(1);
    }

    printf("Producer: DONE\n");
    return (void*)0;
}

void* consumer_thread(void* pUserData)
{
    (void)pUserData;

    for (;;) {
        int result = pthread_sem_wait(&g_sem);
        if (result != 0) {
            break;  /* Some error occurred. */
        }

        if (g_done == 1) {
            break;
        }

        pthread_mutex_lock(&g_lock);
        {
            printf("Consumed: %d\n", g_counter);
            g_counter -= 1; /* Consume data. */
        }
        pthread_mutex_unlock(&g_lock);
    }

    printf("Consumer: DONE\n");
    return (void*)0;
}

int main(int argc, char** argv)
{
    pthread_t consumerThreads[2];
    pthread_t producerThreads[2];

    pthread_mutex_init(&g_lock, NULL);
    pthread_sem_init(&g_sem, 0, 1000);

    pthread_create(&consumerThreads[0], NULL, consumer_thread, NULL);
    //pthread_create(&consumerThreads[1], NULL, consumer_thread, NULL);
    pthread_create(&producerThreads[0], NULL, producer_thread, NULL);
    pthread_create(&producerThreads[1], NULL, producer_thread, NULL);

    printf("Press Enter to quit...\n");
    getchar();

    g_done = 1;

    pthread_sem_signal(&g_sem);
    pthread_join(consumerThreads[0], NULL);
    pthread_join(producerThreads[0], NULL);

    pthread_sem_signal(&g_sem);
    //pthread_join(consumerThreads[1], NULL);
    pthread_join(producerThreads[1], NULL);

    return 0;
}

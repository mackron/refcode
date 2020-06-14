/*
Cross platform yield. Choice of public domain or MIT-0.

David Reid - mackron@gmail.com

This implements a function called ref_yield() which is useful for spin locks. The idea is to use it like this:

    while (myCondition != MA_TRUE) {
        ref_yield();
    }

The yield command basically just a hint to tell the CPU to take control from the current thread and give it to another thread. 
*/

#if defined(__i386) || defined(_M_IX86) || defined(__x86_64__) || defined(_M_X64)
    #if defined(_MSC_VER) && _MSC_VER >= 1400
        #include <intrin.h>
    #endif
#endif

void ref_yield(void)
{
#if defined(__i386) || defined(_M_IX86) || defined(__x86_64__) || defined(_M_X64)
    /* x86/x64 */
    #if defined(_MSC_VER)
        #if _MSC_VER >= 1400
            _mm_pause();
        #else
            __asm pause;
        #endif
    #else
        __asm__ __volatile__ ("pause");
    #endif
#elif (defined(__arm__) && defined(__ARM_ARCH) && __ARM_ARCH >= 6) || (defined(_M_ARM) && _M_ARM >= 6)
    /* ARM */
    #if defined(_MSC_VER)
        /* Apparently there is a __yield() intrinsic that's compatible with ARM, but I cannot find documentation for it nor can I find where it's declared. */
        __yield();
    #else
        __asm__ __volatile__ ("yield");
    #endif
#else
    /* Unknown or unsupported architecture. No-op. */
#endif
}

/* TESTING */
#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif
#include <stdio.h>

int g_IsFinished = 0;

#if defined(_WIN32)
unsigned long WINAPI other_thread(void* pUserData)
#else
void* other_thread(void* pUserData)
#endif
{
    while (g_IsFinished != 1) {
        ref_yield();
    }

    return 0;
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

#if defined(_WIN32)
    HANDLE hThread = CreateThread(NULL, 0, other_thread, NULL, 0, NULL);
#else
    pthread_t thread;
    pthread_create(&thread, NULL, other_thread, NULL);
#endif

    printf("Press Enter to quit...\n");
    getchar();

    g_IsFinished = 1;

#if defined(_WIN32)
    WaitForSingleObject(hThread, INFINITE);
#else
    pthread_join(thread, NULL);
#endif

    return 0;
}

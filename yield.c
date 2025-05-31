/*
Cross platform yield. Choice of public domain or MIT-0.

This implements a function called ns_yield() which is useful for spin locks. The idea is to use it like this:

    while (myCondition != MA_TRUE) {
        ns_yield();
    }

The yield command basically just a hint to tell the CPU to take control from the current thread and give it to another thread. 
*/

#ifndef NS_INLINE
#define NS_INLINE
#endif

/* BEG yield.c */
#if defined(__i386) || defined(_M_IX86) || defined(__x86_64__) || defined(_M_X64)
    #if defined(_MSC_VER) && _MSC_VER >= 1400
        #include <intrin.h>
    #endif
#endif

static NS_INLINE void ns_yield(void)
{
#if defined(__i386) || defined(_M_IX86) || defined(__x86_64__) || defined(_M_X64)
    /* x86/x64 */
    #if defined(_MSC_VER) && !defined(__clang__)
        #if _MSC_VER >= 1400
            _mm_pause();
        #else
            #if defined(__DMC__)
                /* Digital Mars does not recognize the PAUSE opcode. Fall back to NOP. */
                __asm nop;
            #else
                __asm pause;
            #endif
        #endif
    #else
        __asm__ __volatile__ ("rep; nop");  /* Equivalent to "pause". */
    #endif
#elif (defined(__arm__) && defined(__ARM_ARCH) && __ARM_ARCH >= 7) || (defined(_M_ARM) && _M_ARM >= 7) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6T2__)
    /* ARM */
    #if defined(_MSC_VER)
        /* Apparently with MSVC there is a __yield() intrinsic that's compatible with ARM, but I cannot find documentation for it nor can I find where it's declared. */
        __yield();
    #else
        /* The yield instruction is available starting from ARMv7. */
        #if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
            __asm__ __volatile__ ("yield");
        #else
            __asm__ __volatile__ ("nop");
        #endif
    #endif
#else
    /* Unknown or unsupported architecture. No-op. */
#endif
}
/* END yield.c */


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
    (void)pUserData;

    while (g_IsFinished != 1) {
        ns_yield();
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

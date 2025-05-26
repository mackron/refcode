/*
This provides a simple spinlock with support for a decent number of compilers and architectures. If you only care about modern
versions of GCC or Clang, skip this library and just use the __atomic builtins directly. Where this is useful is when you want
to support older compilers and MSVC-like compilers that do not support GCC-style atomic intrinsics.

The intent is to just copy this code into your project either manually or with a tool. It's not intended to be used as a normal
library. I personally use a tool to amalgamate this code into other projects of mine. I'm not interested in turning this into a
normal library so don't bother asking.

As an alternative to this, you can also consider my other library c89atomic which provides a more complete set of atomic
operations, including a spinlock implementation. Indeed, this library is generated from the code in c89atomic.
*/
#ifndef spinlock_h
#define spinlock_h

/* BEG inline.h */
#if defined(_MSC_VER)
    #define SPINLOCK_INLINE __forceinline
#elif defined(__GNUC__)
    /*
    I've had a bug report where GCC is emitting warnings about functions possibly not being inlineable. This warning happens when
    the __attribute__((always_inline)) attribute is defined without an "inline" statement. I think therefore there must be some
    case where "__inline__" is not always defined, thus the compiler emitting these warnings. When using -std=c89 or -ansi on the
    command line, we cannot use the "inline" keyword and instead need to use "__inline__". In an attempt to work around this issue
    I am using "__inline__" only when we're compiling in strict ANSI mode.
    */
    #if defined(__STRICT_ANSI__) || !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
        #define SPINLOCK_INLINE __inline__ __attribute__((always_inline))
    #else
        #define SPINLOCK_INLINE inline __attribute__((always_inline))
    #endif
#elif defined(__WATCOMC__) || defined(__DMC__)
    #define SPINLOCK_INLINE __inline
#else
    #define SPINLOCK_INLINE
#endif
/* END inline.h */

/* BEG arch.h */
#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || defined(__ia64) || defined(_M_IA64) || defined(__aarch64__) || defined(_M_ARM64) || defined(__powerpc64__)
    #ifndef SPINLOCK_64BIT
    #define SPINLOCK_64BIT
    #endif
#else
    #ifndef SPINLOCK_32BIT
    #define SPINLOCK_32BIT
    #endif
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define SPINLOCK_X64
#elif defined(__i386) || defined(_M_IX86) || defined(__i386__)
#define SPINLOCK_X86
#elif defined(__arm64) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
#define SPINLOCK_ARM64
#elif defined(__arm__) || defined(_M_ARM)
#define SPINLOCK_ARM32
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(_ARCH_PPC64)
#define SPINLOCK_PPC64
#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__) || defined(__powerpc) || defined(__ppc) || defined(_ARCH_PPC)
#define SPINLOCK_PPC32
#endif

#if defined(SPINLOCK_ARM32) || defined(SPINLOCK_ARM64)
#define SPINLOCK_ARM
#endif
/* END arch.h */

/* BEG spinlock.h */
#if !defined(SPINLOCK_MODERN_MSVC) && \
    !defined(SPINLOCK_LEGACY_MSVC) && \
    !defined(SPINLOCK_LEGACY_MSVC_ASM) && \
    !defined(SPINLOCK_MODERN_GCC) && \
    !defined(SPINLOCK_LEGACY_GCC) && \
    !defined(SPINLOCK_LEGACY_GCC_ASM)
    #if defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DMC__) || defined(__BORLANDC__)
        #if (defined(_MSC_VER) && _MSC_VER > 1600)
            #define SPINLOCK_MODERN_MSVC
        #else
            #if defined(SPINLOCK_X64)
                #define SPINLOCK_LEGACY_MSVC
            #else
                #define SPINLOCK_LEGACY_MSVC_ASM
            #endif
        #endif
    #elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))) || defined(__clang__)
        #define SPINLOCK_MODERN_GCC
    #else
        #if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
            #define SPINLOCK_LEGACY_GCC
        #else
            #define SPINLOCK_LEGACY_GCC_ASM
        #endif
    #endif
#endif

#if defined(SPINLOCK_MODERN_MSVC) || defined(SPINLOCK_LEGACY_MSVC)
    #include <intrin.h>

    #define spinlock_memory_order_relaxed  1
    #define spinlock_memory_order_consume  2
    #define spinlock_memory_order_acquire  3
    #define spinlock_memory_order_release  4
    #define spinlock_memory_order_acq_rel  5
    #define spinlock_memory_order_seq_cst  6

    #define SPINLOCK_MSVC_ARM_INTRINSIC(dst, src, order, intrin, c89atomicType, msvcType)   \
        c89atomicType result; \
        switch (order) \
        { \
            case spinlock_memory_order_relaxed: \
            { \
                result = (c89atomicType)intrin##_nf((volatile msvcType*)dst, (msvcType)src); \
            } break; \
            case spinlock_memory_order_consume: \
            case spinlock_memory_order_acquire: \
            { \
                result = (c89atomicType)intrin##_acq((volatile msvcType*)dst, (msvcType)src); \
            } break; \
            case spinlock_memory_order_release: \
            { \
                result = (c89atomicType)intrin##_rel((volatile msvcType*)dst, (msvcType)src); \
            } break; \
            case spinlock_memory_order_acq_rel: \
            case spinlock_memory_order_seq_cst: \
            default: \
            { \
                result = (c89atomicType)intrin((volatile msvcType*)dst, (msvcType)src); \
            } break; \
        } \
        return result;

    typedef unsigned int spinlock_t;

    static SPINLOCK_INLINE spinlock_t spinlock_test_and_set_explicit(volatile spinlock_t* dst, spinlock_memory_order order)
    {
        #if defined(SPINLOCK_ARM)
        {
            SPINLOCK_MSVC_ARM_INTRINSIC(dst, 1, order, _InterlockedExchange, spinlock_t, long);
        }
        #else
        {
            (void)order;    
            return (spinlock_t)_InterlockedExchange((volatile long*)dst, (long)1);
        }
        #endif
    }

    static SPINLOCK_INLINE void spinlock_clear_explicit(volatile spinlock_t* dst, spinlock_memory_order order)
    {
        #if defined(SPINLOCK_ARM)
        {
            SPINLOCK_MSVC_ARM_INTRINSIC(dst, 0, order, _InterlockedExchange, spinlock_t, long);
        }
        #else
        {
            (void)order;    
            _InterlockedExchange((volatile long*)dst, (long)0);
        }
        #endif
    }

    static SPINLOCK_INLINE spinlock_t spinlock_load_explicit(volatile const spinlock_t* dst, spinlock_memory_order order)
    {
        (void)order;
        return (unsigned int)_InterlockedCompareExchange((volatile long*)dst, 0, 0);
    }
#endif

#if defined(SPINLOCK_LEGACY_MSVC_ASM)
    #define spinlock_memory_order_relaxed  1
    #define spinlock_memory_order_consume  2
    #define spinlock_memory_order_acquire  3
    #define spinlock_memory_order_release  4
    #define spinlock_memory_order_acq_rel  5
    #define spinlock_memory_order_seq_cst  6

    typedef unsigned int spinlock_t;

    static SPINLOCK_INLINE spinlock_t spinlock_test_and_set_explicit(volatile spinlock_t* dst, spinlock_memory_order order)
    {
        spinlock_t result = 0;

        (void)order;
        __asm {
            mov ecx, dst
            mov eax, 1
            xchg [ecx], eax
            mov result, eax
        }

        return result;
    }

    static SPINLOCK_INLINE void spinlock_clear_explicit(volatile spinlock_t* dst, spinlock_memory_order order)
    {
        if (order == spinlock_memory_order_relaxed) {
            __asm {
                mov esi, dst
                mov dword ptr [esi], 0
            }
        } else {
            __asm {
                mov esi, dst
                mov eax, 0
                xchg [esi], eax
            }
        }
    }

    static SPINLOCK_INLINE spinlock_t spinlock_load_explicit(volatile const spinlock_t* dst, spinlock_memory_order order)
    {
        spinlock_t result = 0;

        if (order == spinlock_memory_order_relaxed) {
            __asm {
                mov esi, dst
                mov eax, [esi]
                mov result, eax
            }
        } else if (order <= spinlock_memory_order_release) {
            __asm {
                mov esi, dst
                mov eax, [esi]
                lock add dword ptr [esp], 0 
                mov result, eax
            }
        } else {
            __asm {
                lock add dword ptr [esp], 0 
                mov esi, dst
                mov eax, [esi]
                mov result, eax
                lock add dword ptr [esp], 0 
            }
        }

        return result;
    }
#endif

#if defined(SPINLOCK_MODERN_GCC)
    #define spinlock_memory_order_relaxed                   __ATOMIC_RELAXED
    #define spinlock_memory_order_consume                   __ATOMIC_CONSUME
    #define spinlock_memory_order_acquire                   __ATOMIC_ACQUIRE
    #define spinlock_memory_order_release                   __ATOMIC_RELEASE
    #define spinlock_memory_order_acq_rel                   __ATOMIC_ACQ_REL
    #define spinlock_memory_order_seq_cst                   __ATOMIC_SEQ_CST

    typedef unsigned int spinlock_t;

    #define spinlock_test_and_set_explicit(dst, order) __atomic_exchange_n(dst, 1, order)
    #define spinlock_clear_explicit(dst, order)        __atomic_store_n(dst, 0, order)
    #define spinlock_load_explicit(dst, order)         __atomic_load_n(dst, order)
#endif

#if defined(SPINLOCK_LEGACY_GCC)
    #define spinlock_memory_order_relaxed  1
    #define spinlock_memory_order_consume  2
    #define spinlock_memory_order_acquire  3
    #define spinlock_memory_order_release  4
    #define spinlock_memory_order_acq_rel  5
    #define spinlock_memory_order_seq_cst  6

    typedef unsigned int spinlock_t;

    static SPINLOCK_INLINE spinlock_t spinlock_test_and_set_explicit(volatile spinlock_t* dst, spinlock_memory_order order)
    {
        if (order > spinlock_memory_order_acquire) {
            __sync_synchronize();
        }

        return __sync_lock_test_and_set(dst, 1);
    }

    static SPINLOCK_INLINE void spinlock_clear_explicit(volatile spinlock_t* dst, spinlock_memory_order order)
    {
        if (order > spinlock_memory_order_release) {
            __sync_synchronize();
        }

        __sync_lock_release(dst);
    }

    static SPINLOCK_INLINE spinlock_t spinlock_load_explicit(volatile const spinlock_t* dst, spinlock_memory_order order)
    {
        (void)order;
        return __sync_val_compare_and_swap((spinlock_t*)dst, 0, 0);
    }
#endif

#if defined(SPINLOCK_LEGACY_GCC_ASM)
    #define spinlock_memory_order_relaxed  1
    #define spinlock_memory_order_consume  2
    #define spinlock_memory_order_acquire  3
    #define spinlock_memory_order_release  4
    #define spinlock_memory_order_acq_rel  5
    #define spinlock_memory_order_seq_cst  6

    
    #if defined(SPINLOCK_X86)
        #define spinlock_thread_fence(order) __asm__ __volatile__("lock; addl , (%%esp)" ::: "memory")
    #elif defined(SPINLOCK_X64)
        #define spinlock_thread_fence(order) __asm__ __volatile__("lock; addq , (%%rsp)" ::: "memory")
    #else
        #error Unsupported architecture.
    #endif

    #define SPINLOCK_XCHG_GCC_X86(instructionSizeSuffix, result, dst, src) \
        __asm__ __volatile__(                    \
            "xchg"instructionSizeSuffix" %0, %1" \
            : "=r"(result),              \
              "=m"(*dst)                 \
            : "0"(src),                  \
              "m"(*dst)                  \
            : "memory"                           \
        )


    #define SPINLOCK_LOAD_RELAXED_GCC_X86(instructionSizeSuffix, result, dst) \
        __asm__ __volatile__(                   \
            "mov"instructionSizeSuffix" %1, %0" \
            : "=r"(result)              \
            : "m"(*dst)                 \
        )

    #define SPINLOCK_LOAD_RELEASE_GCC_X86(instructionSizeSuffix, result, dst) \
        spinlock_thread_fence(spinlock_memory_order_release); \
        __asm__ __volatile__(                   \
            "mov"instructionSizeSuffix" %1, %0" \
            : "=r"(result)              \
            : "m"(*dst)                 \
            : "memory"                          \
        )

    #define SPINLOCK_LOAD_SEQ_CST_GCC_X86(instructionSizeSuffix, result, dst) \
        spinlock_thread_fence(spinlock_memory_order_seq_cst); \
        __asm__ __volatile__(                   \
            "mov"instructionSizeSuffix" %1, %0" \
            : "=r"(result)              \
            : "m"(*dst)                 \
            : "memory"                          \
        );                                      \
        spinlock_thread_fence(spinlock_memory_order_seq_cst)


    typedef unsigned int spinlock_t;

    static SPINLOCK_INLINE spinlock_t spinlock_test_and_set_explicit(volatile spinlock_t* dst, spinlock_memory_order order)
    {
        spinlock_t result;
        #if defined(SPINLOCK_X86) || defined(SPINLOCK_X64)
        {
            (void)order;
            SPINLOCK_XCHG_GCC_X86("l", result, dst, 1);
        }
        #else
        {
            #error Unsupported architecture.
        }
        #endif
        return result;
    }

    static SPINLOCK_INLINE void spinlock_clear_explicit(volatile spinlock_t* dst, spinlock_memory_order order)
    {
        #if defined(SPINLOCK_X86) || defined(SPINLOCK_X64)
        {
            
            if (order == spinlock_memory_order_relaxed) {
                __asm__ __volatile__(
                    "movl , %0"
                    : "=m"(*dst)    
                );
            } else if (order == spinlock_memory_order_release) {
                __asm__ __volatile__(
                    "movl , %0"
                    : "=m"(*dst)    
                    :
                    : "memory"
                );
            } else {
                spinlock_t tmp = 0;
                __asm__ __volatile__(
                    "xchgl %0, %1"
                    : "=r"(tmp),    
                      "=m"(*dst)    
                    : "0"(tmp),     
                      "m"(*dst)     
                    : "memory"
                );
            }
        }
        #else
        {
            #error Unsupported architecture.
        }
        #endif
    }

    static SPINLOCK_INLINE spinlock_t spinlock_load_explicit(volatile const spinlock_t* dst, spinlock_memory_order order)
    {
        #if defined(SPINLOCK_X86) || defined(SPINLOCK_X64)
        {
            spinlock_t result;

            if (order == spinlock_memory_order_relaxed) {
                SPINLOCK_LOAD_RELAXED_GCC_X86("l", result, dst);
            } else if (order <= spinlock_memory_order_release) {
                SPINLOCK_LOAD_RELEASE_GCC_X86("l", result, dst);
            } else {
                SPINLOCK_LOAD_SEQ_CST_GCC_X86("l", result, dst);
            }

            return result;
        }
        #else
        {
            #error Unsupported architecture.
        }
        #endif
    }
#endif


static SPINLOCK_INLINE void spinlock_lock(volatile spinlock_t* pSpinlock)
{
    for (;;) {
        if (spinlock_test_and_set_explicit(pSpinlock, spinlock_memory_order_acquire) == 0) {
            break;
        }

        while (spinlock_load_explicit(pSpinlock, spinlock_memory_order_relaxed) == 1) {
            
        }
    }
}

static SPINLOCK_INLINE void spinlock_unlock(volatile spinlock_t* pSpinlock)
{
    spinlock_clear_explicit(pSpinlock, spinlock_memory_order_release);
}
/* END spinlock.h */

#endif /* spinlock_h */


/* TODO: Write a proper test program here. */
int main(int argc, char** argv)
{
    spinlock_t lock = 0;

    spinlock_lock(&lock);
    {
        // TODO: Do something.
    }
    spinlock_unlock(&lock);

    (void)argc;
    (void)argv;

    return 0;
}

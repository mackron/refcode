#ifndef spinlock_h
#define spinlock_h

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

#if defined(__x86_64__) || defined(_M_X64)
#define SPINLOCK_X64
#elif defined(__i386) || defined(_M_IX86)
#define SPINLOCK_X86
#elif defined(__arm__) || defined(_M_ARM) || defined(__arm64) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
#define SPINLOCK_ARM
#endif

/* BEG spinlock.h */
typedef unsigned int spinlock_t;
typedef int spinlock_memory_order;

#if (defined(_MSC_VER) /*&& !defined(__clang__)*/) || defined(__WATCOMC__) || defined(__DMC__)
    #if _MSC_VER < 1600 && defined(SPINLOCK_X86)   /* 1600 = Visual Studio 2010 */
        #define SPINLOCK_MSVC_USE_INLINED_ASSEMBLY
    #else
        #include <intrin.h>
    #endif

    #define spinlock_memory_order_relaxed  0
    #define spinlock_memory_order_consume  1
    #define spinlock_memory_order_acquire  2
    #define spinlock_memory_order_release  3
    #define spinlock_memory_order_acq_rel  4
    #define spinlock_memory_order_seq_cst  5

    #define SPINLOCK_MSVC_ARM_INTRINSIC(dst, src, order, intrin, spinlockType, msvcType)   \
    spinlockType result; \
        switch (order) \
        { \
            case spinlock_memory_order_relaxed: \
            { \
                result = (spinlockType)intrin##_nf((volatile msvcType*)dst, (msvcType)src); \
            } break; \
            case spinlock_memory_order_consume: \
            case spinlock_memory_order_acquire: \
            { \
                result = (spinlockType)intrin##_acq((volatile msvcType*)dst, (msvcType)src); \
            } break; \
            case spinlock_memory_order_release: \
            { \
                result = (spinlockType)intrin##_rel((volatile msvcType*)dst, (msvcType)src); \
            } break; \
            case spinlock_memory_order_acq_rel: \
            case spinlock_memory_order_seq_cst: \
            default: \
            { \
                result = (spinlockType)intrin((volatile msvcType*)dst, (msvcType)src); \
            } break; \
        } \
        return result;

    static SPINLOCK_INLINE spinlock_t spinlock_atomic_exchange_explicit_32(spinlock_t* lock, unsigned int value, spinlock_memory_order order)
    {
        #if defined(SPINLOCK_MSVC_USE_INLINED_ASSEMBLY)
        {
            spinlock_t result = 0;
                
            (void)order;
            __asm {
                mov ecx, dst
                mov eax, src
                lock xchg [ecx], eax
                mov result, eax
            }

            return result;
        }
        #else
        {
            #if defined(SPINLOCK_ARM)
                SPINLOCK_MSVC_ARM_INTRINSIC(dst, src, order, _InterlockedExchange, spinlock_t, long);
            #else
                (void)order;    /* Always using the strongest memory order. */
                return (spinlock_t)_InterlockedExchange((volatile long*)lock, (long)value);
            #endif
        }
        #endif
    }

    static SPINLOCK_INLINE spinlock_t __stdcall spinlock_atomic_compare_and_swap_32(volatile spinlock_t* dst, spinlock_t expected, spinlock_t desired)
    {
        #if defined(SPINLOCK_MSVC_USE_INLINED_ASSEMBLY)
        {
            spinlock_t result = 0;
            
            __asm {
                mov ecx, dst
                mov eax, expected
                mov edx, desired
                lock cmpxchg [ecx], edx /* Writes to EAX which MSVC will treat as the return value. */
                mov result, eax
            }

            return result;
        }
        #else
        {
            return (spinlock_t)_InterlockedCompareExchange((volatile long*)dst, (long)desired, (long)expected);
        }
        #endif
    }

    static SPINLOCK_INLINE spinlock_t spinlock_atomic_load_explicit_32(volatile const spinlock_t* ptr, spinlock_memory_order order)
    {
    #if defined(SPINLOCK_ARM)
        SPINLOCK_MSVC_ARM_INTRINSIC_COMPARE_EXCHANGE(ptr, 0, 0, order, _InterlockedCompareExchange, spinlock_t, long);
    #else
        (void)order;    /* Always using the strongest memory order. */
        return spinlock_atomic_compare_and_swap_32((volatile spinlock_t*)ptr, 0, 0);
    #endif
    }
#elif defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)))
    /* Modern GCC atomic built-ins. */
    #define spinlock_memory_order_relaxed   __ATOMIC_RELAXED
    #define spinlock_memory_order_consume   __ATOMIC_CONSUME
    #define spinlock_memory_order_acquire   __ATOMIC_ACQUIRE
    #define spinlock_memory_order_release   __ATOMIC_RELEASE
    #define spinlock_memory_order_acq_rel   __ATOMIC_ACQ_REL
    #define spinlock_memory_order_seq_cst   __ATOMIC_SEQ_CST

    #define spinlock_atomic_exchange_explicit_32(dst, src, order)  __atomic_exchange_n(dst, src, order)
    #define spinlock_atomic_load_explicit_32(dst, order)           __atomic_load_n(dst, order)
#else
    /* GCC and compilers supporting GCC-style inlined assembly. */
    #define spinlock_memory_order_relaxed  1
    #define spinlock_memory_order_consume  2
    #define spinlock_memory_order_acquire  3
    #define spinlock_memory_order_release  4
    #define spinlock_memory_order_acq_rel  5
    #define spinlock_memory_order_seq_cst  6

    #if defined(__GNUC__)
        /* Legacy GCC atomic built-ins. Everything is a full memory barrier. */
        static SPINLOCK_INLINE spinlock_t spinlock_atomic_exchange_explicit_32(volatile spinlock_t* dst, spinlock_t src, spinlock_memory_order order)
        {
            spinlock_t oldValue;
        
            do {
                oldValue = *dst;
            } while (__sync_val_compare_and_swap(dst, oldValue, src) != oldValue);
        
            (void)order;
            return oldValue;
        }

        #define spinlock_atomic_compare_and_swap_32(dst, expected, desired)   __sync_val_compare_and_swap(dst, expected, desired)
    #else
        /* Non-GCC compilers supporting GCC-style inlined assembly. The inlined assembly below uses Gas syntax. */
        static SPINLOCK_INLINE spinlock_t spinlock_atomic_exchange_explicit_32(volatile spinlock_t* dst, spinlock_t src, spinlock_memory_order order)
        {
            spinlock_t result;
                
            (void)order;

        #if defined(SPINLOCK_X86) || defined(SPINLOCK_X64)
            __asm__ __volatile__("lock; xchg %1, %0" : "+m"(*dst), "=a"(result) : "a"(src));
        #else
            #error Unsupported architecture. Please submit a feature request.
        #endif

            return result;
        }

        static SPINLOCK_INLINE spinlock_t spinlock_atomic_compare_and_swap_32(volatile spinlock_t* dst, spinlock_t expected, spinlock_t desired)
        {
            spinlock_t result;

        #if defined(SPINLOCK_X86) || defined(SPINLOCK_X64)
            __asm__ __volatile__("lock; cmpxchg %3, %0" : "+m"(*dst), "=a"(result) : "a"(expected), "d"(desired) : "cc");
        #else
            #error Unsupported architecture. Please submit a feature request.
        #endif

            return result;
        }
    #endif

    static SPINLOCK_INLINE spinlock_t spinlock_atomic_load_explicit_32(volatile const spinlock_t* ptr, spinlock_memory_order order)
    {
        (void)order;    /* Always using the strongest memory order. */
        return spinlock_atomic_compare_and_swap_32((spinlock_t*)ptr, 0, 0);
    }
#endif



static SPINLOCK_INLINE void spinlock_lock(spinlock_t* lock)
{
    for (;;) {
        if (spinlock_atomic_exchange_explicit_32(lock, 1, spinlock_memory_order_acquire) == 0) {
            break;
        }

        while (spinlock_atomic_load_explicit_32(lock, spinlock_memory_order_relaxed) == 1) {
            /* Do nothing. */
        }
    }
}

static SPINLOCK_INLINE void spinlock_unlock(spinlock_t* lock)
{
    spinlock_atomic_exchange_explicit_32(lock, 0, spinlock_memory_order_release);
}
/* END spinlock.h*/

#endif /* spinlock_h */

/* TODO: Write a test program here. */

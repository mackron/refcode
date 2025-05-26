#include <stddef.h> /* For size_t. */

#if defined(SIZE_MAX)
    #define NS_SIZE_MAX     SIZE_MAX
#else
    #define NS_SIZE_MAX     0xFFFFFFFF  /* When SIZE_MAX is not defined by the standard library just default to the maximum 32-bit unsigned integer. */
#endif

#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || defined(__ia64) || defined(_M_IA64) || defined(__aarch64__) || defined(_M_ARM64) || defined(__powerpc64__)
    #define NS_SIZEOF_PTR   8
#else
    #define NS_SIZEOF_PTR   4
#endif

#if defined(NS_USE_STDINT)
    #include <stdint.h>
    typedef int8_t                  ns_int8;
    typedef uint8_t                 ns_uint8;
    typedef int16_t                 ns_int16;
    typedef uint16_t                ns_uint16;
    typedef int32_t                 ns_int32;
    typedef uint32_t                ns_uint32;
    typedef int64_t                 ns_int64;
    typedef uint64_t                ns_uint64;
#else
    typedef   signed char           ns_int8;
    typedef unsigned char           ns_uint8;
    typedef   signed short          ns_int16;
    typedef unsigned short          ns_uint16;
    typedef   signed int            ns_int32;
    typedef unsigned int            ns_uint32;
    #if defined(_MSC_VER) && !defined(__clang__)
        typedef   signed __int64    ns_int64;
        typedef unsigned __int64    ns_uint64;
    #else
        #if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wlong-long"
            #if defined(__clang__)
                #pragma GCC diagnostic ignored "-Wc++11-long-long"
            #endif
        #endif
        typedef   signed long long  ns_int64;
        typedef unsigned long long  ns_uint64;
        #if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))
            #pragma GCC diagnostic pop
        #endif
    #endif
#endif  /* NS_USE_STDINT */

#if NS_SIZEOF_PTR == 8
    typedef ns_uint64 ns_uintptr;
    typedef ns_int64  ns_intptr;
#else
    typedef ns_uint32 ns_uintptr;
    typedef ns_int32  ns_intptr;
#endif

typedef unsigned char ns_bool8;
typedef unsigned int  ns_bool32;
#define NS_TRUE  1
#define NS_FALSE 0

#define NS_INT8_MIN   ((ns_int8 )0x80)
#define NS_UINT8_MIN  ((ns_uint8)0x00)
#define NS_INT16_MIN  ((ns_int16)0x8000)
#define NS_UINT16_MIN ((ns_uint16)0x0000)
#define NS_INT32_MIN  ((ns_int32 )0x80000000)
#define NS_UINT32_MIN ((ns_uint32)0x00000000)
#define NS_INT64_MIN  ((ns_int64 )(((ns_uint64)0x80000000 << 32) | 0x00000000))
#define NS_UINT64_MIN ((ns_uint64)(((ns_uint64)0x00000000 << 32) | 0x00000000))

#define NS_INT8_MAX   ((ns_int8 )0x7F)
#define NS_UINT8_MAX  ((ns_uint8)0xFF)
#define NS_INT16_MAX  ((ns_int16)0x7FFF)
#define NS_UINT16_MAX ((ns_uint16)0xFFFF)
#define NS_INT32_MAX  ((ns_int32 )0x7FFFFFFF)
#define NS_UINT32_MAX ((ns_uint32)0xFFFFFFFF)
#define NS_INT64_MAX  ((ns_int64 )(((ns_uint64)0x7FFFFFFF << 32) | 0xFFFFFFFF))
#define NS_UINT64_MAX ((ns_uint64)(((ns_uint64)0xFFFFFFFF << 32) | 0xFFFFFFFF))
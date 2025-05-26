#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || defined(__ia64) || defined(_M_IA64) || defined(__aarch64__) || defined(_M_ARM64) || defined(__powerpc64__)
    #ifndef NS_64BIT
    #define NS_64BIT
    #endif
#else
    #ifndef NS_32BIT
    #define NS_32BIT
    #endif
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define NS_X64
#elif defined(__i386) || defined(_M_IX86) || defined(__i386__)
#define NS_X86
#elif defined(__arm64) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
#define NS_ARM64
#elif defined(__arm__) || defined(_M_ARM)
#define NS_ARM32
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(_ARCH_PPC64)
#define NS_PPC64
#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__) || defined(__powerpc) || defined(__ppc) || defined(_ARCH_PPC)
#define NS_PPC32
#endif

#if defined(NS_ARM32) || defined(NS_ARM64)
#define NS_ARM
#endif
/*
These are allocation callbacks I use in my projects. The idea is to amalgamate these into code
either manually or with a tool.

This has a few macro dependencies. Just look at the code. If you don't like these, just do a
find-and-replace in your amalgamation script.
*/
#include <stddef.h> /* For size_t. */
#include <string.h> /* For memset. */

#ifndef NS_API
#define NS_API
#endif

#ifndef NS_UNUSED
#define NS_UNUSED(x) (void)(x)
#endif

#ifndef NS_ZERO_MEMORY
#define NS_ZERO_MEMORY(p, sz) memset((p), 0, (sz))
#endif

#ifndef NS_MOVE_MEMORY
#define NS_MOVE_MEMORY(dst, src, sz) memmove((dst), (src), (sz))
#endif

typedef size_t ns_uintptr;  /* Technically incorrect, but good enough for my purposes for this file. Projects I amalgamate this code into will define this properly. */



/* BEG allocation_callbacks.h */
typedef struct ns_allocation_callbacks
{
    void* pUserData;
    void* (* onMalloc )(size_t sz, void* pUserData);
    void* (* onRealloc)(void* p, size_t sz, void* pUserData);
    void  (* onFree   )(void* p, void* pUserData);
} ns_allocation_callbacks;

NS_API void* ns_malloc(size_t sz, const ns_allocation_callbacks* pAllocationCallbacks);
NS_API void* ns_calloc(size_t sz, const ns_allocation_callbacks* pAllocationCallbacks);
NS_API void* ns_realloc(void* p, size_t sz, const ns_allocation_callbacks* pAllocationCallbacks);
NS_API void  ns_free(void* p, const ns_allocation_callbacks* pAllocationCallbacks);
NS_API void* ns_aligned_malloc(size_t sz, size_t alignment, const ns_allocation_callbacks* pAllocationCallbacks);
NS_API void* ns_aligned_realloc(void* p, size_t sz, size_t alignment, const ns_allocation_callbacks* pAllocationCallbacks);
NS_API void  ns_aligned_free(void* p, const ns_allocation_callbacks* pAllocationCallbacks);
/* END allocation_callbacks.h */



/* BEG allocation_callbacks.c */
#if !defined(NS_MALLOC) || !defined(NS_REALLOC) || !defined(NS_FREE)
#include <stdlib.h> /* For malloc, realloc, free. */
#endif

#ifndef NS_MALLOC
#define NS_MALLOC(sz) malloc(sz)
#endif
#ifndef NS_REALLOC
#define NS_REALLOC(p, sz) realloc(p, sz)
#endif
#ifndef NS_FREE
#define NS_FREE(p) free(p)
#endif

static void* ns_malloc_default(size_t sz, void* pUserData)
{
    NS_UNUSED(pUserData);
    return NS_MALLOC(sz);
}

static void* ns_realloc_default(void* p, size_t sz, void* pUserData)
{
    NS_UNUSED(pUserData);
    return NS_REALLOC(p, sz);
}

static void ns_free_default(void* p, void* pUserData)
{
    NS_UNUSED(pUserData);
    NS_FREE(p);
}


NS_API ns_allocation_callbacks ns_allocation_callbacks_init_default(void)
{
    ns_allocation_callbacks allocationCallbacks;

    allocationCallbacks.pUserData = NULL;
    allocationCallbacks.onMalloc  = ns_malloc_default;
    allocationCallbacks.onRealloc = ns_realloc_default;
    allocationCallbacks.onFree    = ns_free_default;

    return allocationCallbacks;
}

NS_API ns_allocation_callbacks ns_allocation_callbacks_init_copy(const ns_allocation_callbacks* pAllocationCallbacks)
{
    if (pAllocationCallbacks != NULL) {
        return *pAllocationCallbacks;
    } else {
        return ns_allocation_callbacks_init_default();
    }
}


NS_API void* ns_malloc(size_t sz, const ns_allocation_callbacks* pAllocationCallbacks)
{
    if (pAllocationCallbacks != NULL) {
        if (pAllocationCallbacks->onMalloc != NULL) {
            return pAllocationCallbacks->onMalloc(sz, pAllocationCallbacks->pUserData);
        } else {
            return NULL;    /* Do not fall back to the default implementation. */
        }
    } else {
        return ns_malloc_default(sz, NULL);
    }
}

NS_API void* ns_calloc(size_t sz, const ns_allocation_callbacks* pAllocationCallbacks)
{
    void* p = ns_malloc(sz, pAllocationCallbacks);
    if (p != NULL) {
        NS_ZERO_MEMORY(p, sz);
    }

    return p;
}

NS_API void* ns_realloc(void* p, size_t sz, const ns_allocation_callbacks* pAllocationCallbacks)
{
    if (pAllocationCallbacks != NULL) {
        if (pAllocationCallbacks->onRealloc != NULL) {
            return pAllocationCallbacks->onRealloc(p, sz, pAllocationCallbacks->pUserData);
        } else {
            return NULL;    /* Do not fall back to the default implementation. */
        }
    } else {
        return ns_realloc_default(p, sz, NULL);
    }
}

NS_API void ns_free(void* p, const ns_allocation_callbacks* pAllocationCallbacks)
{
    if (p == NULL) {
        return;
    }

    if (pAllocationCallbacks != NULL) {
        if (pAllocationCallbacks->onFree != NULL) {
            pAllocationCallbacks->onFree(p, pAllocationCallbacks->pUserData);
        } else {
            return; /* Do no fall back to the default implementation. */
        }
    } else {
        ns_free_default(p, NULL);
    }
}

NS_API void* ns_aligned_malloc(size_t sz, size_t alignment, const ns_allocation_callbacks* pAllocationCallbacks)
{
    size_t extraBytes;
    void* pUnaligned;
    void* pAligned;

    if (alignment == 0) {
        return 0;
    }

    extraBytes = alignment-1 + sizeof(void*);

    pUnaligned = ns_malloc(sz + extraBytes, pAllocationCallbacks);
    if (pUnaligned == NULL) {
        return NULL;
    }

    pAligned = (void*)(((ns_uintptr)pUnaligned + extraBytes) & ~((ns_uintptr)(alignment-1)));
    ((void**)pAligned)[-1] = pUnaligned;

    return pAligned;
}

NS_API void* ns_aligned_realloc(void* p, size_t sz, size_t alignment, const ns_allocation_callbacks* pAllocationCallbacks)
{
    size_t extraBytes;
    void* pOldUnaligned;
    void* pNewUnaligned;
    void* pOldAligned;
    void* pNewAligned;

    if (alignment == 0) {
        return 0;
    }

    if (p == NULL) {
        return ns_aligned_malloc(sz, alignment, pAllocationCallbacks);
    }

    pOldAligned   = p;
    pOldUnaligned = ((void**)p)[-1];

    extraBytes = alignment-1 + sizeof(void*);

    pNewUnaligned = ns_realloc(pOldUnaligned, sz + extraBytes, pAllocationCallbacks);
    if (pNewUnaligned == NULL) {
        return NULL;
    }

    pNewAligned = (void*)(((ns_uintptr)pNewUnaligned + extraBytes) & ~((ns_uintptr)(alignment-1)));
    ((void**)pNewAligned)[-1] = pNewUnaligned;

    if (pNewUnaligned != pOldUnaligned) {
        void* pDst = pNewAligned;
        void* pSrc = (unsigned char*)pNewUnaligned + sizeof(void*) + ((ns_uintptr)pOldAligned - ((ns_uintptr)pOldUnaligned + sizeof(void*)));
        NS_MOVE_MEMORY(pDst, pSrc, sz);
    }

    return pNewAligned;
}

NS_API void ns_aligned_free(void* p, const ns_allocation_callbacks* pAllocationCallbacks)
{
    ns_free(((void**)p)[-1], pAllocationCallbacks);
}
/* END allocation_callbacks.c */



/* BEG Tests */
#include <stdio.h>
#include <assert.h>

/* Custom allocator for testing */
typedef struct
{
    size_t mallocCount;
    size_t reallocCount;
    size_t freeCount;
    size_t totalAllocated;
} test_allocator_state;

static void* test_malloc(size_t sz, void* pUserData)
{
    test_allocator_state* pState = (test_allocator_state*)pUserData;
    void* p;
    
    pState->mallocCount++;
    pState->totalAllocated += sz;
    
    p = NS_MALLOC(sz);
    return p;
}

static void* test_realloc(void* p, size_t sz, void* pUserData)
{
    test_allocator_state* pState = (test_allocator_state*)pUserData;
    void* pNew;
    
    pState->reallocCount++;
    
    pNew = NS_REALLOC(p, sz);
    return pNew;
}

static void test_free(void* p, void* pUserData)
{
    test_allocator_state* pState = (test_allocator_state*)pUserData;
    
    pState->freeCount++;
    
    NS_FREE(p);
}

static int test_basic_malloc_free(void)
{
    void* p;
    
    printf("Testing basic malloc/free...\n");
    
    p = ns_malloc(100, NULL);
    if (p == NULL) {
        printf("  FAILED: malloc returned NULL\n");
        return 0;
    }
    
    ns_free(p, NULL);
    
    printf("  PASSED\n");
    return 1;
}

static int test_basic_calloc(void)
{
    size_t i;
    unsigned char* p;
    
    printf("Testing basic calloc...\n");
    
    p = (unsigned char*)ns_calloc(100, NULL);
    if (p == NULL) {
        printf("  FAILED: calloc returned NULL\n");
        return 0;
    }
    
    /* Verify memory is zeroed */
    for (i = 0; i < 100; i++) {
        if (p[i] != 0) {
            printf("  FAILED: memory not zeroed at index %lu\n", (unsigned long)i);
            ns_free(p, NULL);
            return 0;
        }
    }
    
    ns_free(p, NULL);
    
    printf("  PASSED\n");
    return 1;
}

static int test_basic_realloc(void)
{
    void* p;
    void* p2;
    
    printf("Testing basic realloc...\n");
    
    p = ns_malloc(100, NULL);
    if (p == NULL) {
        printf("  FAILED: malloc returned NULL\n");
        return 0;
    }
    
    p2 = ns_realloc(p, 200, NULL);
    if (p2 == NULL) {
        printf("  FAILED: realloc returned NULL\n");
        ns_free(p, NULL);
        return 0;
    }
    
    ns_free(p2, NULL);
    
    printf("  PASSED\n");
    return 1;
}

static int test_custom_allocator(void)
{
    test_allocator_state state;
    ns_allocation_callbacks callbacks;
    void* p;
    
    printf("Testing custom allocator...\n");
    
    NS_ZERO_MEMORY(&state, sizeof(state));
    
    callbacks.pUserData = &state;
    callbacks.onMalloc  = test_malloc;
    callbacks.onRealloc = test_realloc;
    callbacks.onFree    = test_free;
    
    p = ns_malloc(100, &callbacks);
    if (p == NULL) {
        printf("  FAILED: custom malloc returned NULL\n");
        return 0;
    }
    
    if (state.mallocCount != 1) {
        printf("  FAILED: mallocCount = %lu, expected 1\n", (unsigned long)state.mallocCount);
        ns_free(p, &callbacks);
        return 0;
    }
    
    p = ns_realloc(p, 200, &callbacks);
    if (p == NULL) {
        printf("  FAILED: custom realloc returned NULL\n");
        return 0;
    }
    
    if (state.reallocCount != 1) {
        printf("  FAILED: reallocCount = %lu, expected 1\n", (unsigned long)state.reallocCount);
        ns_free(p, &callbacks);
        return 0;
    }
    
    ns_free(p, &callbacks);
    
    if (state.freeCount != 1) {
        printf("  FAILED: freeCount = %lu, expected 1\n", (unsigned long)state.freeCount);
        return 0;
    }
    
    printf("  PASSED\n");
    return 1;
}

static int test_aligned_malloc_free(void)
{
    void* p;
    size_t alignment;
    
    printf("Testing aligned malloc/free...\n");
    
    /* Test various alignments */
    for (alignment = 1; alignment <= 256; alignment *= 2) {
        p = ns_aligned_malloc(100, alignment, NULL);
        if (p == NULL) {
            printf("  FAILED: aligned_malloc returned NULL for alignment %lu\n", (unsigned long)alignment);
            return 0;
        }
        
        /* Check alignment */
        if (((ns_uintptr)p & (alignment - 1)) != 0) {
            printf("  FAILED: pointer not aligned to %lu bytes\n", (unsigned long)alignment);
            ns_aligned_free(p, NULL);
            return 0;
        }
        
        ns_aligned_free(p, NULL);
    }
    
    printf("  PASSED\n");
    return 1;
}

static int test_aligned_realloc(void)
{
    void* p;
    void* p2;
    size_t alignment;
    size_t i;
    unsigned char* pBytes;
    
    printf("Testing aligned realloc...\n");
    
    alignment = 64;
    
    p = ns_aligned_malloc(100, alignment, NULL);
    if (p == NULL) {
        printf("  FAILED: aligned_malloc returned NULL\n");
        return 0;
    }
    
    /* Write some data */
    pBytes = (unsigned char*)p;
    for (i = 0; i < 100; i++) {
        pBytes[i] = (unsigned char)(i & 0xFF);
    }
    
    /* Realloc to larger size */
    p2 = ns_aligned_realloc(p, 200, alignment, NULL);
    if (p2 == NULL) {
        printf("  FAILED: aligned_realloc returned NULL\n");
        ns_aligned_free(p, NULL);
        return 0;
    }
    
    /* Check alignment */
    if (((ns_uintptr)p2 & (alignment - 1)) != 0) {
        printf("  FAILED: reallocated pointer not aligned to %lu bytes\n", (unsigned long)alignment);
        ns_aligned_free(p2, NULL);
        return 0;
    }
    
    /* Verify data was preserved */
    pBytes = (unsigned char*)p2;
    for (i = 0; i < 100; i++) {
        if (pBytes[i] != (unsigned char)(i & 0xFF)) {
            printf("  FAILED: data not preserved after realloc at index %lu\n", (unsigned long)i);
            ns_aligned_free(p2, NULL);
            return 0;
        }
    }
    
    ns_aligned_free(p2, NULL);
    
    printf("  PASSED\n");
    return 1;
}

static int test_aligned_realloc_different_pointer(void)
{
    void* pOriginal;
    void* p;
    void* p2;
    void* pOriginalUnaligned;
    void* pNewUnaligned;
    size_t alignment;
    size_t i;
    unsigned char* pBytes;
    size_t initialSize;
    size_t newSize;
    int attempts;
    int maxAttempts;
    int pointerChanged;
    
    printf("Testing aligned realloc with pointer change...\n");
    
    alignment = 64;
    initialSize = 64;
    newSize = 1024;
    maxAttempts = 25;
    pointerChanged = 0;
    
    p = ns_aligned_malloc(initialSize, alignment, NULL);
    if (p == NULL) {
        printf("  FAILED: aligned_malloc returned NULL\n");
        return 0;
    }
    
    pOriginal = p;
    pOriginalUnaligned = ((void**)p)[-1];
    
    /* Write unique data pattern */
    pBytes = (unsigned char*)p;
    for (i = 0; i < initialSize; i++) {
        pBytes[i] = (unsigned char)((i * 7 + 13) & 0xFF);
    }
    
    /* Keep doubling the size until the underlying pointer changes */
    for (attempts = 0; attempts < maxAttempts; attempts++) {
        newSize *= 2;
        
        p2 = ns_aligned_realloc(p, newSize, alignment, NULL);
        if (p2 == NULL) {
            printf("  FAILED: aligned_realloc returned NULL at size %lu\n", (unsigned long)newSize);
            ns_aligned_free(p, NULL);
            return 0;
        }
        
        /* Check alignment of new pointer */
        if (((ns_uintptr)p2 & (alignment - 1)) != 0) {
            printf("  FAILED: reallocated pointer not aligned to %lu bytes\n", (unsigned long)alignment);
            ns_aligned_free(p2, NULL);
            return 0;
        }
        
        /* Verify all original data was preserved */
        pBytes = (unsigned char*)p2;
        for (i = 0; i < initialSize; i++) {
            unsigned char expected = (unsigned char)((i * 7 + 13) & 0xFF);
            if (pBytes[i] != expected) {
                printf("  FAILED: data not preserved after realloc at index %lu (got %u, expected %u)\n", 
                       (unsigned long)i, (unsigned int)pBytes[i], (unsigned int)expected);
                printf("          Old pointer: %p, New pointer: %p\n", (void*)p, (void*)p2);
                ns_aligned_free(p2, NULL);
                return 0;
            }
        }
        
        /* Check if the underlying unaligned pointer changed */
        pNewUnaligned = ((void**)p2)[-1];
        if (pNewUnaligned != pOriginalUnaligned) {
            pointerChanged = 1;
            printf("  INFO: Underlying pointer changed after %d realloc(s) at size %lu bytes\n", 
                   attempts + 1, (unsigned long)newSize);
            printf("        Original unaligned: %p, New unaligned: %p\n", 
                   (void*)pOriginalUnaligned, (void*)pNewUnaligned);
            printf("        Original aligned: %p, New aligned: %p\n", 
                   (void*)pOriginal, (void*)p2);
            break;
        }
        
        /* Update p for next iteration */
        p = p2;
    }
    
    /* Verify that we actually got a different pointer */
    if (!pointerChanged) {
        printf("  FAILED: underlying pointer never changed after %d attempts (final size: %lu bytes)\n", 
               maxAttempts, (unsigned long)newSize);
        ns_aligned_free(p2, NULL);
        return 0;
    }
    
    ns_aligned_free(p2, NULL);
    
    printf("  PASSED\n");
    return 1;
}

static int test_aligned_realloc_from_null(void)
{
    void* p;
    size_t alignment;
    
    printf("Testing aligned realloc from NULL...\n");
    
    alignment = 32;
    
    p = ns_aligned_realloc(NULL, 100, alignment, NULL);
    if (p == NULL) {
        printf("  FAILED: aligned_realloc from NULL returned NULL\n");
        return 0;
    }
    
    /* Check alignment */
    if (((ns_uintptr)p & (alignment - 1)) != 0) {
        printf("  FAILED: pointer not aligned to %lu bytes\n", (unsigned long)alignment);
        ns_aligned_free(p, NULL);
        return 0;
    }
    
    ns_aligned_free(p, NULL);
    
    printf("  PASSED\n");
    return 1;
}

static int test_free_null(void)
{
    printf("Testing free with NULL pointer...\n");
    
    /* Should not crash */
    ns_free(NULL, NULL);
    
    printf("  PASSED\n");
    return 1;
}

static int test_allocation_callbacks_init(void)
{
    ns_allocation_callbacks callbacks;
    
    printf("Testing allocation_callbacks initialization...\n");
    
    callbacks = ns_allocation_callbacks_init_default();
    if (callbacks.onMalloc == NULL || callbacks.onRealloc == NULL || callbacks.onFree == NULL) {
        printf("  FAILED: default callbacks not initialized properly\n");
        return 0;
    }
    
    callbacks = ns_allocation_callbacks_init_copy(NULL);
    if (callbacks.onMalloc == NULL || callbacks.onRealloc == NULL || callbacks.onFree == NULL) {
        printf("  FAILED: copy from NULL not initialized properly\n");
        return 0;
    }
    
    printf("  PASSED\n");
    return 1;
}

int main(int argc, char** argv)
{
    int passedTests = 0;
    int totalTests = 0;
    
    (void)argc;
    (void)argv;
    
    printf("Running allocation_callbacks tests...\n\n");
    
    totalTests++; if (test_basic_malloc_free()) passedTests++;
    totalTests++; if (test_basic_calloc()) passedTests++;
    totalTests++; if (test_basic_realloc()) passedTests++;
    totalTests++; if (test_custom_allocator()) passedTests++;
    totalTests++; if (test_aligned_malloc_free()) passedTests++;
    totalTests++; if (test_aligned_realloc()) passedTests++;
    totalTests++; if (test_aligned_realloc_different_pointer()) passedTests++;
    totalTests++; if (test_aligned_realloc_from_null()) passedTests++;
    totalTests++; if (test_free_null()) passedTests++;
    totalTests++; if (test_allocation_callbacks_init()) passedTests++;
    
    printf("\n========================================\n");
    printf("Tests passed: %d/%d\n", passedTests, totalTests);
    printf("========================================\n");
    
    if (passedTests == totalTests) {
        printf("All tests PASSED!\n");
        return 0;
    } else {
        printf("Some tests FAILED!\n");
        return 1;
    }
}
/* END Tests */

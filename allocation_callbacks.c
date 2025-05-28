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

NS_API void ns_aligned_free(void* p, const ns_allocation_callbacks* pAllocationCallbacks)
{
    ns_free(((void**)p)[-1], pAllocationCallbacks);
}
/* END allocation_callbacks.c */



/* TODO: Implement a proper test, particularly for aligned_malloc() and aligned_free(). */
int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    return 0;
}

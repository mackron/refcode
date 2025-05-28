/*
A simple generic insert sort. Nothing special going on here. You will most likely want to build something more
specialised if you need something high performance. Will add to this as demanded by my own requirements.
*/
#include <stddef.h> /* for size_t */

#ifndef NS_API
#define NS_API
#endif

#ifndef NS_INLINE
#define NS_INLINE
#endif

/* BEG sort.h */
NS_API void ns_sort(void* pBase, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);
/* END sort.h */

/* BEG sort.c */
NS_API void ns_sort(void* pBase, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData)
{
    /* Just a simple insert sort for now. Will improve on this later. */
    size_t i;
    size_t j;

    for (i = 1; i < count; i += 1) {
        for (j = i; j > 0; j -= 1) {
            void* pA = (char*)pBase + (j - 1) * stride;
            void* pB = (char*)pBase + j * stride;

            if (compareProc(pUserData, pA, pB) <= 0) {
                break;
            }

            /* Swap. */
            {
                size_t k;
                char* a = (char*)pA;
                char* b = (char*)pB;
                
                for (k = 0; k < stride; k += 1) {
                    char temp = *a;
                    *a++ = *b;
                    *b++ = temp;
                }
            }
        }
    }
}
/* END sort.c */


/* TODO: Implement a proper test. */
int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    
    return 0;
}

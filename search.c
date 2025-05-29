#include <stddef.h>
#include <stdlib.h>

#ifndef NS_API
#define NS_API
#endif

/* BEG binary_search.h */
NS_API void* ns_binary_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);
/* END binary_search.h */

/* BEG linear_search.h */
NS_API void* ns_linear_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);
/* END linear_search.h */

/* BEG sorted_search.h */
NS_API void* ns_sorted_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);
/* END sorted_search.h */



/* BEG binary_search.c */
NS_API void* ns_binary_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData)
{
    size_t iStart;
    size_t iEnd;
    size_t iMid;

    if (count == 0) {
        return NULL;
    }

    iStart = 0;
    iEnd = count - 1;

    while (iStart <= iEnd) {
        int compareResult;

        iMid = iStart + (iEnd - iStart) / 2;

        compareResult = compareProc(pUserData, pKey, (char*)pList + (iMid * stride));
        if (compareResult < 0) {
            iEnd = iMid - 1;
        } else if (compareResult > 0) {
            iStart = iMid + 1;
        } else {
            return (void*)((char*)pList + (iMid * stride));
        }
    }

    return NULL;
}
/* END binary_search.c */

/* BEG linear_search.c */
NS_API void* ns_linear_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData)
{
    size_t i;

    for (i = 0; i < count; i+= 1) {
        int compareResult = compareProc(pUserData, pKey, (char*)pList + (i * stride));
        if (compareResult == 0) {
            return (void*)((char*)pList + (i * stride));
        }
    }

    return NULL;
}
/* END linear_search.c */

/* BEG sorted_search.c */
NS_API void* ns_sorted_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData)
{
    const size_t threshold = 10;

    if (count < threshold) {
        return ns_linear_search(pKey, pList, count, stride, compareProc, pUserData);
    } else {
        return ns_binary_search(pKey, pList, count, stride, compareProc, pUserData);
    }
}
/* END sorted_search.c */



#include <stdio.h>
#include <string.h>

int compare_int(void* context, const void* a, const void* b)
{
    (void)context;
    return (*(int*)a - *(int*)b);
}

int compare_strings(void* context, const void* a, const void* b)
{
    (void)context;
    return strcmp((const char*)a, *(const char**)b);
}

int main(void)
{
    {
        int arr[] = {1, 2, 3, 4, 5};
        int key = 4;
        int* result = ns_binary_search(&key, arr, 5, sizeof(int), compare_int, NULL);
        if (result != NULL) {
            printf("Key found at index %u\n", (unsigned int)(result - arr));
        } else {
            printf("Key not found\n");
        }
    }
    
    {
        char* list[] = {"apple", "banana", "cherry", "date"};
        const char* key = "cherry";
        char** result = ns_binary_search(key, list, 4, sizeof(char*), compare_strings, NULL);
        if (result != NULL) {
          printf("String found at index %u\n", (unsigned int)(result - list));
        } else {
          printf("String not found\n");
        }
    }

    return 0;
}

#if defined(__has_attribute)
    #if __has_attribute(format)
        #define NS_ATTRIBUTE_FORMAT(fmt, va) __attribute__((format(printf, fmt, va)))
    #endif
#endif
#ifndef NS_ATTRIBUTE_FORMAT
#define NS_ATTRIBUTE_FORMAT(fmt, va)
#endif
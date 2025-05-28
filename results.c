/*
The result codes listed here are just what I use as standard across all my projects. There's probably
more here than is necessary so I may streamline this later.
*/

#ifndef NS_API
#define NS_API
#endif

/* BEG result.h */
typedef enum
{
    NS_SUCCESS                       =  0,
    NS_ERROR                         = -1,  /* Generic, unknown error. */
    NS_INVALID_ARGS                  = -2,
    NS_INVALID_OPERATION             = -3,
    NS_OUT_OF_MEMORY                 = -4,
    NS_OUT_OF_RANGE                  = -5,
    NS_ACCESS_DENIED                 = -6,
    NS_DOES_NOT_EXIST                = -7,
    NS_ALREADY_EXISTS                = -8,
    NS_TOO_MANY_OPEN_FILES           = -9,
    NS_INVALID_FILE                  = -10,
    NS_TOO_BIG                       = -11,
    NS_PATH_TOO_LONG                 = -12,
    NS_NOT_DIRECTORY                 = -14,
    NS_IS_DIRECTORY                  = -15,
    NS_DIRECTORY_NOT_EMPTY           = -16,
    NS_AT_END                        = -17,
    NS_NO_SPACE                      = -18,
    NS_BUSY                          = -19,
    NS_IO_ERROR                      = -20,
    NS_INTERRUPT                     = -21,
    NS_UNAVAILABLE                   = -22,
    NS_ALREADY_IN_USE                = -23,
    NS_BAD_ADDRESS                   = -24,
    NS_BAD_SEEK                      = -25,
    NS_BAD_PIPE                      = -26,
    NS_DEADLOCK                      = -27,
    NS_TOO_MANY_LINKS                = -28,
    NS_NOT_IMPLEMENTED               = -29,
    NS_NO_DATA_AVAILABLE             = -32,
    NS_INVALID_DATA                  = -33,
    NS_TIMEOUT                       = -34,
    NS_NO_NETWORK                    = -35,
    NS_NOT_UNIQUE                    = -36,
    NS_NOT_SOCKET                    = -37,
    NS_NO_ADDRESS                    = -38,
    NS_BAD_PROTOCOL                  = -39,
    NS_PROTOCOL_UNAVAILABLE          = -40,
    NS_PROTOCOL_NOT_SUPPORTED        = -41,
    NS_PROTOCOL_FAMILY_NOT_SUPPORTED = -42,
    NS_ADDRESS_FAMILY_NOT_SUPPORTED  = -43,
    NS_SOCKET_NOT_SUPPORTED          = -44,
    NS_CONNECTION_RESET              = -45,
    NS_ALREADY_CONNECTED             = -46,
    NS_NOT_CONNECTED                 = -47,
    NS_CONNECTION_REFUSED            = -48,
    NS_NO_HOST                       = -49,
    NS_IN_PROGRESS                   = -50,
    NS_CANCELLED                     = -51,
    NS_MEMORY_ALREADY_MAPPED         = -52,
    NS_CHECKSUM_MISMATCH             = -100,
    NS_NO_BACKEND                    = -101,

    /* Non-Error Result Codes. */
    NS_NEEDS_MORE_INPUT              = 100, /* Some stream needs more input data before it can be processed. */
    NS_HAS_MORE_OUTPUT               = 102, /* Some stream has more output data to be read, but there's not enough room in the output buffer. */
} ns_result;
/* END result.h */

/* BEG result_from_errno.h */
NS_API ns_result ns_result_from_errno(int error);
/* END result_from_errno.h */

/* BEG result_from_GetLastError.h */
NS_API ns_result ns_result_from_GetLastError(void);
/* END result_from_GetLastError.h */




/* BEG result_from_errno.c */
#include <errno.h>

NS_API ns_result ns_result_from_errno(int error)
{
    if (error == 0) {
        return NS_SUCCESS;
    }
#ifdef EPERM
    else if (error == EPERM) { return NS_INVALID_OPERATION; }
#endif
#ifdef ENOENT
    else if (error == ENOENT) { return NS_DOES_NOT_EXIST; }
#endif
#ifdef ESRCH
    else if (error == ESRCH) { return NS_DOES_NOT_EXIST; }
#endif
#ifdef EINTR
    else if (error == EINTR) { return NS_INTERRUPT; }
#endif
#ifdef EIO
    else if (error == EIO) { return NS_IO_ERROR; }
#endif
#ifdef ENXIO
    else if (error == ENXIO) { return NS_DOES_NOT_EXIST; }
#endif
#ifdef E2BIG
    else if (error == E2BIG) { return NS_INVALID_ARGS; }
#endif
#ifdef ENOEXEC
    else if (error == ENOEXEC) { return NS_INVALID_FILE; }
#endif
#ifdef EBADF
    else if (error == EBADF) { return NS_INVALID_FILE; }
#endif
#ifdef EAGAIN
    else if (error == EAGAIN) { return NS_UNAVAILABLE; }
#endif
#ifdef ENOMEM
    else if (error == ENOMEM) { return NS_OUT_OF_MEMORY; }
#endif
#ifdef EACCES
    else if (error == EACCES) { return NS_ACCESS_DENIED; }
#endif
#ifdef EFAULT
    else if (error == EFAULT) { return NS_BAD_ADDRESS; }
#endif
#ifdef EBUSY
    else if (error == EBUSY) { return NS_BUSY; }
#endif
#ifdef EEXIST
    else if (error == EEXIST) { return NS_ALREADY_EXISTS; }
#endif
#ifdef ENODEV
    else if (error == ENODEV) { return NS_DOES_NOT_EXIST; }
#endif
#ifdef ENOTDIR
    else if (error == ENOTDIR) { return NS_NOT_DIRECTORY; }
#endif
#ifdef EISDIR
    else if (error == EISDIR) { return NS_IS_DIRECTORY; }
#endif
#ifdef EINVAL
    else if (error == EINVAL) { return NS_INVALID_ARGS; }
#endif
#ifdef ENFILE
    else if (error == ENFILE) { return NS_TOO_MANY_OPEN_FILES; }
#endif
#ifdef EMFILE
    else if (error == EMFILE) { return NS_TOO_MANY_OPEN_FILES; }
#endif
#ifdef ENOTTY
    else if (error == ENOTTY) { return NS_INVALID_OPERATION; }
#endif
#ifdef ETXTBSY
    else if (error == ETXTBSY) { return NS_BUSY; }
#endif
#ifdef EFBIG
    else if (error == EFBIG) { return NS_TOO_BIG; }
#endif
#ifdef ENOSPC
    else if (error == ENOSPC) { return NS_NO_SPACE; }
#endif
#ifdef ESPIPE
    else if (error == ESPIPE) { return NS_BAD_SEEK; }
#endif
#ifdef EROFS
    else if (error == EROFS) { return NS_ACCESS_DENIED; }
#endif
#ifdef EPIPE
    else if (error == EPIPE) { return NS_BAD_PIPE; }
#endif
#ifdef EDOM
    else if (error == EDOM) { return NS_OUT_OF_RANGE; }
#endif
#ifdef ERANGE
    else if (error == ERANGE) { return NS_OUT_OF_RANGE; }
#endif
#ifdef EDEADLK
    else if (error == EDEADLK) { return NS_DEADLOCK; }
#endif
#ifdef ENAMETOOLONG
    else if (error == ENAMETOOLONG) { return NS_PATH_TOO_LONG; }
#endif
#ifdef ENOSYS
    else if (error == ENOSYS) { return NS_NOT_IMPLEMENTED; }
#endif
#ifdef ENOTEMPTY
    else if (error == ENOTEMPTY) { return NS_DIRECTORY_NOT_EMPTY; }
#endif
#ifdef ELNRNG
    else if (error == ELNRNG) { return NS_OUT_OF_RANGE; }
#endif
#ifdef EBFONT
    else if (error == EBFONT) { return NS_INVALID_FILE; }
#endif
#ifdef ENODATA
    else if (error == ENODATA) { return NS_NO_DATA_AVAILABLE; }
#endif
#ifdef ETIME
    else if (error == ETIME) { return NS_TIMEOUT; }
#endif
#ifdef ENOSR
    else if (error == ENOSR) { return NS_NO_DATA_AVAILABLE; }
#endif
#ifdef ENONET
    else if (error == ENONET) { return NS_NO_NETWORK; }
#endif
#ifdef EOVERFLOW
    else if (error == EOVERFLOW) { return NS_TOO_BIG; }
#endif
#ifdef ELIBACC
    else if (error == ELIBACC) { return NS_ACCESS_DENIED; }
#endif
#ifdef ELIBBAD
    else if (error == ELIBBAD) { return NS_INVALID_FILE; }
#endif
#ifdef ELIBSCN
    else if (error == ELIBSCN) { return NS_INVALID_FILE; }
#endif
#ifdef EILSEQ
    else if (error == EILSEQ) { return NS_INVALID_DATA; }
#endif
#ifdef ENOTSOCK
    else if (error == ENOTSOCK) { return NS_NOT_SOCKET; }
#endif
#ifdef EDESTADDRREQ
    else if (error == EDESTADDRREQ) { return NS_NO_ADDRESS; }
#endif
#ifdef EMSGSIZE
    else if (error == EMSGSIZE) { return NS_TOO_BIG; }
#endif
#ifdef EPROTOTYPE
    else if (error == EPROTOTYPE) { return NS_BAD_PROTOCOL; }
#endif
#ifdef ENOPROTOOPT
    else if (error == ENOPROTOOPT) { return NS_PROTOCOL_UNAVAILABLE; }
#endif
#ifdef EPROTONOSUPPORT
    else if (error == EPROTONOSUPPORT) { return NS_PROTOCOL_NOT_SUPPORTED; }
#endif
#ifdef ESOCKTNOSUPPORT
    else if (error == ESOCKTNOSUPPORT) { return NS_SOCKET_NOT_SUPPORTED; }
#endif
#ifdef EOPNOTSUPP
    else if (error == EOPNOTSUPP) { return NS_INVALID_OPERATION; }
#endif
#ifdef EPFNOSUPPORT
    else if (error == EPFNOSUPPORT) { return NS_PROTOCOL_FAMILY_NOT_SUPPORTED; }
#endif
#ifdef EAFNOSUPPORT
    else if (error == EAFNOSUPPORT) { return NS_ADDRESS_FAMILY_NOT_SUPPORTED; }
#endif
#ifdef EADDRINUSE
    else if (error == EADDRINUSE) { return NS_ALREADY_IN_USE; }
#endif
#ifdef ENETDOWN
    else if (error == ENETDOWN) { return NS_NO_NETWORK; }
#endif
#ifdef ENETUNREACH
    else if (error == ENETUNREACH) { return NS_NO_NETWORK; }
#endif
#ifdef ENETRESET
    else if (error == ENETRESET) { return NS_NO_NETWORK; }
#endif
#ifdef ECONNABORTED
    else if (error == ECONNABORTED) { return NS_NO_NETWORK; }
#endif
#ifdef ECONNRESET
    else if (error == ECONNRESET) { return NS_CONNECTION_RESET; }
#endif
#ifdef ENOBUFS
    else if (error == ENOBUFS) { return NS_NO_SPACE; }
#endif
#ifdef EISCONN
    else if (error == EISCONN) { return NS_ALREADY_CONNECTED; }
#endif
#ifdef ENOTCONN
    else if (error == ENOTCONN) { return NS_NOT_CONNECTED; }
#endif
#ifdef ETIMEDOUT
    else if (error == ETIMEDOUT) { return NS_TIMEOUT; }
#endif
#ifdef ECONNREFUSED
    else if (error == ECONNREFUSED) { return NS_CONNECTION_REFUSED; }
#endif
#ifdef EHOSTDOWN
    else if (error == EHOSTDOWN) { return NS_NO_HOST; }
#endif
#ifdef EHOSTUNREACH
    else if (error == EHOSTUNREACH) { return NS_NO_HOST; }
#endif
#ifdef EALREADY
    else if (error == EALREADY) { return NS_IN_PROGRESS; }
#endif
#ifdef EINPROGRESS
    else if (error == EINPROGRESS) { return NS_IN_PROGRESS; }
#endif
#ifdef ESTALE
    else if (error == ESTALE) { return NS_INVALID_FILE; }
#endif
#ifdef EREMOTEIO
    else if (error == EREMOTEIO) { return NS_IO_ERROR; }
#endif
#ifdef EDQUOT
    else if (error == EDQUOT) { return NS_NO_SPACE; }
#endif
#ifdef ENOMEDIUM
    else if (error == ENOMEDIUM) { return NS_DOES_NOT_EXIST; }
#endif
#ifdef ECANCELED
    else if (error == ECANCELED) { return NS_CANCELLED; }
#endif
    
    return NS_ERROR;
}
/* END result_from_errno.c */

/* BEG result_from_GetLastError.c */
#if defined(_WIN32)
#include <windows.h> /* For GetLastError, ERROR_* constants. */

NS_API ns_result ns_result_from_GetLastError(void)
{
    switch (GetLastError())
    {
        case ERROR_SUCCESS:                return NS_SUCCESS;
        case ERROR_INVALID_FUNCTION:       return NS_INVALID_OPERATION;
        case ERROR_FILE_NOT_FOUND:         return NS_DOES_NOT_EXIST;
        case ERROR_PATH_NOT_FOUND:         return NS_DOES_NOT_EXIST;
        case ERROR_TOO_MANY_OPEN_FILES:    return NS_TOO_MANY_OPEN_FILES;
        case ERROR_ACCESS_DENIED:          return NS_ACCESS_DENIED;
        case ERROR_INVALID_HANDLE:         return NS_INVALID_ARGS;
        case ERROR_NOT_ENOUGH_MEMORY:      return NS_OUT_OF_MEMORY;
        case ERROR_OUTOFMEMORY:            return NS_OUT_OF_MEMORY;
        case ERROR_INVALID_DATA:           return NS_INVALID_DATA;
        case ERROR_NO_DATA:                return NS_NO_DATA_AVAILABLE;
        case ERROR_OPERATION_ABORTED:      return NS_CANCELLED;
        case ERROR_SEM_TIMEOUT:            return NS_TIMEOUT;
        case ERROR_BUSY:                   return NS_BUSY;
        default:                           return NS_ERROR; /* Generic error. */
    }
}
#endif /* _WIN32 */
/* END result_from_GetLastError.c */

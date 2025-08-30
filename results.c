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
    NS_NAME_TOO_LONG                 = -13,
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
    NS_NO_MESSAGE                    = -30,
    NS_BAD_MESSAGE                   = -31,
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
    NS_DIFFERENT_DEVICE              = -53,
    NS_CHECKSUM_MISMATCH             = -100,
    NS_NO_BACKEND                    = -101,

    /* Non-Error Result Codes. */
    NS_NEEDS_MORE_INPUT              = 100, /* Some stream needs more input data before it can be processed. */
    NS_HAS_MORE_OUTPUT               = 102, /* Some stream has more output data to be read, but there's not enough room in the output buffer. */
} ns_result;
/* END result.h */

/* BEG result_to_string.h */
NS_API const char* ns_result_to_string(ns_result result);
/* END result_to_string.h */

/* BEG result_from_errno.h */
NS_API ns_result ns_result_from_errno(int error);
/* END result_from_errno.h */

/* BEG result_from_GetLastError.h */
NS_API ns_result ns_result_from_GetLastError(void);
/* END result_from_GetLastError.h */



/* BEG result_to_string.c */
NS_API const char* ns_result_to_string(ns_result result)
{
    switch (result)
    {
        case NS_SUCCESS:                       return "No error";
        case NS_ERROR:                         return "Unknown error";
        case NS_INVALID_ARGS:                  return "Invalid argument";
        case NS_INVALID_OPERATION:             return "Invalid operation";
        case NS_OUT_OF_MEMORY:                 return "Out of memory";
        case NS_OUT_OF_RANGE:                  return "Out of range";
        case NS_ACCESS_DENIED:                 return "Permission denied";
        case NS_DOES_NOT_EXIST:                return "Resource does not exist";
        case NS_ALREADY_EXISTS:                return "Resource already exists";
        case NS_TOO_MANY_OPEN_FILES:           return "Too many open files";
        case NS_INVALID_FILE:                  return "Invalid file";
        case NS_TOO_BIG:                       return "Too large";
        case NS_PATH_TOO_LONG:                 return "Path too long";
        case NS_NAME_TOO_LONG:                 return "Name too long";
        case NS_NOT_DIRECTORY:                 return "Not a directory";
        case NS_IS_DIRECTORY:                  return "Is a directory";
        case NS_DIRECTORY_NOT_EMPTY:           return "Directory not empty";
        case NS_AT_END:                        return "At end";
        case NS_NO_SPACE:                      return "No space available";
        case NS_BUSY:                          return "Device or resource busy";
        case NS_IO_ERROR:                      return "Input/output error";
        case NS_INTERRUPT:                     return "Interrupted";
        case NS_UNAVAILABLE:                   return "Resource unavailable";
        case NS_ALREADY_IN_USE:                return "Resource already in use";
        case NS_BAD_ADDRESS:                   return "Bad address";
        case NS_BAD_SEEK:                      return "Illegal seek";
        case NS_BAD_PIPE:                      return "Broken pipe";
        case NS_DEADLOCK:                      return "Deadlock";
        case NS_TOO_MANY_LINKS:                return "Too many links";
        case NS_NOT_IMPLEMENTED:               return "Not implemented";
        case NS_NO_MESSAGE:                    return "No message of desired type";
        case NS_BAD_MESSAGE:                   return "Invalid message";
        case NS_NO_DATA_AVAILABLE:             return "No data available";
        case NS_INVALID_DATA:                  return "Invalid data";
        case NS_TIMEOUT:                       return "Timeout";
        case NS_NO_NETWORK:                    return "Network unavailable";
        case NS_NOT_UNIQUE:                    return "Not unique";
        case NS_NOT_SOCKET:                    return "Socket operation on non-socket";
        case NS_NO_ADDRESS:                    return "Destination address required";
        case NS_BAD_PROTOCOL:                  return "Protocol wrong type for socket";
        case NS_PROTOCOL_UNAVAILABLE:          return "Protocol not available";
        case NS_PROTOCOL_NOT_SUPPORTED:        return "Protocol not supported";
        case NS_PROTOCOL_FAMILY_NOT_SUPPORTED: return "Protocol family not supported";
        case NS_ADDRESS_FAMILY_NOT_SUPPORTED:  return "Address family not supported";
        case NS_SOCKET_NOT_SUPPORTED:          return "Socket type not supported";
        case NS_CONNECTION_RESET:              return "Connection reset";
        case NS_ALREADY_CONNECTED:             return "Already connected";
        case NS_NOT_CONNECTED:                 return "Not connected";
        case NS_CONNECTION_REFUSED:            return "Connection refused";
        case NS_NO_HOST:                       return "No host";
        case NS_IN_PROGRESS:                   return "Operation in progress";
        case NS_CANCELLED:                     return "Operation cancelled";
        case NS_MEMORY_ALREADY_MAPPED:         return "Memory already mapped";
        case NS_DIFFERENT_DEVICE:              return "Different device";
        default:                               return "Unknown error";
    }
}
/* END result_to_string.c */


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
#ifdef EXDEV
    else if (error == EXDEV) { return NS_DIFFERENT_DEVICE; }
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
        case ERROR_NOT_ENOUGH_MEMORY:      return NS_OUT_OF_MEMORY;
        case ERROR_OUTOFMEMORY:            return NS_OUT_OF_MEMORY;
        case ERROR_BUSY:                   return NS_BUSY;
        case ERROR_SEM_TIMEOUT:            return NS_TIMEOUT;
        case ERROR_ALREADY_EXISTS:         return NS_ALREADY_EXISTS;
        case ERROR_FILE_EXISTS:            return NS_ALREADY_EXISTS;
        case ERROR_ACCESS_DENIED:          return NS_ACCESS_DENIED;
        case ERROR_WRITE_PROTECT:          return NS_ACCESS_DENIED;
        case ERROR_PRIVILEGE_NOT_HELD:     return NS_ACCESS_DENIED;
        case ERROR_SHARING_VIOLATION:      return NS_ACCESS_DENIED;
        case ERROR_LOCK_VIOLATION:         return NS_ACCESS_DENIED;
        case ERROR_FILE_NOT_FOUND:         return NS_DOES_NOT_EXIST;
        case ERROR_PATH_NOT_FOUND:         return NS_DOES_NOT_EXIST;
        case ERROR_INVALID_NAME:           return NS_INVALID_ARGS;
        case ERROR_BAD_PATHNAME:           return NS_INVALID_ARGS;
        case ERROR_INVALID_PARAMETER:      return NS_INVALID_ARGS;
        case ERROR_INVALID_HANDLE:         return NS_INVALID_ARGS;
        case ERROR_INVALID_FUNCTION:       return NS_INVALID_OPERATION;
        case ERROR_FILENAME_EXCED_RANGE:   return NS_PATH_TOO_LONG;
        case ERROR_DIRECTORY:              return NS_NOT_DIRECTORY;
        case ERROR_DIR_NOT_EMPTY:          return NS_DIRECTORY_NOT_EMPTY;
        case ERROR_FILE_TOO_LARGE:         return NS_TOO_BIG;
        case ERROR_DISK_FULL:              return NS_OUT_OF_RANGE;
        case ERROR_HANDLE_EOF:             return NS_AT_END;
        case ERROR_SEEK:                   return NS_BAD_SEEK;
        case ERROR_OPERATION_ABORTED:      return NS_CANCELLED;
        case ERROR_CANCELLED:              return NS_INTERRUPT;
        case ERROR_TOO_MANY_OPEN_FILES:    return NS_TOO_MANY_OPEN_FILES;
        case ERROR_INVALID_DATA:           return NS_INVALID_DATA;
        case ERROR_NO_DATA:                return NS_NO_DATA_AVAILABLE;
        case ERROR_NOT_SAME_DEVICE:        return NS_DIFFERENT_DEVICE;
        default:                           return NS_ERROR; /* Generic error. */
    }
}
#endif /* _WIN32 */
/* END result_from_GetLastError.c */



int main(void)
{
    return 0;
}

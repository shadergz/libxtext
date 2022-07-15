#ifndef LIBBIN_BINTYPES_H
#define LIBBIN_BINTYPES_H

#include <stdbool.h>

typedef int fd_t;
typedef int flags_t;

typedef enum
{
    BIN_E_OK = 0,
    /* Can't open the file, maybe the file not exist */
    BIN_E_OPEN_FILE,

    /* Can't close the file, maybe has been deleted */
    BIN_E_CLOSE_FILE

} bin_err_e;

typedef struct
{
#if defined(__unix__)
    fd_t fd;
#if _POSIX_C_SOURCE >= 200809L
    fd_t dir_fd;
#endif
    int internal_errno;
    flags_t fd_flags;
#endif
    /* A internal error status value (allow the getting method) */
    bin_err_e error_status;

} bin_ctx_t;

#endif

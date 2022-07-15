#ifndef LIBBIN_BINTYPES_H
#define LIBBIN_BINTYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef int fd_t;
typedef int flags_t;

typedef enum
{
    BIN_E_OK = 0,
    /* Can't open the file, maybe the file not exist */
    BIN_E_OPEN_FILE,

    /* Can't close the file, maybe has been deleted */
    BIN_E_CLOSE_FILE,

    /* A recently allocation using the malloc function has been failed */
    BIN_E_MALLOCATION_ERROR,

    /* The supposed binary file is empty */
    BIN_E_IS_EMPTY,

    /* The executable has been alredy parsed */
    BIN_E_ALREDY_PARSED,

#if defined(__unix__)
    /* A recently call for function fstat has returned a fail value */
    BIN_E_FSTAT_FAILED,

    /* A recently call for mmap has failed */
    BIN_E_MMAP_FAILED,

    /* A recently call for munmap has failed */
    BIN_E_MUNMAP_FAILED,

#endif

    BIN_E_FINAL_NULL_VALUE

} bin_err_e;

typedef struct
{
    /* Binary file pathname */
    char *pathname;

#if defined(__unix__)
    fd_t fd;
#if _POSIX_C_SOURCE >= 200809L
    fd_t dir_fd;
#endif
    int internal_errno;
    flags_t fd_flags;
#endif

    size_t binary_file_size;

    /* A internal error status value (allow the getting method) */
    bin_err_e error_status;

    union {
        struct {
#if defined(__unix__)
            unsigned char *map_start;
            uintptr_t map_end;
            size_t map_size;
#endif
        } mapped_area;

        bool using_mapped;
    };

} bin_ctx_t;

#endif

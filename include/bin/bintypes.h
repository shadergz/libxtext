#ifndef LIBBIN_BINTYPES_H
#define LIBBIN_BINTYPES_H

#include <stdbool.h>

#include <stddef.h>

#include <stdint.h>

typedef int32_t fd_t;
typedef int32_t flags_t;

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
    /* A recently try to read a chunk of memory has been failed */
    BIN_E_CANT_READ,

    /* A invalid file has been passed */
    BIN_E_INVALID_FILE,

    BIN_E_FINAL_NULL_VALUE

} bin_err_e;

typedef enum
{
    // Uknow executable file
    BT_UNKNOW = 0,
    // Portable executable
    BT_PE_FILE,
    // Executable and Linkable Format
    BT_ELF_FILE
} bin_type_e;

typedef struct
{
    /* Binary file pathname */
    char *pathname;

#if defined(__unix__)
    fd_t fd;
#if _POSIX_C_SOURCE >= 200809L
    fd_t dir_fd;
#endif
    int32_t internal_errno;
    flags_t fd_flags;
#endif

    size_t binary_file_size;

    /* A internal error status value (allow the getting method) */
    bin_err_e error_status;

#if defined(__unix__)
    unsigned char *map_start;
    uintptr_t map_end;
    size_t map_size;
#endif
    bool using_mapped;

    /* Executable type (used for parser another structures) */
    bin_type_e binary_type;

} bin_ctx_t;

#endif

#ifndef BIN_TYPES_H
#define BIN_TYPES_H

#include <stdbool.h>

#include <stddef.h>
#include <stdint.h>

#include <xtext/common.h>

/* File descriptor type, specifiers the FD integral value */
typedef int32_t native_fd_t;
/* Flags type used in IO operations */
typedef int32_t ioflags_t;

typedef enum
{
  XTEXT_E_OK = 0,
  /* Can't open the file, maybe the file not exist */
  XTEXT_E_OPEN_FILE,
  /* Can't close the file, maybe has been deleted */
  XTEXT_E_CLOSE_FILE,
  /* A recent allocation using the malloc function has been failed */
  XTEXT_E_MALLOCATION_ERROR,
  /* The supposed binary file is empty */
  XTEXT_E_IS_EMPTY,
  /* Executable was already been parsed */
  XTEXT_E_ALREADY_PARSED,
#if defined(__unix__)
  /* A recent call for function fstat has returned a fail value */
  XTEXT_E_FSTAT_FAILED,
  /* A recent call for mmap has failed */
  XTEXT_E_MMAP_FAILED,
  /* A recent call for munmap has failed */
  XTEXT_E_MUNMAP_FAILED,
#endif
  /* A recent try to read a chunk of memory was failed */
  XTEXT_E_CANT_READ,
  /* An invalid file has been passed */
  XTEXT_E_INVALID_FILE,
  /* Object isn't an ELF format */
  XTEXT_E_NOT_A_ELF,
} xtext_error_e;

/* CPU endianness of binary file */
typedef enum
{
  CPUE_END_UNKN = 0,
  CPUE_END_LITTLE,
  CPUE_END_BIG
} cpu_endian_e;

typedef enum
{
  // Unknown executable file
  XTEXT_TYPE_UNKN = 0,
  // Portable executable
  XTEXT_PE_FORMAT,
  // Executable and Linkable Format
  XTEXT_ELF_FORMAT
} xtext_type_t;

typedef struct
{
  /* Binary file pathname */
  char *pathname;
#if defined(__unix__)
  native_fd_t object_nfd;
#if _POSIX_C_SOURCE >= 200809L
  native_fd_t dir_fdesc;
#endif
  ioflags_t fd_flags;
#endif
} xtext_io_t;

typedef struct
{
#if defined(__unix__)
  /* We're mapping the file in memory */
  union
  {
    uint8_t *map_start;
    /* Automatically setted when map_start gains a valid pointer */
    bool is_map_alloc;
  };
  uintptr_t map_end;
  size_t map_size;
#endif
} xtext_map_t;

typedef enum
{
  CLASS_UNKNOWN = 0,
  CLASS_32_BITS,
  CLASS_64_BITS
} class_bits_e;

typedef struct
{
  size_t binary_size;
} xtext_info_t;

typedef struct
{

} object_symb_t;

typedef struct
{
  xtext_io_t binary_file;
  int32_t internal_error;
  /* An internal error status value (allow the getting method) */
  xtext_error_e error_instatus;
  /* Executable type (used for parser another structures) */
  xtext_map_t binary_map;
  xtext_info_t binary_info;
} xtext_ctx_t;

#endif

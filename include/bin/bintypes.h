#ifndef BIN_TYPES_H
#define BIN_TYPES_H

#include <stdbool.h>

#include <stddef.h>
#include <stdint.h>

/* File descriptor type, specifiers the FD integral value */
typedef int32_t FD_t;
/* Flags type used in IO operations */
typedef int32_t Flags_t;

typedef enum
{
	BIN_E_OK = 0,
	/* Can't open the file, maybe the file not exist */
	BIN_E_OPEN_FILE,
	/* Can't close the file, maybe has been deleted */
	BIN_E_CLOSE_FILE,
	/* A recent allocation using the malloc function has been failed */
	BIN_E_MALLOCATION_ERROR,
	/* The supposed binary file is empty */
	BIN_E_IS_EMPTY,
	/* The executable was already been parsed */
	BIN_E_ALREADY_PARSED __attribute__((unused)),
#if defined(__unix__)
	/* A recent call for function fstat has returned a fail value */
	BIN_E_FSTAT_FAILED,
	/* A recent call for mmap has failed */
	BIN_E_MMAP_FAILED,
	/* A recent call for munmap has failed */
	BIN_E_MUNMAP_FAILED,
#endif
	/* A recent try to read a chunk of memory was failed */
	BIN_E_CANT_READ __attribute__((unused)),
	/* An invalid file has been passed */
	BIN_E_INVALID_FILE,
	/* Object isn't an ELF format */
	BIN_E_NOT_A_ELF,

	BIN_E_FINAL_NULL_VALUE
} BinError_t;

typedef enum
{
	// Unknown executable file
	BT_UNKNOWN __attribute__((unused)) = 0,
	// Portable executable
	BT_PE_FILE __attribute__((unused)),
	// Executable and Linkable Format
	BT_ELF_FILE,
	BT_FINAL_NULL_VALUE
} BinType_t;

typedef struct
{
	/* Binary file pathname */
	char* 		pathname;
	#if defined(__unix__)
	FD_t 		objectFD;
	#if _POSIX_C_SOURCE >= 200809L
	FD_t 		dirFd;
	#endif
	Flags_t 	fdFlags;
	#endif
} BinIO_t;

typedef struct
{
	#if defined(__unix__)
	/* We're mapping the file in memory */
	uint8_t*	mapStart;
	uintptr_t 	mapEnd;
	size_t 		mapSize;
	#endif
} BinMap_t;

typedef struct
{
	size_t 		binaryFileSize;
} BinInfo_t;

typedef struct
{
	BinIO_t 	binaryFile;
	int32_t 	internalError;
	/* An internal error status value (allow the getting method) */
	BinError_t 	errorStatus;
	BinInfo_t	binaryInfo;
	/* Executable type (used for parser another structures) */
	BinType_t 	binaryType;
	BinMap_t	binaryMap;
} BinCtx_t;

#endif

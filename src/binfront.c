
#include <stdlib.h>

#if defined(__unix__)
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#endif

#include <assert.h>
#include <string.h>

#include "bin/elftypes.h"
#include "bin/binary.h"
#include "object.h"

UNUSED const char* LIBRARY_NAME	= "libbin";
UNUSED const char* LIBRARY_VERSION = "0.0.4";

#define BIN_HAND_IS_READABLE(ctx, retValue)\
	if (bin_obj_is_loaded(ctx) == false) {\
		(ctx)->errorStatus = BIN_E_CANT_READ;\
		return retValue;\
	}\
	(void)ctx

UNUSED const char* bin_obj_get_filename(BinCtx_t *binCtx) {
	assert(binCtx);
	BIN_HAND_IS_READABLE(binCtx, NULL);
	const BinIO_t *bio = &binCtx->binaryFile;
	return bio->pathname;
}

UNUSED const BinError_e bin_error_get_last(const BinCtx_t *binCtx) {
	return binCtx->errorStatus;
}

UNUSED size_t bin_obj_get_size(BinCtx_t *binCtx) {
	BIN_HAND_IS_READABLE(binCtx, 0);
	const BinInfo_t *binInfo = &binCtx->binaryInfo;
	return binInfo->binarySize;
}

UNUSED size_t bin_obj_memory_size(BinCtx_t *binCtx) {
	assert(binCtx);
	BIN_HAND_IS_READABLE(binCtx, 0);
	const BinMap_t *binMap = &binCtx->binaryMap;
	return binMap->mapSize;
}

/* Checks binary type and returns it */
static BinType_t CheckMagic(const unsigned char magicHeader[4]) {
	if (memcmp(magicHeader, ELFSignature, sizeof(ELFSignature)) == 0) {
		/* Binary is a ELF file */
		return BT_ELF_FILE;
	}
	return BT_UNKNOWN;
}

/* Returns object type current loaded */
UNUSED BinType_t bin_obj_get_type(BinCtx_t *binCtx) {
	BIN_HAND_IS_READABLE(binCtx, BT_UNKNOWN);
	const BinMap_t *binMap = &binCtx->binaryMap;
	uint8_t *magicHeader = BIN_MAKE_PTR(0, binMap->mapStart);
	return CheckMagic(magicHeader);
}

const char* const BinaryTypesStr[];

UNUSED const char* bin_obj_type_to_str(BinType_t binType) {
	if (binType > BT_ELF_FILE) {
		binType = BT_UNKNOWN;
	}
	return BinaryTypesStr[binType];
}

const char* const BinaryTypesStr[] = {
	// BT_UNKNOWN
	"unknown (not recognized, a.k.a UNK)",
	// BT_PE_FILE
	"portable executable (PE)",
	// BT_ELF_FILE
	"executable and linkable format (ELF)",
	NULL
};

UNUSED bool bin_load_file(const char *pathname, BinCtx_t *binCtx) {
	#define SYNC_IO_OPERATIONS 1
	bool canRead = false;
	#if defined(__unix__)
	canRead = access(pathname, F_OK) == 0;
	#endif
	if (canRead == false) {
	#if defined(__unix__)
		binCtx->internalError = errno;
	#endif
		binCtx->errorStatus = BIN_E_OPEN_FILE;
		return false;
	}
	/* If there's any file or memory allocated, clear everything now
	 * before continue!
	*/
	memset(binCtx, 0, sizeof(BinCtx_t));
	BinIO_t *binIO = &binCtx->binaryFile;
	#if defined(__unix__)
	binIO->fdFlags =
	#if SYNC_IO_OPERATIONS
		O_DSYNC |
	#endif
		/* O_RDWR | O_WRONLY | */ 
		O_RDONLY;
	#if _POSIX_C_SOURCE >= 200809L
	/* Opening current directory stored by process */
	FD_t openDir = binIO->dirFd = open(".", O_DIRECTORY | /* O_PATH */ O_RDONLY);

	if (openDir == -1) {
		binCtx->internalError = errno;
		binCtx->errorStatus = BIN_E_OPEN_FILE;
		return false;
	}

	const FD_t openFD = binIO->objectFD = openat(binIO->dirFd, pathname, binIO->fdFlags);
	close(binIO->dirFd);
	binIO->dirFd = -1;
	#else
	#error ""
	#endif
	if (openFD == -1) {
		binCtx->internalError = errno;
		binCtx->errorStatus = BIN_E_OPEN_FILE;
		return false;
	}
	#endif
	/* At this moment the file has been opened with success */
	binIO->pathname = strdup(pathname);
	if (binIO->pathname == NULL) {
		binCtx->errorStatus = BIN_E_MALLOCATION_ERROR;
		return false;
	}
	binCtx->errorStatus = BIN_E_OK;
	return true;
}

UNUSED bool bin_unload_file(BinCtx_t *bin) {
	assert(bin != NULL);
	#if defined(__unix__)
	errno = 0;
	BinIO_t *bio = &bin->binaryFile;
	FD_t binFD = bio->objectFD;

	if (bio->pathname != NULL) {
		free(bio->pathname);
		bio->pathname = NULL;
	}
	if (binFD != -1) {
		close(binFD);
		/* Ensuring an invalid state after the file was closed */
		bio->objectFD = binFD = -1;
	}
	if (errno != 0) {
		bin->errorStatus = BIN_E_CLOSE_FILE;
		return false;
	}
	#endif
	bin->errorStatus = BIN_E_OK;
	return true;
}

static bool UnmapFileMemory(BinCtx_t *binCtx) {
	void *mapStart;
	size_t mapSize;
	BinMap_t *map = &binCtx->binaryMap;

	mapStart = map->mapStart;
	mapSize = map->mapSize;
	assert(mapStart != NULL);
	assert(map->mapEnd != 0);
	assert(mapSize != 0);

	if (munmap(mapStart, mapSize) != 0) {
		binCtx->internalError = errno;
		binCtx->errorStatus = BIN_E_MUNMAP_FAILED;
	}
	map->mapStart = NULL;
	map->mapEnd = map->mapSize = 0;

	return true;
}

/* Returns the number of pages allocated (MEMORY / 1024) */
static bool MapFileMemory(BinCtx_t *bin) {
	size_t mapSize;
	uint8_t *mapStart;
	uintptr_t mapEnd;

	BinMap_t *map = &bin->binaryMap;
	BinIO_t *bio = &bin->binaryFile;
	BinInfo_t *binInfo = &bin->binaryInfo;

	size_t binFileSize = binInfo->binarySize;

	/* Or something like: 1,048,576 * x */
	#define MEBIBYTE(x)((x) * 1024 * 1024) // 1048576
	assert(binFileSize < MEBIBYTE(124));
	/* Mapping the file in memory */
	#if defined(__unix__)
	const int32_t mapProt = PROT_READ | PROT_WRITE;
	const int32_t mapFlags = /* MAP_SHARED */ MAP_PRIVATE;
	mapSize = binFileSize;
	mapStart = mmap(NULL, mapSize, mapProt, mapFlags, bio->objectFD, 0);

	if (mapStart == MAP_FAILED) {
		bin->errorStatus = BIN_E_MMAP_FAILED;
		return false;
	}

	mapEnd = (uintptr_t)mapStart + mapSize;
	map->mapStart = mapStart;
	map->mapEnd = mapEnd;
	map->mapSize = mapSize;
	/* Advise kernel about allocation purposes */
	madvise(mapStart, mapSize, MADV_SEQUENTIAL);
	/* Closing the file descriptor (is not more useful) */
	close(bio->objectFD);
	bio->objectFD = -1;
	#endif
	return true;
}

UNUSED bool bin_parser(BinCtx_t *binCtx) {
	BinIO_t *bio = &binCtx->binaryFile;
	BinInfo_t *binInfo = &binCtx->binaryInfo;

	if (binCtx->errorStatus != BIN_E_OK) {
		return false;
	}

	if (bin_obj_is_loaded(binCtx) == true) {
		binCtx->errorStatus = BIN_E_ALREADY_PARSED;
		return false;
	}
	#if defined(__unix__)
	const FD_t fd = bio->objectFD;
	if (fd == -1) {
		binCtx->errorStatus = BIN_E_OPEN_FILE;
		return false;
	}
	errno = 0;
	struct stat fdStat;
	const int32_t statRet = fstat(fd, &fdStat);
	if (statRet == -1) {
		binCtx->errorStatus = BIN_E_FSTAT_FAILED;
		return false;
	}
	const size_t binSize = fdStat.st_size;

	if (binSize == 0) {
		if (errno == 0) {
			binCtx->errorStatus = BIN_E_IS_EMPTY;
			return false;
		}
		else {
			binCtx->errorStatus = BIN_E_FSTAT_FAILED;
			return false;
		}
	}
	binInfo->binarySize = binSize;

	#endif
	/* Must be bigger than 0 */
	if (MapFileMemory(binCtx) == false) {
		if (binCtx->errorStatus != BIN_E_OK) {
			return false;
		}
	}
	binCtx->errorStatus = BIN_E_OK;
	return true;
}

static const char* const ErrorsStr[];

UNUSED const char* bin_error_to_str(BinError_e errorValue) {
	if (errorValue > BIN_E_NOT_A_ELF) {
		errorValue = BIN_E_NOT_A_ELF;
	}
	return ErrorsStr[errorValue];
}

static const char* const ErrorsStr[] = {
	// BIN_E_OK
	"everything is ok",
	// BIN_E_OPEN_FILE
	"can't open the file, maybe the file not exist",
	// BIN_E_CLOSE_FILE
	"can't close the file, maybe has been deleted",
	// BIN_E_MALLOCATION_ERROR
	"a recently allocation using the malloc function has been failed",
	// BIN_E_IS_EMPTY
	"the supposed binary file is empty",
	// BIN_E_ALREADY_PARSED
	"the executable has been already parsed",
	#if defined(__unix__)
	// BIN_E_FSTAT_FAILED
	"a recent call for function fstat has returned a fail value",
	// BIN_E_MMAP_FAILED
	"a recent call for mmap has failed",
	// BIN_E_MUNMAP_FAILED
	"a recent call for munmap has failed",
	#endif
	// BIN_E_CANT_READ
	"a recently try to read a chunk of memory has been failed",
	// BIN_E_INVALID_FILE
	"an invalid file name was been passed",
	// BIN_E_NOT_A_ELF
	"object file isn't an ELF",
	NULL
};

static const char* CPUEndiannessStr[] = {
	"unknown CPU endianness",
	"2's complement, little endian",
	"2's complement, big endian",
	NULL
};

UNUSED const char* bin_cpu_endian_to_str(CPU_Endian_e cpuEndian) {
	if (cpuEndian > CPUE_BIG) {
		cpuEndian = CPUE_UNKNOWN;
	}
	return CPUEndiannessStr[cpuEndian];
}

/* Translate a ELFEndian_e into a CPU_Endian_e */
static CPU_Endian_e ELFCPUEndian(ELFEndian_e elfEndian) {
	switch (elfEndian) {
		default:
		case ELF_DATA_NONE: return CPUE_UNKNOWN;
		case ELF_DATA_2LSB: return CPUE_LITTLE;
		case ELF_DATA_2MSB: return CPUE_BIG;
	}
}

/* Returns object CPU endianness */
UNUSED CPU_Endian_e bin_obj_get_endian(BinCtx_t *binCtx) {
	assert(binCtx);
	CPU_Endian_e cpuEndianE = CPUE_LITTLE;
	const BinMap_t *binMap = &binCtx->binaryMap;
	const uint8_t *elfIdent = (const uint8_t*)BIN_MAKE_PTR(ELF_IDENT_OFF, binMap->mapStart);
	switch (bin_obj_get_type(binCtx)) {
	case BT_UNKNOWN:
	case BT_ELF_FILE:
		cpuEndianE = ELFCPUEndian((ELFEndian_e) elfIdent[5]); break;
	case BT_PE_FILE: break;
	}

	return cpuEndianE;
}

static const char* BinaryClassStr[] = {
	"unknown object class",
	"32 bits",
	"64 bits"
};

UNUSED const char* bin_class_to_str(ClassBits_e classBits) {
	if (classBits > CLASS_64_BITS) {
		classBits = CLASS_UNKNOWN;
	}
	return BinaryClassStr[classBits];
}

static ClassBits_e ELFClass(ELFClass_e elfClass) {
	switch (elfClass) {
		default:
		case ELF_CLASS_32: return CLASS_32_BITS;
		case ELF_CLASS_64: return CLASS_64_BITS;
	}
}

UNUSED ClassBits_e bin_obj_get_class(BinCtx_t *binCtx) {
	assert(binCtx);
	BIN_HAND_IS_READABLE(binCtx, false);
	ClassBits_e classBits = CLASS_32_BITS;
	const BinMap_t *binMap = &binCtx->binaryMap;
	const uint8_t *elfIdent = (const uint8_t*)BIN_MAKE_PTR(ELF_IDENT_OFF, binMap->mapStart);

	switch(bin_obj_get_type(binCtx)) {
	case BT_ELF_FILE:
		classBits = ELFClass((ELFClass_e) elfIdent[ELF_IDENT_CLASS]);
	case BT_PE_FILE:
	case BT_UNKNOWN: break;
	}
	return classBits;
}

UNUSED bool bin_obj_class_is_32b(BinCtx_t *binCtx) {
	assert(binCtx);
	return bin_obj_get_class(binCtx) == CLASS_32_BITS;
}

UNUSED bool bin_obj_class_is_64b(BinCtx_t *binCtx) {
	assert(binCtx);
	return bin_obj_get_class(binCtx) == CLASS_64_BITS;
}

UNUSED bool bin_obj_is_loaded(const BinCtx_t *binCtx) {
	assert(binCtx);
	const BinMap_t *binMap = &binCtx->binaryMap;
	return binMap->mapStart != NULL && binMap->mapSize > 0;
}

/* Sanitizer check if binary is an ELF format */
UNUSED bool bin_obj_is_ELF(BinCtx_t *binCtx) {
	assert(binCtx);
	BIN_HAND_IS_READABLE(binCtx, false);

	const BinMap_t *binMap = &binCtx->binaryMap;
	const uint8_t *elfIdent = (const uint8_t*)BIN_MAKE_PTR(ELF_IDENT_OFF, binMap->mapStart);
	uint8_t headerMagic[4];
	memcpy(headerMagic, elfIdent, sizeof(headerMagic));
	return CheckMagic(headerMagic) == BT_ELF_FILE;
}

UNUSED bool bin_finish(BinCtx_t *bin) {
	if (bin_unload_file(bin) != BIN_E_OK) {
		return false;
	}
	/* Unmapping memory region pages allocated for binary */
	UnmapFileMemory(bin);
	if (bin->errorStatus == BIN_E_MUNMAP_FAILED) {
		return false;
	}
	/* Must be BIN_E_OK */
	const BinError_e errorStatus = bin->errorStatus;
	memset(bin, 0, sizeof(BinCtx_t));
	return errorStatus;
}


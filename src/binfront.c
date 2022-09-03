
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

__attribute__((unused)) const char* LIBRARY_NAME 	= "libbin";
__attribute__((unused)) const char* LIBRARY_VERSION = "0.0.4";

#include "bin/elftypes.h"
#include "bin/binary.h"

__attribute__((unused)) const char* BinGetFilename(const BinCtx_t *bin)
{
	const BinIO_t *bio = &bin->binaryFile;
	return bio->pathname;
}

__attribute__((unused)) const BinError_t BinGetLastError(const BinCtx_t *bin)
{
	return bin->errorStatus;
}

__attribute__((unused)) size_t BinGetObjectSize(const BinCtx_t *bin)
{
	const BinInfo_t *info = &bin->binaryInfo;
	return info->binaryFileSize;
}

/* Returns object type current loaded */
__attribute__((unused)) BinType_t BinObjectGetType(const BinCtx_t *bin)
{
	return bin->binaryType;
}

const char* const strBinaryTypesList[];

__attribute__((unused)) const char* BinObjectTypeToStr(BinType_t bin_type)
{
	if (bin_type >= BT_FINAL_NULL_VALUE)
		return strBinaryTypesList[BT_FINAL_NULL_VALUE];
	
	return strBinaryTypesList[bin_type];
}

const char* const strBinaryTypesList[] =
{
	// BT_UNKNOWN
	"unknown (not recognized, a.k.a UNK)",
	// BT_PE_FILE
	"portable executable (PE)",
	// BT_ELF_FILE
	"executable and linkable format (ELF)",
	// BT_FINAL_NULL_VALUE
	"(null)",
	NULL
};

__attribute__((unused)) bool BinLoadFile(const char *pathname, BinCtx_t *bin)
{
	#define SYNC_IO_OPERATIONS 1
	bool canRead = false;
	#if defined(__unix__)
	canRead = access(pathname, F_OK) == 0;
	#endif
	if (canRead == false)
	{
	#if defined(__unix__)
		bin->internalError = errno;
	#endif
		bin->errorStatus = BIN_E_OPEN_FILE;
		return false;
	}
	/* If there's any file or memory allocated, clear everything now
	 * before continue!
	*/
	memset(bin, 0, sizeof(BinCtx_t));

	BinIO_t *binIO = &bin->binaryFile;

	#if defined(__unix__)
	binIO->fdFlags =
	#if SYNC_IO_OPERATIONS
		O_DSYNC |
	#endif
		/* O_RDWR | O_WRONLY | */ 
		O_RDONLY;
	#if _POSIX_C_SOURCE >= 200809L
	/* Opening current directory stored by process */
	FD_t open_dir = binIO->dirFd = open(".", O_DIRECTORY | /* O_PATH */ O_RDONLY);

	if (open_dir == -1)
	{
		bin->internalError = errno;
		bin->errorStatus = BIN_E_OPEN_FILE;
		return false;
	}

	const FD_t open_fd = binIO->objectFD = openat(binIO->dirFd, pathname, binIO->fdFlags);
	close(binIO->dirFd);
	binIO->dirFd = -1;
	#else
	#error ""
	#endif
	if (open_fd == -1)
	{
		bin->internalError = errno;
		return bin->errorStatus = BIN_E_OPEN_FILE;
	}
	#endif
	/* At this moment the file has been opened with success */
	binIO->pathname = strdup(pathname);
	if (binIO->pathname == NULL)
	{
		bin->errorStatus = BIN_E_MALLOCATION_ERROR;
		return false;
	}
	bin->errorStatus = BIN_E_OK;
	return false;
}

__attribute__((unused)) bool BinUnloadFile(BinCtx_t *bin)
{
	assert(bin != NULL);
	#if defined(__unix__)
	errno = 0;

	BinIO_t *bio = &bin->binaryFile;
	FD_t binFD = bio->objectFD;

	if (bio->pathname != NULL)
	{
		free(bio->pathname);
		bio->pathname = NULL;
	}
	if (binFD != -1)
	{
		close(binFD);
		/* Ensuring an invalid state after the file was closed */
		bio->objectFD = binFD = -1;
	}
	if (errno != 0)
	{
		bin->errorStatus = BIN_E_CLOSE_FILE;
		return false;
	}
	#endif

	bin->errorStatus = BIN_E_OK;

	return true;
}


static bool UnmapFileMemory(BinCtx_t *bin)
{    
	void *mapStart;
	size_t mapSize;
	BinMap_t *map = &bin->binaryMap;

	mapStart = map->mapStart;
	mapSize = map->mapSize;
	assert(mapStart != NULL);
	assert(map->mapEnd != 0);
	assert(mapSize != 0);

	if (munmap(mapStart, mapSize) != 0)
	{
		bin->internalError = errno;
		bin->errorStatus = BIN_E_MUNMAP_FAILED;
	}
	map->mapStart = NULL;
	map->mapEnd = map->mapSize = 0;

	return true;
}

/* Returns the number of pages allocated (MEMORY / 1024) */
static bool MapFileMemory(BinCtx_t *bin)
{
	size_t binFileSize;
	size_t mapSize;
	uint8_t *mapStart;
	uintptr_t mapEnd;

	BinMap_t *map = &bin->binaryMap;
	BinIO_t *bio = &bin->binaryFile;
	BinInfo_t *info = &bin->binaryInfo;

	/* Or something like: 1,048,576 * x */
	#define MEBIBYTE(x) ((x) * 1024 * 1024) // 1048576
	binFileSize = info->binaryFileSize;
	assert(binFileSize < MEBIBYTE(124));
	/* Mapping the file in memory */
	#if defined(__unix__)
	const int32_t mapProt = PROT_READ | PROT_WRITE;
	const int32_t mapFlags = /* MAP_SHARED */ MAP_PRIVATE;
	mapSize = binFileSize;
	mapStart = mmap(NULL, mapSize, mapProt, mapFlags, bio->objectFD, 0);

	if (mapStart == MAP_FAILED)
	{
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

/* Checks binary type and returns it */
static BinType_t CheckMagic(const unsigned char magicHeader[4])
{
	if (memcmp(magicHeader, elfSignature, sizeof(elfSignature)) == 0)
	{
		/* Binary is a ELF file */
		return BT_ELF_FILE;
	}
	return BT_UNKNOWN;
}

/*
#define COPY_MEM_OBJ(dest, size, offset, area)\
	memcpy(dest, (area) + (offset), size)
*/

__attribute__((unused)) bool BinParser(BinCtx_t *bin)
{
	BinIO_t *bio = &bin->binaryFile;
	BinInfo_t *info = &bin->binaryInfo;
	BinMap_t *map = &bin->binaryMap;

	if (info->binaryFileSize != 0)
	{
		bin->errorStatus = BIN_E_ALREADY_PARSED;
		return false;
	}
	if (bin->errorStatus != BIN_E_OK)
	{
		return false;
	}
	#if defined(__unix__)
	const FD_t fd = bio->objectFD;
	if (fd == -1)
	{
		bin->errorStatus = BIN_E_OPEN_FILE;
		return false;
	}
	errno = 0;
	struct stat fdStat;
	const int32_t statRet = fstat(fd, &fdStat);
	if (statRet == -1)
	{
		bin->errorStatus = BIN_E_FSTAT_FAILED;
		return false;
	}
	const size_t binSize = fdStat.st_size;

	if (binSize == 0)
	{
		if (errno == 0)
			return bin->errorStatus = BIN_E_IS_EMPTY;
		else
			return bin->errorStatus = BIN_E_FSTAT_FAILED;
	}
	#endif
	/* Must be bigger than 0 */
	info->binaryFileSize = binSize;
	if (MapFileMemory(bin) == false)
	{
		if (bin->errorStatus != BIN_E_OK)
		{
			return false;
		}
	}
	uint8_t *magicHeader;
	// uint8_t magicHeader[4];
	/* COPY_MEM_OBJ(magicHeader, sizeof(magicHeader), 0, map->mapStart); */
	magicHeader = map->mapStart;
	const BinType_t bin_type = CheckMagic(magicHeader);
	/* From this point, the object has been noted has a ELF file */
	if (bin_type == BT_UNKNOWN)
	{
		bin->errorStatus = BIN_E_INVALID_FILE;
		return false;
	}

	bin->binaryType = bin_type;
	bin->errorStatus = BIN_E_OK;

	return true;
}

static const char* const strErrorsList[];

__attribute__((unused)) const char* BinErrorToStr(const BinError_t error_value)
{
	if (error_value >= BIN_E_FINAL_NULL_VALUE)
	{
		return strErrorsList[BIN_E_FINAL_NULL_VALUE];
	}
	return strErrorsList[error_value];
}

static const char* const strErrorsList[] = {
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
	// BIN_E_FINAL_NULL_VALUE
	/* Invalid error values */
	"(null)",
	NULL
};

__attribute__((unused)) bool BinFinish(BinCtx_t *bin)
{
	if (BinUnloadFile(bin) != BIN_E_OK)
	{
		return false;
	}
	/* Unmapping memory region pages allocated for binary */
	UnmapFileMemory(bin);
	if (bin->errorStatus == BIN_E_MUNMAP_FAILED)
	{
		return false;
	}
	/* Must be BIN_E_OK */
	const BinError_t errorStatus = bin->errorStatus;
	memset(bin, 0, sizeof(BinCtx_t));
	return errorStatus;
}


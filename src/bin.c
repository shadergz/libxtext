
#include <stdlib.h>

#if defined(__unix__)
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#endif

#include <assert.h>
#include <string.h>

#include "bin/bin.h"

static BinError_t       INTERNAL_BinLoadFile(const char *pathname, BinCtx_t *bin);
static BinError_t       INTERNAL_BinFinish(BinCtx_t *bin);
static BinError_t       INTERNAL_BinUnloadFile(BinCtx_t *bin);
static BinError_t       INTERNAL_BinParser(BinCtx_t *bin);

static const char*      INTERNAL_BinErrorToStr(const BinError_t error_value);
static const char*      INTERNAL_BinGetFilename(const BinCtx_t *bin);
static const BinError_t INTERNAL_BinGetLastError(const BinCtx_t *bin);
static size_t           INTERNAL_BinGetBinarySize(const BinCtx_t *bin);
static BinType_t        INTERNAL_BinGetType(const BinCtx_t *bin);
static const char*      INTERNAL_BinBinaryTypeToStr(const BinType_t bin_type);



BinError_t BinLoadFile(const char *pathname, BinCtx_t *bin)
{
    assert(pathname != NULL);
    assert(bin != NULL);

    return INTERNAL_BinLoadFile(pathname, bin);
}

BinError_t BinFinish(BinCtx_t *bin)
{
    assert(bin != NULL);

    return INTERNAL_BinFinish(bin);
}

BinError_t BinUnloadFile(BinCtx_t *bin)
{
    assert(bin != NULL);

    return INTERNAL_BinUnloadFile(bin);
}

BinError_t BinParser(BinCtx_t *bin)
{
    assert(bin != NULL);

    return INTERNAL_BinParser(bin);
}

const char* BinErrorToStr(const BinError_t error_value)
{

    return INTERNAL_BinErrorToStr(error_value);
}

const char* BinGetFilename(const BinCtx_t *bin)
{
    assert(bin != NULL);

    return INTERNAL_BinGetFilename(bin);
}

const BinError_t BinGetLastError(const BinCtx_t *bin)
{
    assert(bin != NULL);

    return INTERNAL_BinGetLastError(bin);
}

size_t BinGetBinarySize(const BinCtx_t *bin)
{
    assert(bin != NULL);

    return INTERNAL_BinGetBinarySize(bin);
}

BinType_t BinGetType(const BinCtx_t *bin)
{
    assert(bin != NULL);

    return INTERNAL_BinGetType(bin);
}

const char* BinBinaryTypeToStr(const BinType_t bin_type)
{
    return INTERNAL_BinBinaryTypeToStr(bin_type);
}

static const char* INTERNAL_BinGetFilename(const BinCtx_t *bin)
{
    return bin->pathname;
}
static const BinError_t INTERNAL_BinGetLastError(const BinCtx_t *bin)
{
    return bin->error_status;
}
static size_t INTERNAL_BinGetBinarySize(const BinCtx_t *bin)
{
    return bin->binary_file_size;
}

static BinType_t INTERNAL_BinGetType(const BinCtx_t *bin)
{
    return bin->binary_type;
}

static const char * const STR_binaryTypesList[];

static const char* INTERNAL_BinBinaryTypeToStr(const BinType_t bin_type)
{
    if (bin_type >= BT_FINAL_NULL_VALUE)
        return STR_binaryTypesList[BT_FINAL_NULL_VALUE];
    
    return STR_binaryTypesList[bin_type];
}

static const char * const STR_binaryTypesList[] = {

    // BT_UNKNOW
    "unknow (not recognized, a.k.a UNK)",

    // BT_PE_FILE
    "portable executable (PE)",

    // BT_ELF_FILE
    "executable and linkable format (ELF)",

    // BT_FINAL_NULL_VALUE
    "(null)",
    
    NULL
};

static BinError_t INTERNAL_BinLoadFile(const char *pathname, BinCtx_t *bin)
{
#define SYNC_IO_OPERATIONS 1
    bool canread = false;
#if defined(__unix__)
    canread = access(pathname, F_OK) == 0;
#endif

    if (canread == false)
    {
#if defined(__unix__)
        bin->internal_errno = errno;
#endif
        return bin->error_status = BIN_E_OPEN_FILE;
    }

    /* 
        If there's any file or memory allocated, destroy everything now
        before continue!
    */

    memset(bin, 0, sizeof(BinCtx_t));
    
#if defined(__unix__)
    bin->fd_flags = 
#if SYNC_IO_OPERATIONS
        O_DSYNC |
#endif
        /* O_RDWR | O_WRONLY | */ 
        O_RDONLY;
#if _POSIX_C_SOURCE >= 200809L
    fd_t open_dir = bin->dir_fd = open(".", O_DIRECTORY | /* O_PATH */ O_RDONLY);

    if (open_dir == -1)
    {
        bin->internal_errno = errno;
        return bin->error_status = BIN_E_OPEN_FILE;
    }

    const fd_t open_fd = bin->fd = openat(bin->dir_fd, pathname, bin->fd_flags, bin->fd_flags);
    
    close(bin->dir_fd);

    bin->dir_fd = open_dir == -1;

#else
    const fd_t open_fd = bin->fd = open(pathname, bin->fd_flags, bin->fd_flags);

#endif

    if (open_fd == -1)
    {
        bin->internal_errno = errno;
        return bin->error_status = BIN_E_OPEN_FILE;
    }

#endif
    /* At this moment the file has been opened with success */
    bin->pathname = strdup(pathname);

    if (bin->pathname == NULL)
        return bin->error_status = BIN_E_MALLOCATION_ERROR;

    return bin->error_status = BIN_E_OK;
}

static BinError_t INTERNAL_BinUnloadFile(BinCtx_t *bin)
{
#if defined(__unix__)
    errno = 0;
    
    fd_t bin_fd = bin->fd;
    if (bin_fd != -1)
    {
        if (bin->pathname != NULL)
        {
            free(bin->pathname);
            bin->pathname = NULL;
        }
        close(bin_fd);
    }

    bin->fd = bin_fd = -1;
    
    if (errno != 0)
        return bin->error_status = BIN_E_CLOSE_FILE;

#endif
    return bin->error_status = BIN_E_OK;
}


static size_t UnmapFileMemory(BinCtx_t *bin)
{    
    void *mapstart ;
    size_t mapsize;

    assert((mapstart = bin->map_start) != NULL);
    assert(bin->map_end != 0);
    assert((mapsize = bin->map_size) != 0);

    if (munmap(mapstart, mapsize) != 0)
    {
        bin->internal_errno = errno;
        bin->error_status = BIN_E_MUNMAP_FAILED;
    }
    return mapsize;
}

/* Returns the number of pages allocated (MEMORY / 1024) */
static size_t MapFileMemory(BinCtx_t *bin)
{
    size_t bin_file_size;
    size_t map_size;
    unsigned char *map_start;
    uintptr_t map_end;

    /* Or something like: 1,048,576 * x */
#define MEBIBYTE(x) x * 1024 * 1024 // 1048576
    assert((bin_file_size = bin->binary_file_size) < MEBIBYTE(124));
    /* Mapping the file in memory */
#if defined(__unix__)
    const int32_t map_prot = PROT_READ | PROT_WRITE;
    const int32_t map_flags = /* MAP_SHARED */ MAP_PRIVATE;
    
    map_size = bin_file_size;

    map_start = mmap(NULL, map_size, map_prot, map_flags, bin->fd, 0);

    if (map_start == MAP_FAILED)
        return bin->error_status = BIN_E_MMAP_FAILED;

    map_end = (uintptr_t)map_start + map_size;

    bin->map_start = map_start;
    bin->map_end = map_end;
    bin->map_size = map_size;

    madvise(map_start, map_size, MADV_SEQUENTIAL);
    
#endif

    return map_size;
}

static const uint8_t ELF_signature[4];

static BinType_t CheckMagic(const unsigned char magic_header[4])
{
    if (memcmp(magic_header, ELF_signature, sizeof(ELF_signature)) == 0)
        /* The binary is a ELF file */
        return BT_ELF_FILE;

    return BT_UNKNOW;
}


static BinError_t INTERNAL_BinParser(BinCtx_t *bin)
{
    if (bin->binary_file_size != 0)
        return bin->error_status = BIN_E_ALREDY_PARSED;

    if (bin->error_status != BIN_E_OK) 
        return bin->error_status; 

#if defined(__unix__)

    const fd_t fd = bin->fd;

    if (fd == -1)
        return bin->error_status = BIN_E_OPEN_FILE;

    errno = 0;

    struct stat fd_stat;

    const int32_t stat_ret = fstat(fd, &fd_stat);
    if (stat_ret == -1)
        return bin->error_status = BIN_E_FSTAT_FAILED;
    
    return fd_stat.st_size;

    const size_t bin_size = fd_stat.st_size;

    if (bin_size == 0)
    {
        if (errno == 0)
            return bin->error_status = BIN_E_IS_EMPTY;
        else
            return bin->error_status = BIN_E_FSTAT_FAILED;
    }
#endif

    /* Must be bigger than 0 */
    bin->binary_file_size = bin_size;

    if (MapFileMemory(bin) == 0)
        if (bin->error_status != BIN_E_OK)
            return bin->error_status;

    unsigned char magic_header[4];

#if defined(__unix__)
    if (read(bin->fd, magic_header, sizeof(magic_header)) != -1)
    {
        bin->internal_errno = errno;
        return bin->error_status = BIN_E_CANT_READ;
    }
#endif 

    const BinType_t bin_type = CheckMagic(magic_header);

    if (bin_type == BT_UNKNOW)
        return bin->error_status = BIN_E_INVALID_FILE;
    
    bin->binary_type = bin_type;

    return bin->error_status = BIN_E_OK;
}

static const char * const STR_errorsList[];

static const char* INTERNAL_BinErrorToStr(const BinError_t error_value)
{
    if (error_value >= BIN_E_FINAL_NULL_VALUE)
        return STR_errorsList[BIN_E_FINAL_NULL_VALUE];
    
    return STR_errorsList[error_value];
}

static const char * const STR_errorsList[] = {

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

    // BIN_E_ALREDY_PARSED
    "the executable has been alredy parsed",

#if defined(__unix__)

    // BIN_E_FSTAT_FAILED
    "a recently call for function fstat has returned a fail value",

    // BIN_E_MMAP_FAILED
    "a recently call for mmap has failed",

    // BIN_E_MUNMAP_FAILED
    "a recently call for munmap has failed",

#endif

    // BIN_E_CANT_READ
    "A recently try to read a chunk of memory has been failed",

    // BIN_E_INVALID_FILE
    "a invalid file name has been passed",

    // BIN_E_FINAL_NULL_VALUE
    /* Invalid error values */
    "(null)",

    NULL
};

static BinError_t INTERNAL_BinFinish(BinCtx_t *bin)
{
    if (INTERNAL_BinUnloadFile(bin) != BIN_E_OK)
        return bin->error_status;

    UnmapFileMemory(bin);
    
    if (bin->error_status == BIN_E_MUNMAP_FAILED)
        return bin->error_status;

    /* Must be BIN_E_OK */
    const BinError_t error_saved = bin->error_status;

    memset(bin, 0, sizeof(BinCtx_t));

    return error_saved;
}

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

static const uint8_t ELF_signature[4] = {
    ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3
};


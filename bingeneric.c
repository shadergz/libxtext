#include <assert.h>
#include <string.h>
#include <malloc.h>

#if defined(__unix__)

#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <unistd.h>

#endif

#include "bin/bin.h"

static bin_err_e        bin_load_file_ext(const char *pathname, bin_ctx_t *bin);
static bin_err_e        bin_unload_file_ext(bin_ctx_t *bin);

static bin_err_e        bin_parser_ext(bin_ctx_t *bin);

static const char*      bin_get_filename_ext(const bin_ctx_t *bin);
static const bin_err_e  bin_get_last_error_ext(const bin_ctx_t *bin);
static size_t           bin_get_binary_size_ext(const bin_ctx_t *bin);

static const char*      bin_error_to_str_ext(const bin_err_e error_value);

static bin_err_e        bin_finish_ext(bin_ctx_t *bin);

bin_err_e bin_load_file(const char *pathname, bin_ctx_t *bin)
{
    assert(pathname != NULL);
    assert(bin != NULL);

    return bin_load_file_ext(pathname, bin);
}

bin_err_e bin_finish(bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_finish_ext(bin);
}

bin_err_e bin_unload_file(bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_finish(bin);
}

bin_err_e bin_parser(bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_parser_ext(bin);
}

const char* bin_error_to_str(const bin_err_e error_value)
{

    return bin_error_to_str_ext(error_value);
}

const char* bin_get_filename(const bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_get_filename_ext(bin);
}

const bin_err_e bin_get_last_error(const bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_get_last_error_ext(bin);
}

size_t bin_get_binary_size(const bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_get_binary_size_ext(bin);
}

static inline bool fs_can_open(const char *pathname)
{
#if defined(__unix__)
    return access(pathname, F_OK) == 0;
#endif
}

#if defined(__unix__)
static inline size_t fs_get_size(int fd)
{    
    struct stat fd_stat;

    const int stat_ret = fstat(fd, &fd_stat);
    if (stat_ret == -1)
        return 0;
    
    return fd_stat.st_size;
}
#endif

/* Returns the number of pages allocated (MEMORY / 1024) */
static size_t check_and_map(bin_ctx_t *bin)
{
    size_t bin_file_size;
    /* Or something like: 1,048,576 * x */
#define MEBIBYTE(x) x * 1024 * 1024 // 1048576
    if ((bin_file_size = bin->binary_file_size) > MEBIBYTE(124))
    {
        /* If the binary file is bigger than 124MiBs */
        bin->using_mapped = false;
        return 0;
    }

    /* Mapping the file in memory */

#if defined(__unix__)
    const int map_prot = PROT_READ | PROT_WRITE;
    const int map_flags = /* MAP_SHARED */ MAP_PRIVATE;
    const size_t map_size = bin_file_size;

    unsigned char *map_start = mmap(NULL, map_size, map_prot, map_flags, bin->fd, 0);

    if (map_start == MAP_FAILED)
        return bin->error_status = BIN_E_MMAP_FAILED;
    
	const uintptr_t map_end = (uintptr_t)map_start + map_size;

    bin->mapped_area.map_start = map_start;
    bin->mapped_area.map_end = map_end;
    bin->mapped_area.map_size = map_size;

    return map_size;
#endif
}

static size_t check_and_unmap(bin_ctx_t *bin)
{
    if (bin->using_mapped == false)
        return 0;
    
    void *mapstart ;
    size_t mapsize;

    assert((mapstart = bin->mapped_area.map_start) != NULL);
    assert(bin->mapped_area.map_end != 0);
    assert((mapsize = bin->mapped_area.map_size) != 0);

    if (munmap(mapstart, mapsize) != 0)
        bin->error_status = BIN_E_MUNMAP_FAILED;
    
    return mapsize;
}

static bin_err_e bin_parser_ext(bin_ctx_t *bin)
{
    if (bin->binary_file_size)
        return BIN_E_ALREDY_PARSED;

    if (bin->error_status != BIN_E_OK) 
        return bin->error_status;

    const fd_t fd = bin->fd;

    if (fd > 2)
        return bin->error_status = BIN_E_OPEN_FILE;

    errno = 0;

#if defined(__unix__)
    const size_t bin_size = fs_get_size(fd);

    if (bin_size == 0) {
        if (errno == 0)
            return BIN_E_IS_EMPTY;
        else
            return BIN_E_FSTAT_FAILED;
    }
#endif

    /* Must be bigger than 0 */
    bin->binary_file_size = bin_size;

    if (check_and_map(bin) == 0)
        if (bin->error_status != BIN_E_OK)
            return bin->error_status;

    return bin->error_status = BIN_E_OK;
}

static bin_err_e bin_load_file_ext(const char *pathname, bin_ctx_t *bin)
{
#define SYNC_IO_OPERATIONS 1

    if (fs_can_open(pathname) == false)
    {
#if defined(__unix)
        bin->internal_errno = errno;
#endif
        return bin->error_status = BIN_E_OPEN_FILE;
    }

    /* 
        If there's any file or memory allocated, destroy everything now
        before continue!
    */
    if (bin_finish_ext(bin) != BIN_E_OK)
        return bin->error_status;
    
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

static bin_err_e bin_unload_file_ext(bin_ctx_t *bin)
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

static bin_err_e bin_finish_ext(bin_ctx_t *bin)
{
    if (bin_unload_file_ext(bin) != BIN_E_OK)
        return bin->error_status;

    check_and_unmap(bin);
    
    if (bin->error_status == BIN_E_MUNMAP_FAILED)
        return bin->error_status;

    /* Must be BIN_E_OK */
    const bin_err_e error_saved = bin->error_status;

    memset(bin, 0, sizeof(bin_ctx_t));

    return error_saved;
}

static const char * const bin_e_str_list[];

static const char* bin_error_to_str_ext(const bin_err_e error_value)
{
    if (error_value >= BIN_E_FINAL_NULL_VALUE)
        return bin_e_str_list[BIN_E_FINAL_NULL_VALUE];
    
    return bin_e_str_list[error_value];
    
}

static const char * const bin_e_str_list[] = {

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
    /* Invalid error values */
    "(null)",
    NULL
};

static const char* bin_get_filename_ext(const bin_ctx_t *bin)
{
    return bin->pathname;
}
static const bin_err_e bin_get_last_error_ext(const bin_ctx_t *bin)
{
    return bin->error_status;
}
static size_t bin_get_binary_size_ext(const bin_ctx_t *bin)
{
    return bin->binary_file_size;
}


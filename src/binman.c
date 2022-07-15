#if defined(__unix__)

#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <unistd.h>

#endif

#include <malloc.h>
#include <string.h>

#include "binman.h"
#include "unload.h"
#include "fshand.h"

bin_err_e bin_load_file_ext(const char *pathname, bin_ctx_t *bin)
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

    memset(bin, 0, sizeof(bin_ctx_t));
    
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

bin_err_e bin_unload_file_ext(bin_ctx_t *bin)
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

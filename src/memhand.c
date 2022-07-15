#if defined(__unix__)

#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <unistd.h>

#endif

#include <assert.h>

#include "memhand.h"

size_t check_and_unmap(bin_ctx_t *bin)
{
    if (bin->using_mapped == false)
        return 0;
    
    void *mapstart ;
    size_t mapsize;

    assert((mapstart = bin->map_start) != NULL);
    assert(bin->map_end != 0);
    assert((mapsize = bin->map_size) != 0);

    if (munmap(mapstart, mapsize) != 0)
        bin->error_status = BIN_E_MUNMAP_FAILED;
    
    return mapsize;
}

/* Returns the number of pages allocated (MEMORY / 1024) */
size_t check_and_map(bin_ctx_t *bin)
{
    size_t bin_file_size;
    size_t map_size;
    unsigned char *map_start;
    uintptr_t map_end;

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

    bin->using_mapped = true;
#endif

    return map_size;
}

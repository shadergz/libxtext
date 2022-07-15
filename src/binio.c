#if defined(__unix__)

#include <errno.h>
#include <fcntl.h>

#include <unistd.h>

#endif

#include <string.h>

#include "binio.h"

size_t read_binary(void *dest, uintptr_t offset, size_t size, bin_ctx_t *bin)
{
#if defined(__unix__)
    if (bin->using_mapped)
    {
        if (((uintptr_t)bin->map_start + offset + size) > bin->map_end)
            return 0;
        /* Can't continue for reading, because offset + size is bigger than the map address */
        memcpy(dest, bin->map_start + offset, size);
        return size;
    }

    if (bin->fd > 2)
        lseek(bin->fd, offset, SEEK_SET);
    else
        return 0;

    return read(bin->fd, dest, size);
#endif
}

#if defined(__unix__)
#include <unistd.h>
#include <sys/stat.h>
#endif

#include "fshand.h"

bool fs_can_open(const char *pathname)
{
#if defined(__unix__)
    return access(pathname, F_OK) == 0;
#endif
}

#if defined(__unix__)
size_t fs_get_size(int fd)
{    
    struct stat fd_stat;

    const int stat_ret = fstat(fd, &fd_stat);
    if (stat_ret == -1)
        return 0;
    
    return fd_stat.st_size;
}
#endif
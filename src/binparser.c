#include <stdio.h>

#if defined(__unix__)
#include <errno.h>
#endif

#include "bin/bin.h"

#include "binio.h"
#include "fshand.h"
#include "identify.h"
#include "memhand.h"

bin_err_e bin_parser_ext(bin_ctx_t *bin)
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

    const size_t bin_size = fs_get_size(fd);

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

    if (check_and_map(bin) == 0)
        if (bin->error_status != BIN_E_OK)
            return bin->error_status;

    unsigned char magic_header[4];
    
    if (read_binary(magic_header, 0, sizeof(magic_header), bin) == 0)
        return bin->error_status = BIN_E_CANT_READ;

    const bin_type_e bin_type = identify_unknow_magic(magic_header);

    if (bin_type == BT_UNKNOW)
        return bin->error_status = BIN_E_INVALID_FILE;
    
    bin->binary_type = bin_type;

    return bin->error_status = BIN_E_OK;
}


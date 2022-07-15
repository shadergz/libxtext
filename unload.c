#include <string.h>

#include "unload.h"
#include "memhand.h"
#include "binman.h"

bin_err_e bin_finish_ext(bin_ctx_t *bin)
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

#ifndef LIBBIN_BINMAN_H
#define LIBBIN_BINMAN_H

#include "bin/bin.h"

__attribute__((visibility("hidden"))) extern bin_err_e bin_load_file_ext(
    const char *pathname, bin_ctx_t *bin
);
__attribute__((visibility("hidden"))) extern bin_err_e bin_unload_file_ext(
    bin_ctx_t *bin
);

#endif

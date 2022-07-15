#ifndef LIBBIN_UNLOAD_H
#define LIBBIN_UNLOAD_H

#include "bin/bin.h"

__attribute__((visibility("hidden"))) extern bin_err_e bin_finish_ext(
    bin_ctx_t *bin
);

#endif

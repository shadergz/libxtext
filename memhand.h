#ifndef LIBBIN_MEMHAND_H
#define LIBBIN_MEMHAND_H

#include "bin/bin.h"

__attribute__((visibility("hidden"))) extern size_t check_and_unmap(bin_ctx_t *bin);
__attribute__((visibility("hidden"))) extern size_t check_and_map(bin_ctx_t *bin);

#endif


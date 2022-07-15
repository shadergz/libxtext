#ifndef LIBBIN_BINIO_H
#define LIBBIN_BINIO_H

#include <stddef.h>

#include "bin/bin.h"

__attribute__((visibility("hidden"))) size_t read_binary(void *dest, uintptr_t offset, size_t size, bin_ctx_t *bin);

#endif


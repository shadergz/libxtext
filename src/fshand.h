#ifndef LIBBIN_FSHAND_H
#define LIBBIN_FSHAND_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

__attribute__((visibility("hidden"))) extern bool fs_can_open(const char *pathname);
__attribute__((visibility("hidden"))) extern size_t fs_get_size(int32_t fd);

#endif

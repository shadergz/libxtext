#ifndef LIBBIN_IDENTIFY_H
#define LIBBIN_IDENTIFY_H

#include "bin/bin.h"

__attribute__((visibility("hidden"))) extern bin_type_e identify_unknow_magic(
    const unsigned char magic_header[4]
);

#endif

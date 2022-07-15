#ifndef LIBBIN_ELFDATA_H
#define LIBBIN_ELFDATA_H

#include <stdint.h>

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

/* A array that's contain the above information */
__attribute__((visibility("hidden"))) extern const uint8_t ELF_signature[4];

#endif


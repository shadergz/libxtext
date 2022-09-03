#ifndef BIN_ELFTYPES_H
#define BIN_ELFTYPES_H

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#include <stdint.h>

extern const uint8_t elfSignature[4];

typedef struct
{

} ObjectSymbolCtx_t;

#endif

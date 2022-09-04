#ifndef BIN_OBJECT_H
#define BIN_OBJECT_H

#include "bin/elftypes.h"

#define ELF_IDENT_OFF offsetof(ELFHeaderCtx_t, elfIdent)

#define BIN_MAKE_PTR(relative, map)\
    (relative + map)

#endif

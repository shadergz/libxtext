#ifndef BIN_OBJECT_H
#define BIN_OBJECT_H

#include "bin/elftypes.h"

#define ELF_IDENT_OFF offsetof(ELFHeader64Ctx_t, elfIdent)
/* #define ELF_SYMHDR32_OFF offsetof(ELFHeaderCtx_t, elf32Shoff) */
#define ELF_SYMHDR64_OFF offsetof(ELFHeader64Ctx_t, elf64Shoff)

#define ELF_SECHDR64_SZ_OFF offsetof(ELFHeader64Ctx_t, elfSecHeaderSize)
#define ELF_SECHDR64_COUNT_OFF offsetof(ELFHeader64Ctx_t, elfSecHeaderCount)

#define BIN_MAKE_PTR(relative, map)\
    (relative + map)

#endif

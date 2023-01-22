#ifndef XTEXT_OBJECT_H
#define XTEXT_OBJECT_H

#include <xtext/elf_content.h>

#define ELF_IDENT_OFF offsetof (elf_header64_t, elfIdent)
/* #define ELF_SYMHDR32_OFF offsetof(elf_header_ctx_t, elf32Shoff) */
#define ELF_SYMHDR64_OFF offsetof (elf_header64_t, elf64Shoff)

#define ELF_SECHDR64_SZ_OFF offsetof (elf_header64_t, elfSecHeaderSize)
#define ELF_SECHDR64_COUNT_OFF offsetof (elf_header64_t, elfSecHeaderCount)

#define XTEXT_MAKE_PTR(relative, map) (relative + map)

#endif

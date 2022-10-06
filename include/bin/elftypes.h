#ifndef BIN_ELFTYPES_H
#define BIN_ELFTYPES_H

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#include <stdint.h>
#include <stdbool.h>

extern const uint8_t ELFSignature[4];

#define ELF_IDENT_SZ 16

typedef uint32_t ELF32Addr_t;
typedef uint32_t Elf32Off_t;

typedef uint32_t ELF64Addr_t;
typedef uint64_t Elf64Off_t;

typedef struct {
    uint8_t		    elfIdent[ELF_IDENT_SZ];
    uint16_t	    elfType;
    uint16_t	    elfMachine;
    uint32_t	    elfVersion;
    ELF32Addr_t     elf32Entry;
    Elf32Off_t      elf32Phoff;
    Elf32Off_t      elf32Shoff;

    uint32_t        elfFlags;
    uint16_t        elfHeaderSize;
    /* This member holds the size in bytes of one entry in the
     * file's program header table; all entries are the same size. */
    uint16_t        elfPHeaderTabSize;
    uint16_t        elfPHeaderTableSize;
    /* This member holds a sections header's size in bytes.  A
    section header is one entry in the section header table;
    all entries are the same size. */
    uint16_t        elfSecHeaderSize;
    uint16_t        elfSecHeaderCount;
    /*
     * This member holds the section header table index of the
     * entry associated with the section name string table.  If
     * the file has no section name string table, this member
     * holds the value SHN_UNDEF.
    */
    uint16_t        elfSecHeaderTabIndex;
} ELFHeader32Ctx_t;

typedef struct {
    uint8_t		    elfIdent[ELF_IDENT_SZ];
    uint16_t	    elfType;
    uint16_t	    elfMachine;
    uint32_t	    elfVersion;
    ELF64Addr_t     elf64Entry;
    Elf64Off_t      elf64Phoff;

    Elf64Off_t      elf64Shoff;

    uint32_t        elfFlags;
    uint16_t        elfHeaderSize;
    /* This member holds the size in bytes of one entry in the
     * file's program header table; all entries are the same size. */
    uint16_t        elfPHeaderTabSize;
    uint16_t        elfPHeaderTableSize;
    /* This member holds a sections header's size in bytes.  A
    section header is one entry in the section header table;
    all entries are the same size. */
    uint16_t        elfSecHeaderSize;
    uint16_t        elfSecHeaderCount;
    /*
     * This member holds the section header table index of the
     * entry associated with the section name string table.  If
     * the file has no section name string table, this member
     * holds the value SHN_UNDEF.
    */
    uint16_t        elfSecHeaderTabIndex;
} ELFHeader64Ctx_t;

typedef struct {
    union {
        ELFHeader32Ctx_t header32Ctx;
        ELFHeader64Ctx_t header64Ctx;
    };
} ELFHeaderCtx_t;

typedef enum {
    ELF_DATA_NONE = 0,
    ELF_DATA_2LSB,
    ELF_DATA_2MSB
} ELFEndian_e;

typedef enum {
    ELF_CLASS_32 = 1,
    ELF_CLASS_64
} ELFClass_e;

typedef struct {
    uint32_t    secName;
    uint32_t    secType;
    uint32_t    secFlags;
    ELF32Addr_t sec32Addr;
    Elf32Off_t  sec32Offset;

    uint32_t    secSize;
    uint32_t    secLink;
    uint32_t    secInfo;
    uint32_t    secAddrAlign;
    uint32_t    secEntSize;
} ELFSecHdr32_t;

typedef struct {
    uint32_t    secName;
    uint32_t    secType;
    uint32_t    secFlags;
    ELF64Addr_t sec64Addr;
    Elf32Off_t  sec32Offset;

    uint32_t    secSize;
    uint32_t    secLink;
    uint32_t    secInfo;
    uint32_t    secAddrAlign;
    uint32_t    secEntSize;
} ELFSecHdr64_t;

typedef struct {
    union {
        ELFSecHdr32_t header32;
        ELFSecHdr64_t header64;
    };
} ELFSecHdr_t;

#define ELF_IDENT_CLASS 4

#define SHT_SYM_TAB 2

#endif

#ifndef BIN_ELFTYPES_H
#define BIN_ELFTYPES_H

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#include <stdint.h>

extern const uint8_t ELFSignature[4];

#define EI_NIDENT 16

typedef uint32_t ELF32Addr_t;
typedef uint32_t Elf32Off_t;

typedef uint32_t ELF64Addr_t;
typedef uint64_t Elf64Off_t;

typedef struct {
	uint8_t		elfIdent[EI_NIDENT];
	uint16_t	elfType;
	uint16_t	elfMachine;
	uint32_t	elfVersion;
	union {
		ELF32Addr_t elf32Entry;
		ELF64Addr_t elf64Entry;
	};
	union {
		Elf32Off_t elf32Off;
		Elf64Off_t elf64Off;
	};
	union {
		Elf32Off_t elf32Phoff;
		Elf64Off_t elf64Phoff;
	};
	union {
		Elf32Off_t elf32Shoff;
		Elf64Off_t elf64Shoff;
	};

	uint32_t      elfFlags;
	uint16_t      elfHeaderSize;
	/* This member holds the size in bytes of one entry in the
	 * file's program header table; all entries are the same size. */
	uint16_t      elfPHeaderTabSize;
	uint16_t      elfPHeaderTableSize;
	/* This member holds a sections header's size in bytes.  A
	section header is one entry in the section header table;
	all entries are the same size. */
	uint16_t      elfSecHeaderSize;
	uint16_t      elfSecHeaderCount;
	/*
	 * This member holds the section header table index of the
	 * entry associated with the section name string table.  If
	 * the file has no section name string table, this member
	 * holds the value SHN_UNDEF.
	*/
	uint16_t      elfSecHeaderTabIndex;
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

#define ELF_IDENT_CLASS 4

#endif

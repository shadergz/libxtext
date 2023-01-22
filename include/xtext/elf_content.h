#ifndef XTEXT_ELF_CONTENT_H
#define XTEXT_ELF_CONTENT_H

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#include <stdbool.h>
#include <stdint.h>

extern const uint8_t elf_signature[4];

#define ELF_IDENT_SZ 16

typedef uint32_t elf32_addr_t;
typedef uint32_t elf32_off_t;

typedef uint32_t elf64_addr_t;
typedef uint64_t elf64_off_t;

typedef struct
{
  uint8_t elfIdent[ELF_IDENT_SZ];
  uint16_t elfType;
  uint16_t elfMachine;
  uint32_t elfVersion;
  elf32_addr_t elf32Entry;
  elf32_off_t elf32Phoff;
  elf32_off_t elf32Shoff;

  uint32_t elfFlags;
  uint16_t elfHeaderSize;
  /* This member holds the size in bytes of one entry in the
   * file's program header table; all entries are the same size. */
  uint16_t elfPHeaderTabSize;
  uint16_t elfPHeaderTableSize;
  /* This member holds a sections header's size in bytes.  A
  section header is one entry in the section header table;
  all entries are the same size. */
  uint16_t elfSecHeaderSize;
  uint16_t elfSecHeaderCount;
  /*
   * This member holds the section header table index of the
   * entry associated with the section name string table.  If
   * the file has no section name string table, this member
   * holds the value SHN_UNDEF.
   */
  uint16_t elfSecHeaderTabIndex;
} elf_header32_t;

typedef struct
{
  uint8_t elfIdent[ELF_IDENT_SZ];
  uint16_t elfType;
  uint16_t elfMachine;
  uint32_t elfVersion;
  elf64_addr_t elf64Entry;
  elf64_off_t elf64Phoff;

  elf64_off_t elf64Shoff;

  uint32_t elfFlags;
  uint16_t elfHeaderSize;
  /* This member holds the size in bytes of one entry in the
   * file's program header table; all entries are the same size. */
  uint16_t elfPHeaderTabSize;
  uint16_t elfPHeaderTableSize;
  /* This member holds a sections header's size in bytes.  A
  section header is one entry in the section header table;
  all entries are the same size. */
  uint16_t elfSecHeaderSize;
  uint16_t elfSecHeaderCount;
  /*
   * This member holds the section header table index of the
   * entry associated with the section name string table.  If
   * the file has no section name string table, this member
   * holds the value SHN_UNDEF.
   */
  uint16_t elfSecHeaderTabIndex;
} elf_header64_t;

typedef struct
{
  union
  {
    elf_header32_t header32Ctx;
    elf_header64_t header64Ctx;
  };
} elf_header_ctx_t;

typedef enum
{
  ELF_DATA_NONE = 0,
  ELF_DATA_2LSB,
  ELF_DATA_2MSB
} elf_endian_e;

typedef enum
{
  ELF_CLASS_32 = 1,
  ELF_CLASS_64
} elf_class_e;

typedef struct
{
  uint32_t secName;
  uint32_t secType;
  uint32_t secFlags;
  elf32_addr_t sec32Addr;
  elf32_off_t sec32Offset;

  uint32_t sec_size;
  uint32_t secLink;
  uint32_t secInfo;
  uint32_t secAddrAlign;
  uint32_t secEntSize;
} elf_sec_hdr32_t;

typedef struct
{
  uint32_t secName;
  uint32_t secType;
  uint32_t secFlags;
  elf64_addr_t sec64Addr;
  elf32_off_t sec32Offset;

  uint32_t sec_size;
  uint32_t secLink;
  uint32_t secInfo;
  uint32_t secAddrAlign;
  uint32_t secEntSize;
} elf_sec_hdr64_t;

typedef struct
{
  union
  {
    elf_sec_hdr32_t header32;
    elf_sec_hdr64_t header64;
  };
} elfsec_hdr_t;

#define ELF_IDENT_CLASS 4

#define SHT_SYM_TAB 2

#endif
